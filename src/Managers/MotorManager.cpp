#include "Managers/MotorManager.hpp"

namespace manager {

MotorManager::MotorManager(Context &context) {
  motors::RoboClawController *roboClawController = context.getRoboClawController();
  // motors::SoloController *soloController = context.getSoloController();
  // RequestMessage &requestMessage = context.getRequestMessage();
  // DriveEncodersMessage &driveEncodersMessage = context.getDriveEncodersMessage();

  // begin serial communication with motor controller
  // must match baud rate specified in motor controller
  // soloController->begin(SOLOMotorControllers::ControlMode::speedMode, 38400);
  roboClawController->begin(38400);

  // initalize all motor objects
  _motor["Left Motor"] = Motor("Left Motor", roboClawController, 0x80, 0);
  _motor["Right Motor"] = Motor("Right Motor", roboClawController, 0x81, 1);
  // _motor["Solo UNO"] = Motor("Solo UNO", soloController, 1, 1);
}

void MotorManager::update(Context &context) {

  DriveEncodersMessage &driveEncodersMessage = context.getDriveEncodersMessage();
  RequestMessage &requestMessage = context.getRequestMessage();

  // check if speed request needs to be serviced
  if (requestMessage.requestSpeed) {

    // write speed commands from requestMessage into each motor
    bool valid;
    _motor["Left Motor"].setSpeed(requestMessage.leftSpeed, valid);
    _motor["Right Motor"].setSpeed(requestMessage.rightSpeed, valid);

    requestMessage.requestSpeed = false;
  }

  // read encoder ticks from each motor, copy into driveEncodersMessage
  driveEncodersMessage.leftSpeed = _motor["Left Motor"].getSpeed(driveEncodersMessage.has_leftSpeed);
  driveEncodersMessage.rightSpeed = _motor["Right Motor"].getSpeed(driveEncodersMessage.has_rightSpeed);
}

// void MotorManager::update(Context &context, RequestMessage message) {
//   // DriveEncodersMessage &driveEncodersMessage = context.getDriveEncodersMessage();
//   RequestMessage &requestMessage = message;

//   // Serial.println("before if");

//   // check if speed request needs to be serviced
//   if (requestMessage.requestSpeed) {

//     // write speed commands from requestMessage into each motor
//     int32_t *_ticksWrite[NUM_MOTORS];

//     _ticksWrite[0] = &requestMessage.leftSpeed;
//     _ticksWrite[1] = &requestMessage.rightSpeed;
//     _ticksWrite[2] = &requestMessage.leftSpeed;
//     _ticksWrite[3] = &requestMessage.rightSpeed;
//     _ticksWrite[4] = &requestMessage.leftSpeed;
//     _ticksWrite[5] = &requestMessage.rightSpeed;

//     bool valid;

//     for (size_t i = 0; i < NUM_MOTORS; i++) {
//       Motor &motor = _motors[i];
//       if (_ticksWrite[i]) {
//         motor.setSpeed(*_ticksWrite[i], valid);
//       }
//     }

//     requestMessage.requestSpeed = false;
//   }

//   // Requesting position control
//   if (requestMessage.requestPosition) {

//     // write speed commands from requestMessage into each motor
//     int32_t *_ticksWrite[NUM_MOTORS];

//     _ticksWrite[0] = &requestMessage.motor0Pos;
//     _ticksWrite[1] = &requestMessage.motor1Pos;
//     _ticksWrite[2] = &requestMessage.motor2Pos;
//     _ticksWrite[3] = &requestMessage.motor3Pos;
//     _ticksWrite[4] = &requestMessage.motor4Pos;
//     _ticksWrite[5] = &requestMessage.motor5Pos;

//     bool valid;

//     for (size_t i = 0; i < NUM_MOTORS; i++) {
//       Motor &motor = _motors[i];
//       if (_ticksWrite[i]) {
//         motor.setPosition(*_ticksWrite[i], valid);
//       }
//     }

//     requestMessage.requestSpeed = false;
//   }

//   // read encoder ticks from each motor, copy into driveEncodersMessage
//   // int32_t *_ticksRead[NUM_MOTORS];
//   // bool *_validTicksRead[NUM_MOTORS];

//   // _validTicksRead[0] = &driveEncodersMessage.has_leftSpeed;
//   // _validTicksRead[1] = &driveEncodersMessage.has_rightSpeed;

//   // _ticksRead[0] = &driveEncodersMessage.leftSpeed;
//   // _ticksRead[1] = &driveEncodersMessage.rightSpeed;

//   bool valid;

//   for (size_t i = 0; i < NUM_MOTORS; i++) {
//     Motor &motor = _motors[i];
//     int32_t speed = motor.getSpeed(valid);

//     // if (_validTicksRead[i]) *_validTicksRead[i] = valid;
//     // if (_ticksRead[i]) *_ticksRead[i] = speed;
//   }
// }

MotorManager::Motor::Motor() 
  : Motor("", nullptr, 0, 0) {}

MotorManager::Motor::Motor(std::string name, motors::MotorController *motorController, uint32_t address, uint32_t channel) 
  : _name(name), _motorController(motorController), _address(address), _channel(channel) {}

int32_t MotorManager::Motor::getSpeed(bool &valid) {
  if (_motorController) {
    return _motorController->getSpeed(_address, _channel, valid);
  } else {
    valid = false;
    return -1;
  }
}

void MotorManager::Motor::setSpeed(int32_t speed, bool &valid) {
  if (_motorController) {
    _motorController->setSpeed(speed, _address, _channel, valid);
  } else {
    valid = false;
  }
}

int32_t MotorManager::Motor::getPosition(bool &valid) {
  if (_motorController) {
    return _motorController->getPosition(_address, _channel, valid);
  } else {
    valid = false;
    return -1;
  }
}

void MotorManager::Motor::setPosition(int32_t speed, bool &valid) {
  if (_motorController) {
    _motorController->setPosition(speed, _address, _channel, valid);
  } else {
    valid = false;
  }
}

}  // namespace manager