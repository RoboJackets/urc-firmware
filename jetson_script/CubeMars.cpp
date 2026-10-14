#include "CubeMars.h"

#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstring>

#include <net/if.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <linux/can.h>
#include <linux/can/raw.h>

namespace {

// Servo-mode feedback frames use function ID 0x29 in bits [15:8], motor ID in [7:0].
constexpr uint32_t FEEDBACK_FUNC_ID = 0x29;

// Big-endian packing, as used by the CubeMars protocol.
void put_i32(uint8_t* b, int32_t v) {
    uint32_t u = static_cast<uint32_t>(v);
    b[0] = u >> 24; b[1] = u >> 16; b[2] = u >> 8; b[3] = u;
}
void put_i16(uint8_t* b, int16_t v) {
    uint16_t u = static_cast<uint16_t>(v);
    b[0] = u >> 8; b[1] = u;
}
int16_t get_i16(const uint8_t* b) {
    return static_cast<int16_t>((static_cast<uint16_t>(b[0]) << 8) | b[1]);
}

int32_t scale_i32(double value, double scale, double lo, double hi) {
    value = std::clamp(value, lo, hi);
    return static_cast<int32_t>(std::lround(value * scale));
}

int openCanSocket(const std::string& iface, uint8_t id) {
    int s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0)
        throw std::runtime_error(std::string("socket(PF_CAN) failed: ") + std::strerror(errno));

    struct ifreq ifr {};
    std::strncpy(ifr.ifr_name, iface.c_str(), IFNAMSIZ - 1);
    if (ioctl(s, SIOCGIFINDEX, &ifr) < 0) {
        int e = errno; close(s);
        throw std::runtime_error("CAN interface '" + iface + "' not found: " + std::strerror(e));
    }

    struct sockaddr_can addr {};
    addr.can_family  = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (bind(s, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0) {
        int e = errno; close(s);
        throw std::runtime_error("bind() to '" + iface + "' failed: " + std::strerror(e));
    }

    // Only receive this motor's feedback frames (extended ID 0x29xx).
    struct can_filter filt {};
    filt.can_id   = CAN_EFF_FLAG | ((FEEDBACK_FUNC_ID << 8) | id);
    filt.can_mask = CAN_EFF_FLAG | CAN_EFF_MASK;
    if (setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &filt, sizeof(filt)) < 0) {
        int e = errno; close(s);
        throw std::runtime_error(std::string("CAN filter setup failed: ") + std::strerror(e));
    }
    return s;
}

} // namespace

CubeMarsServo::CubeMarsServo(const std::string& iface, uint8_t id, int timeout_ms)
    : sock_(openCanSocket(iface, id)), id_(id), timeout_ms_(timeout_ms) {}

CubeMarsServo::~CubeMarsServo() {
    if (sock_ >= 0) close(sock_);
}

void CubeMarsServo::sendFrame(CAN_PACKET_ID mode, const uint8_t* data, uint8_t len) {
    struct can_frame frame {};
    frame.can_id  = CAN_EFF_FLAG | (static_cast<uint32_t>(mode) << 8) | id_;
    frame.can_dlc = len;
    if (len > 0) std::memcpy(frame.data, data, len);
    ssize_t n = write(sock_, &frame, sizeof(frame));
    if (n != static_cast<ssize_t>(sizeof(frame)))
        throw std::runtime_error(std::string("CAN write failed: ") + std::strerror(errno));
}

// ---------------------------------------------------------------- commands

void CubeMarsServo::setPosition(float degrees) {
    setPositionVelocity(degrees, default_speed_erpm_, default_accel_);
}

void CubeMarsServo::setDefaultMotion(float speed_erpm, float accel_erpm_s2) {
    default_speed_erpm_ = speed_erpm;
    default_accel_      = accel_erpm_s2;
}

void CubeMarsServo::setPositionVelocity(float deg, float speed_erpm, float accel_erpm_s2) {
    uint8_t buf[8];
    // position: int32, degrees * 10000 (range +-36000 deg)
    put_i32(buf, scale_i32(deg, 10000.0, -36000.0, 36000.0));
    // speed: int16, 1 unit = 10 ERPM
    put_i16(buf + 4, static_cast<int16_t>(scale_i32(speed_erpm, 0.1, -32768.0, 32767.0)));
    // acceleration: int16 (0..32767), 1 unit = 10 ERPM/s^2
    put_i16(buf + 6, static_cast<int16_t>(scale_i32(accel_erpm_s2, 0.1, 0.0, 32767.0)));
    sendFrame(CAN_PACKET_SET_POS_SPD, buf, 8);
}

void CubeMarsServo::setSpeed(float rpm) {   // value is ERPM, see header
    uint8_t buf[4];
    put_i32(buf, scale_i32(rpm, 1.0, -100000.0, 100000.0));
    sendFrame(CAN_PACKET_SET_RPM, buf, 4);
}

void CubeMarsServo::setEffort(float amps) {
    uint8_t buf[4];
    put_i32(buf, scale_i32(amps, 1000.0, -60.0, 60.0));   // mA
    sendFrame(CAN_PACKET_SET_CURRENT, buf, 4);
}

void CubeMarsServo::setBrake(float amps) {
    uint8_t buf[4];
    put_i32(buf, scale_i32(amps, 1000.0, 0.0, 60.0));     // mA
    sendFrame(CAN_PACKET_SET_CURRENT_BRAKE, buf, 4);
}

void CubeMarsServo::setOrigin(bool permanent) {
    uint8_t mode = permanent ? 1 : 0;
    sendFrame(CAN_PACKET_SET_ORIGIN_HERE, &mode, 1);
}

void CubeMarsServo::stop()    { setSpeed(0.0f); }
void CubeMarsServo::release() { setEffort(0.0f); }

// ---------------------------------------------------------------- feedback

bool CubeMarsServo::parseFeedback(const uint8_t* d, uint8_t len) {
    if (len < 8) return false;
    state_.position_deg  = get_i16(d + 0) * 0.1f;
    state_.speed_erpm    = get_i16(d + 2) * 10.0f;
    state_.current_amps  = get_i16(d + 4) * 0.01f;
    state_.temperature_c = static_cast<int8_t>(d[6]);
    state_.error_code    = d[7];
    have_feedback_  = true;
    last_feedback_  = std::chrono::steady_clock::now();
    return true;
}

bool CubeMarsServo::update() {
    bool got = false;
    for (;;) {
        struct pollfd p { sock_, POLLIN, 0 };
        if (poll(&p, 1, 0) <= 0) break;
        struct can_frame frame {};
        ssize_t n = read(sock_, &frame, sizeof(frame));
        if (n != static_cast<ssize_t>(sizeof(frame))) break;
        if (parseFeedback(frame.data, frame.can_dlc)) got = true;
    }
    return got;
}

bool CubeMarsServo::waitForFeedback(int timeout_ms) {
    if (timeout_ms < 0) timeout_ms = timeout_ms_;
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout_ms);
    for (;;) {
        auto remaining = std::chrono::duration_cast<std::chrono::milliseconds>(
            deadline - std::chrono::steady_clock::now()).count();
        if (remaining < 0) remaining = 0;
        struct pollfd p { sock_, POLLIN, 0 };
        if (poll(&p, 1, static_cast<int>(remaining)) > 0 && update()) return true;
        if (remaining == 0) return false;
    }
}

bool CubeMarsServo::feedbackFresh(int max_age_ms) const {
    if (!have_feedback_) return false;
    auto age = std::chrono::steady_clock::now() - last_feedback_;
    return age <= std::chrono::milliseconds(max_age_ms);
}
