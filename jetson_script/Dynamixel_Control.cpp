#include <iostream>
#include <string>
#include "Dynamixel.h"

int main() {
    const char* deviceName = "/dev/ttyUSB0"; 

    uint8_t id = 1;  // Change if your motor has a different ID

    // Initialize communication
    init(deviceName);

    std::cout << "Dynamixel Position Control Test\n";
    std::cout << "Connecting to motor ID " << (int)id << "...\n";

    // Enable motor torque
    enableTorque(id);

    std::string input;

    while (true) {
        std::cout << "\nEnter goal position (or 'q' to quit): ";
        std::cin >> input;

        if (input == "q") {
            break;
        }

        try {
            int position = std::stoi(input);

            // Send command
            setPosition(id, position);

            // Read back position
            int32_t currentPos = getPosition(id);

            std::cout << "Commanded: " << position
                      << " | Current Position: " << currentPos << "\n";

        } catch (...) {
            std::cout << "Invalid input. Enter an integer.\n";
        }
    }

    // Disable torque before exiting
    stop(id);

    std::cout << "Motor stopped. Exiting.\n";
    return 0;
}
