#include "MotorController/MotorController.hpp"

namespace motors {

int32_t RoboClawController::getSpeed(uint8_t channel, bool &valid) {
    bool success;
    int32_t speed = 0;

    if (channel == 0) {
        speed = _roboclaw.ReadSpeedM1(_address, nullptr, &success);
        valid = success;
    } else if (channel == 1) {
        speed = _roboclaw.ReadSpeedM2(_address, nullptr, &success);
        valid = success;
    } else {
        valid = false;
    }

    return speed;
}

void RoboClawController::setSpeed(int32_t speed, uint8_t channel, bool &valid) {

    if (channel == 0) {
        valid = _roboclaw.SpeedM1(_address, speed);
    } else if (channel == 1) {
        valid = _roboclaw.SpeedM2(_address, speed);
    } else {
        valid = false;
    }

}

}  // namespace motors