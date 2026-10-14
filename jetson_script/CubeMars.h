#ifndef CUBEMARS_H
#define CUBEMARS_H

// CubeMars AK-series driver (servo mode) over Linux SocketCAN.
// Protocol reference: AK Series Module Driver Manual v1.0.17, section 5
// (extended 29-bit frames, 1 Mbps). Requires C++17 and Linux.
//
// Motor must be in SERVO mode, with "Send status over CAN" enabled in
// CubeMars Tool (Application Functions) so feedback frames are streamed.

#if __has_include("Motor.h")
#include "Motor.h"
#elif __has_include("../Motor.h")
#include "../Motor.h"
#else
#error "Motor.h not found in the local include path or project root."
#endif

#include <cstdint>
#include <string>
#include <stdexcept>
#include <chrono>

enum CAN_PACKET_ID : uint8_t {
    CAN_PACKET_SET_DUTY = 0,
    CAN_PACKET_SET_CURRENT,
    CAN_PACKET_SET_CURRENT_BRAKE,
    CAN_PACKET_SET_RPM,
    CAN_PACKET_SET_POS,
    CAN_PACKET_SET_ORIGIN_HERE,
    CAN_PACKET_SET_POS_SPD
};

// Latest values decoded from the motor's 0x29 feedback frame.
struct MotorState {
    float   position_deg  = 0.0f;   // int16 * 0.1 deg
    float   speed_erpm    = 0.0f;   // int16 * 10 ERPM (electrical RPM)
    float   current_amps  = 0.0f;   // int16 * 0.01 A
    int8_t  temperature_c = 0;
    uint8_t error_code    = 0;
};

class CubeMarsServo : public Motor {
public:
    // iface: e.g. "can0". id: the motor's Controller ID (0-255).
    // timeout_ms: default wait used by waitForFeedback().
    CubeMarsServo(const std::string& iface, uint8_t id, int timeout_ms = 100);
    ~CubeMarsServo() override;

    // Not copyable (Motor forbids it) and not movable: hold in a unique_ptr
    // if you need to put motors in a container.
    CubeMarsServo(CubeMarsServo&&)            = delete;
    CubeMarsServo& operator=(CubeMarsServo&&) = delete;

    // ---- Motor interface (signatures must match Motor.h exactly) ----

    // Velocity loop. NOTE: the value is sent as ERPM (electrical RPM), not
    // output-shaft RPM. Convert with the motor's pole pairs and gear ratio.
    void setSpeed(float rpm) override;
    // Multi-loop position in degrees (absolute from the current origin,
    // range -36000..36000), using the default speed/accel below.
    void setPosition(float degrees) override;
    // Motor.h declares the getters as void, so these only refresh the
    // cached feedback. Read the values with positionDeg() etc. below.
    void getSpeed() override    { update(); }
    void getPosition() override { update(); }
    void getEffort(float) override { update(); }   // argument unused
    // Motor.h's no-argument setEffort(): 0 A (coast). Use setEffort(amps) for current.
    void setEffort() override   { release(); }
    void stop() override;       // velocity loop at 0: actively holds still

    // ---- Extra commands (all throw std::runtime_error if the CAN write fails) ----

    // Position-velocity loop. speed in ERPM, accel in ERPM/s^2.
    void setPositionVelocity(float deg, float speed_erpm, float accel_erpm_s2);
    // Defaults used by setPosition(). Initial values: 5000 ERPM, 3000 ERPM/s^2.
    void setDefaultMotion(float speed_erpm, float accel_erpm_s2);
    // Current loop, in amps (signed).
    void setEffort(float amps);
    // Current brake, in amps (>= 0).
    void setBrake(float amps);
    // Make the current position 0 deg. Temporary origin (lost on power cycle)
    // unless permanent is true (dual-encoder models only).
    void setOrigin(bool permanent = false);
    void release();   // 0 A: motor coasts, no holding torque

    // ---- Feedback ----

    // Drain pending feedback frames without blocking. True if any arrived.
    bool update();
    // Block up to timeout_ms (default: constructor value) for fresh feedback.
    bool waitForFeedback(int timeout_ms = -1);
    // True if feedback was received within the last max_age_ms.
    bool feedbackFresh(int max_age_ms) const;

    // Latest cached values (call update() / getPosition() etc. first).
    float   positionDeg()   const { return state_.position_deg; }
    float   speedErpm()     const { return state_.speed_erpm; }
    float   currentAmps()   const { return state_.current_amps; }
    int8_t  temperatureC()  const { return state_.temperature_c; }
    uint8_t errorCode()     const { return state_.error_code; }
    const MotorState& state() const { return state_; }
    uint8_t id() const { return id_; }

protected:
    void sendFrame(CAN_PACKET_ID mode, const uint8_t* data, uint8_t len);
    bool parseFeedback(const uint8_t* data, uint8_t len);

    int      sock_;
    uint8_t  id_;
    int      timeout_ms_;
    float    default_speed_erpm_  = 5000.0f;
    float    default_accel_       = 3000.0f;
    MotorState state_;
    bool     have_feedback_ = false;
    std::chrono::steady_clock::time_point last_feedback_;
};

#endif