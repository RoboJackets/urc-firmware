#include "Managers/MotorManager.hpp"

namespace manager {

MotorManager::MotorManager(Context &context) {
  motors::RoboClawController *roboClawController = context.getRoboClawController();
  motors::SoloController *soloController = context.getSoloController();
  RequestMessage &requestMessage = context.getRequestMessage();
  DriveEncodersMessage &driveEncodersMessage = context.getDriveEncodersMessage();

  // begin serial communication with motor controller
  // must match baud rate specified in motor controller
  soloController->begin(SOLOMotorControllers::ControlMode::speedMode, 38400);
  roboClawController->begin(38400);

  // initalize all motor objects
  _motors[0].init("soloUNO 1", soloController, 0, 0);
  // _motors[1].init("Right soloUNO Motor", soloController, 1, 1);
  _motors[1].init("RoboClaw", roboClawController, 0x81, 0);
  _motors[2].init("RoboClaw", roboClawController, 0x81, 1);
  

  // _motors[3].init("Right RoboClaw Motor", roboClawController, 0x81, 1);
}

void MotorManager::update(Context &context) {

  DriveEncodersMessage &driveEncodersMessage = context.getDriveEncodersMessage();
  RequestMessage &requestMessage = context.getRequestMessage();

  // check if speed request needs to be serviced
  if (requestMessage.requestSpeed) {

    // write speed commands from requestMessage into each motor
    int32_t *_ticksWrite[NUM_MOTORS];

    _ticksWrite[0] = &requestMessage.leftSpeed;
    _ticksWrite[1] = &requestMessage.rightSpeed;

    bool valid;

    for (size_t i = 0; i < NUM_MOTORS; i++) {
      Motor &motor = _motors[i];
      if (_ticksWrite[i]) {
        motor.setSpeed(*_ticksWrite[i], valid);
      }
    }

    requestMessage.requestSpeed = false;
  }

  // read encoder ticks from each motor, copy into driveEncodersMessage
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

void MotorManager::update(Context &context, RequestMessage message) {
  // DriveEncodersMessage &driveEncodersMessage = context.getDriveEncodersMessage();
  RequestMessage &requestMessage = message;

  // Serial.println("before if");

  // check if speed request needs to be serviced
  if (requestMessage.requestSpeed) {

    // write speed commands from requestMessage into each motor
    int32_t *_ticksWrite[NUM_MOTORS];

    _ticksWrite[0] = &requestMessage.leftSpeed;
    _ticksWrite[1] = &requestMessage.rightSpeed;
    _ticksWrite[2] = &requestMessage.leftSpeed;
    _ticksWrite[3] = &requestMessage.rightSpeed;
    _ticksWrite[4] = &requestMessage.leftSpeed;
    _ticksWrite[5] = &requestMessage.rightSpeed;

    bool valid;

    for (size_t i = 0; i < NUM_MOTORS; i++) {
      Motor motor = _motors[i];
      if (_ticksWrite[i]) {
        motor.setSpeed(*_ticksWrite[i], valid);
      }
    }

    requestMessage.requestSpeed = false;
  }

  // Requesting position control
  if (requestMessage.requestPosition) {

    // write speed commands from requestMessage into each motor
    int32_t *_ticksWrite[NUM_MOTORS];

    _ticksWrite[0] = requestMessage.has_motor0Pos ? &requestMessage.motor0Pos : NULL;
    _ticksWrite[1] = requestMessage.has_motor1Pos ? &requestMessage.motor1Pos : NULL;
    _ticksWrite[2] = requestMessage.has_motor2Pos ? &requestMessage.motor2Pos : NULL;
    _ticksWrite[3] = requestMessage.has_motor3Pos ? &requestMessage.motor3Pos : NULL;
    _ticksWrite[4] = requestMessage.has_motor4Pos ? &requestMessage.motor4Pos : NULL;
    _ticksWrite[5] = requestMessage.has_motor5Pos ? &requestMessage.motor5Pos : NULL;

    bool valid;

    for (size_t i = 0; i < NUM_MOTORS; i++) {
      Motor motor = _motors[i];
      if (&motor != NULL && _ticksWrite[i] != NULL) 
        motor.setPosition(*_ticksWrite[i], valid);
      }
    }

    requestMessage.requestPosition = false;
  }

  // read encoder ticks from each motor, copy into driveEncodersMessage
  // int32_t *_ticksRead[NUM_MOTORS];
  // bool *_validTicksRead[NUM_MOTORS];

  // _validTicksRead[0] = &driveEncodersMessage.has_leftSpeed;
  // _validTicksRead[1] = &driveEncodersMessage.has_rightSpeed;

  // _ticksRead[0] = &driveEncodersMessage.leftSpeed;
  // _ticksRead[1] = &driveEncodersMessage.rightSpeed;

  bool valid;

  for (size_t i = 0; i < NUM_MOTORS; i++) {
    Motor &motor = _motors[i];
    int32_t speed = motor.getSpeed(valid);

    // if (_validTicksRead[i]) *_validTicksRead[i] = valid;
    // if (_ticksRead[i]) *_ticksRead[i] = speed;
  }
}

void MotorManager::Motor::init(const char *name, motors::MotorController *motorController, uint32_t address,
                               uint32_t channel) {
  memcpy(_name, name, MAX_NAME_LEN);
  _motorController = motorController;
  _address = address;
  _channel = channel;
}

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