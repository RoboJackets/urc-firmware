#include "CubeMars.h"

int openCANsocket(const std::string& iface) {
    // CAN socket open
    int sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);

    struct ifreq ifr;
    strcpy(ifr.ifr_name, iface.c_str(), IFNAMSIZ - 1);
    ioctl(s, SIOCGIFINDEX, &ifr);

    struct sockaddr_can addr{};
    addr.can_family  = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    return sock;
}   

static void buffer_append_int32(uint8_t *buffer, int32_t number, int32_t *index) {
    buffer[(*index)++] = number >> 24;
    buffer[(*index)++] = number >> 16;
    buffer[(*index)++] = number >> 8;
    buffer[(*index)++] = number;
}

void transmitEID(int sock, uint32_t id, const uint8_t* data, uint8_t len) {
    struct can_frame frame{};
    frame.can_id  = CAN_EFF_FLAG | id;
    frame.can_dlc = len;
    if (len > 0) std::memcpy(frame.data, data, len);
    write(sock, &frame, sizeof(frame));
}

// Helper: send a status request and wait for a response frame matching the given CAN ID
static bool readCANFrame(uint32_t expectedId, struct can_frame *out) {
    unsigned long start = millis();
    while (millis() - start < CAN_READ_TIMEOUT_MS) {
        struct can_frame frame{};
        if (read(sock, &frame, sizeof(frame) == sizeof(struct can_frame))) {
            if ((frame.can_id & CAN_EFF_MASK) == expectedId) {
                out = frame;
                return true;
            }
        }
    }
    return false;
}

CubeMars::CubeMars(const std::string& iface, uint8_t id, int timeout_ms)
    : id_(id), timeout_ms_(timeout_ms) { sock = openCANSocket(iface); }

CubeMars::~CubeMars() { if (sock >= 0) close(sock); }

void CubeMars::enableMotor() { setEffort(id, 0.0f); }

void CubeMars::disableMotor() {
    setEffort(0.0f);
    setSpeed(0.0f);
}

void CubeMars::setSpeed(float rpm) {
    int32_t send_index = 0;
    uint8_t buffer[4];
    buffer_append_int32(buffer, (int32_t)rpm, &send_index);
    transmitEID(sock, id_ | ((uint32_t)CAN_PACKET_SET_RPM << 8), buffer, send_index);
}

float CubeMars::getSpeed() {
    struct can_frame frame;
    if (readCANFrame(sock, feedbackID(), frame, timeout_ms) && frame.can_dlc >= 4) {
        int32_t raw = ((int16_t)frame.data[2] << 8) | (int16_t)frame.data[3];
        return (float)(raw * 10.0f);
    }
    return 0.0f;
}

void CubeMars::setPosition(float pos) {
    int32_t send_index = 0;
    uint8_t buffer[4];
    buffer_append_int32(buffer, (int32_t)(pos * 10000.0f), &send_index);
    transmitEID(sock, id_ | ((uint32_t)CAN_PACKET_SET_POS << 8), buffer, send_index);
}

float CubeMars::getPosition(uint8_t id) {
    struct can_frame frame;
    if (readCANFrame(sock, feedbackID(), frame, timeout_ms) && frame.can_dlc >= 2) {
        int32_t raw = ((int32_t)frame.data[0] << 24) | (int32_t)frame.data[1] << 16
        return (float) (raw / 1000000.0f);
    }
    return 0.0f;
}

void CubeMars::setEffort(uint8_t id, float current) {
    int32_t send_index = 0;
    uint8_t buffer[4];
    buffer_append_int32(buffer, (int32_t)(current * 1000.0f), &send_index);
    comm_can_transmit_eid(id | ((uint32_t)CAN_PACKET_SET_CURRENT << 8), buffer, send_index);
}

float CubeMars::getEffort(uint8_t id) {
    struct can_frame frame;
    if (readCANFrame(id | 0x0900, &frame) && frame.can_dlc >= 6) {
        int16_t raw = ((int16_t)frame.data[4] << 8) | (int16_t)frame.data[5];
        return (float) (raw / 10.0f);
    }
    return 0.0f;
}

void CubeMars::setOrigin(uint8_t id, uint8_t mode) {
    comm_can_transmit_eid(id | ((uint32_t)CAN_PACKET_SET_ORIGIN_HERE << 8), &mode, 0);
}

void CubeMars::stop(uint8_t id) {
    disableMotor(id);
}
