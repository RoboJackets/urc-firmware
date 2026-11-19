// Multi-loop position test for a CubeMars AK45-10 (default Controller ID 201).
// Marks the current position as 0, moves to a target (default 3600 deg of the motor's
// OUTPUT shaft = 10 turns) at the given speed/acceleration, then returns to 0.
// It resends the target every cycle so the motor's CAN timeout never trips, and it stops
// the motor if anything looks wrong (no feedback, error code, high current/temperature,
// Ctrl+C).
//
// Build: g++ -std=c++17 -Wall -Wextra CubeMars.cpp cubemars_test.cpp -o cubemars_test
// Run:   ./cubemars_test can0 [motor_id] [target_deg] [speed_erpm] [accel_erpm_s2]
//        defaults: can0, 201, 3600, 15000, 5000   (speed/accel match CubeMars Tool's des S / des A)
#include "CubeMars.h"

#include <chrono>
#include <cmath>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <thread>

namespace {

constexpr int   DEFAULT_MOTOR_ID  = 201;
constexpr float DEFAULT_TARGET    = 3600.0f;
constexpr float DEFAULT_SPEED     = 15000.0f;
constexpr float DEFAULT_ACCEL     = 5000.0f;

// Hard caps: the program refuses to run above these.
constexpr float MAX_TARGET_DEG    = 36000.0f;  // protocol range
constexpr float MAX_SPEED_ERPM    = 20000.0f;  // ~25000 ERPM is the limit at 24 V
constexpr float MAX_ACCEL         = 10000.0f;

constexpr float ORIGIN_TOL_DEG    = 2.0f;      // after Set Origin the position must read ~0
constexpr float ARRIVE_DEG        = 1.5f;      // "arrived" tolerance
constexpr float ARRIVE_SPEED_ERPM = 100.0f;    // ...and nearly stopped
constexpr float MAX_CURRENT_A     = 3.5f;      // abort above this (rated ~2.1 A, peak 5 A)
constexpr int   MAX_TEMP_C        = 75;        // abort above this
constexpr int   MOVE_TIMEOUT_MS   = 30000;
constexpr int   STALE_MS          = 500;       // abort if feedback is older than this
constexpr int   CYCLE_MS          = 100;       // resend rate for the position command

volatile std::sig_atomic_t g_interrupted = 0;
void onSigint(int) { g_interrupted = 1; }

// True if the motor looks safe; otherwise prints the reason and returns false.
bool healthy(CubeMarsServo& m) {
    if (!m.feedbackFresh(STALE_MS)) { std::cerr << "ABORT: feedback is stale\n"; return false; }
    const MotorState& s = m.state();
    if (s.error_code != 0) { std::cerr << "ABORT: motor error code " << int(s.error_code) << "\n"; return false; }
    if (std::fabs(s.current_amps) > MAX_CURRENT_A) { std::cerr << "ABORT: current " << s.current_amps << " A\n"; return false; }
    if (s.temperature_c > MAX_TEMP_C) { std::cerr << "ABORT: temperature " << int(s.temperature_c) << " C\n"; return false; }
    return true;
}

// Command a position and wait until it is reached. False means stop the test.
bool moveTo(CubeMarsServo& m, float target_deg, float speed, float accel) {
    std::cout << "-> " << target_deg << " deg  (speed " << speed << " ERPM, accel " << accel << ")\n";
    const auto start = std::chrono::steady_clock::now();
    for (;;) {
        m.setPositionVelocity(target_deg, speed, accel);   // resent each cycle
        std::this_thread::sleep_for(std::chrono::milliseconds(CYCLE_MS));
        if (g_interrupted) { std::cerr << "Interrupted.\n"; return false; }
        m.update();
        if (!healthy(m)) return false;
        const MotorState& s = m.state();
        std::cout << "   pos " << s.position_deg << " deg, speed " << s.speed_erpm
                  << " ERPM, " << s.current_amps << " A, " << int(s.temperature_c) << " C\n";
        if (std::fabs(s.position_deg - target_deg) < ARRIVE_DEG && std::fabs(s.speed_erpm) < ARRIVE_SPEED_ERPM)
            return true;
        const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start).count();
        if (ms > MOVE_TIMEOUT_MS) { std::cerr << "ABORT: move timed out\n"; return false; }
    }
}

} // namespace

int main(int argc, char** argv) {
    const std::string iface = (argc > 1) ? argv[1] : "can0";
    const int   id     = (argc > 2) ? std::atoi(argv[2]) : DEFAULT_MOTOR_ID;
    const float target = (argc > 3) ? std::strtof(argv[3], nullptr) : DEFAULT_TARGET;
    const float speed  = (argc > 4) ? std::strtof(argv[4], nullptr) : DEFAULT_SPEED;
    const float accel  = (argc > 5) ? std::strtof(argv[5], nullptr) : DEFAULT_ACCEL;

    if (id < 0 || id > 255) { std::cerr << "Motor ID must be 0-255.\n"; return 1; }
    if (std::fabs(target) > MAX_TARGET_DEG) { std::cerr << "Target must be within +/-" << MAX_TARGET_DEG << " deg.\n"; return 1; }
    if (speed <= 0 || speed > MAX_SPEED_ERPM) { std::cerr << "Speed must be 1-" << MAX_SPEED_ERPM << " ERPM.\n"; return 1; }
    if (accel <= 0 || accel > MAX_ACCEL) { std::cerr << "Accel must be 1-" << MAX_ACCEL << " ERPM/s^2.\n"; return 1; }
    std::signal(SIGINT, onSigint);

    try {
        CubeMarsServo motor(iface, static_cast<uint8_t>(id));

        if (!motor.waitForFeedback(1000)) {
            std::cerr << "No feedback - not moving the motor.\n";
            return 1;
        }
        motor.update();
        if (!healthy(motor)) return 1;

        std::cout << "Feedback OK from motor ID " << id << ". Starting at "
                  << motor.state().position_deg << " deg.\n";

        motor.setOrigin();                  // current position becomes 0 deg
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        motor.update();
        if (!healthy(motor)) return 1;
        if (std::fabs(motor.state().position_deg) > ORIGIN_TOL_DEG) {
            std::cerr << "ABORT: origin did not reset (position reads " << motor.state().position_deg << " deg)\n";
            return 1;
        }
        std::cout << "Origin set (position reads " << motor.state().position_deg << " deg).\n";

        bool ok = moveTo(motor, target, speed, accel) && moveTo(motor, 0.0f, speed, accel);

        motor.stop();                       // always leave the motor holding still
        std::cout << (ok ? "Test finished.\n" : "Test stopped early; motor told to stop.\n");
        return ok ? 0 : 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}
