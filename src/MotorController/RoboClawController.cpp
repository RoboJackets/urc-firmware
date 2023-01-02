#include "MotorController/MotorController.hpp"

namespace motors {

int32_t RoboClawController::getSpeed(uint8_t channel) {
    uint8_t status;
    bool valid;
    // return _roboclaw.ReadSpeedM1(_address, &status, &valid);
}

}  // namespace motors