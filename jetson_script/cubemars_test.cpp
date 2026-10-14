// First on-motor test: moves the AK45-10 (ID 201) +90, -90, then back to 0.
// Build: g++ -std=c++17 -Wall -Wextra CubeMars.cpp cubemars_test.cpp -o cubemars_test
// Run:   ./cubemars_test can0
#include "CubeMars.h"
#include <chrono>
#include <iostream>
#include <thread>

int main(int argc, char** argv) {
    const std::string iface = (argc > 1) ? argv[1] : "can0";
    try {
        CubeMarsServo motor(iface, 201);

        if (!motor.waitForFeedback(1000))
            std::cerr << "No feedback yet - check 'Send status over CAN' and the Controller ID.\n";

        motor.setOrigin();                    // current position becomes 0 deg
        motor.setDefaultMotion(5000, 3000);   // ERPM, ERPM/s^2

        for (float target : {90.0f, -90.0f, 0.0f}) {
            std::cout << "-> " << target << " deg\n";
            motor.setPosition(target);
            for (int i = 0; i < 20; ++i) {    // ~2 s
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                motor.update();
                const MotorState& s = motor.state();
                std::cout << "  pos " << s.position_deg << " deg, speed " << s.speed_erpm
                          << " ERPM, " << s.current_amps << " A, " << int(s.temperature_c)
                          << " C, err " << int(s.error_code) << "\n";
            }
        }
        motor.stop();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}