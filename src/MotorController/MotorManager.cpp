#include "MotorController/MotorController.hpp"

namespace motors {

#include "MotorController/MotorController.hpp"

void MotorManager::Motor::init(char *name, MotorController *motorController, uint8_t channel) {
    memcpy(_name, name, MAX_NAME_LEN);
    _motorController = motorController;
    _channel = channel;
}

int32_t MotorManager::Motor::getSpeed() {
    _motorController->getSpeed(_channel);
}

void MotorManager::setup(Context &context) {
    RoboClawController &roboClawController = context.getRoboClawController();

    motors[0].init("Left Motor", &roboClawController, 0); 
}

}