#include "Managers.hpp"

namespace manager {

MotorManager::MotorManager(Context &context) {
    motors::RoboClawController *roboClawController = context.getRoboClawController();
    RequestMessage &requestMessage = context.getRequestMessage();
    DriveEncodersMessage &driveEncodersMessage = context.getDriveEncodersMessage();

    _motors[0].init("Left Motor", roboClawController, 0);
    _motors[1].init("Right Motor", roboClawController, 1);
}

void MotorManager::update(Context &context) {
    
    DriveEncodersMessage &driveEncodersMessage = context.getDriveEncodersMessage();
    RequestMessage &requestMessage = context.getRequestMessage();
    
    if (requestMessage.requestSpeed) {
        
        int32_t *_ticksWrite[NUM_MOTORS];

        _ticksWrite[0] = &requestMessage.leftSpeed;
        _ticksWrite[1] = &requestMessage.rightSpeed;

        bool valid;

        for (size_t i = 0; i < NUM_MOTORS; i++) {
            Motor &motor = _motors[i];
            if (_ticksWrite[i]) motor.setSpeed(*_ticksWrite[i], valid);
        }
    }

    int32_t *_ticksRead[NUM_MOTORS];
    bool *_validTicksRead[NUM_MOTORS];

    _validTicksRead[0] = &driveEncodersMessage.has_leftSpeed;
    _validTicksRead[1] = &driveEncodersMessage.has_rightSpeed;

    _ticksRead[0] = &driveEncodersMessage.leftSpeed;
    _ticksRead[1] = &driveEncodersMessage.rightSpeed;

    bool valid;

    for (size_t i = 0; i < NUM_MOTORS; i++) {
        Motor &motor = _motors[i];
        int32_t speed = motor.getSpeed(valid);

        if (_validTicksRead[i]) *_validTicksRead[i] = valid;
        if (_ticksRead[i]) *_ticksRead[i] = speed;
    }

}

void MotorManager::Motor::init(const char *name, motors::MotorController *motorController, uint32_t channel) {
    memcpy(_name, name, MAX_NAME_LEN);
    _motorController = motorController;
    _channel = channel;
}

int32_t MotorManager::Motor::getSpeed(bool &valid) {
    if (_motorController) {
        return _motorController->getSpeed(_channel, valid);
    } else {
        valid = false;
        return 0;
    }
}

void MotorManager::Motor::setSpeed(int32_t speed, bool &valid) {
    if (_motorController) {
        _motorController->setSpeed(speed, _channel, valid);
    } else {
        valid = false;
    }
}


}