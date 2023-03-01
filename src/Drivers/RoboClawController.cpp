#include "Drivers/RoboClawController.hpp"

namespace motors {

void RoboClawController::begin(long speed) {
  _roboclaw.begin(speed);
  init_pid();

  bool a = false;

  // zero(0x081, 0, 0);
}

int32_t RoboClawController::init_pid() {
  _roboclaw.SetM1VelocityPID(0x081, 0.03771, 0.00349, 0.0, 797812);
  _roboclaw.SetM2VelocityPID(0x081, 0.03771, 0.00349, 0.0, 797812);

  _roboclaw.SetM1PositionPID(0x081, 53.34633, 1.54104, 462.31150, 5428, 1000, 0, 4294967295);
}

int32_t RoboClawController::init_pid(uint8_t address = 0x081, float Kp = 0.03771, float Ki = 0.00349, float Kd = 0.0, uint32_t qpps = 797812) {
  _roboclaw.SetM1VelocityPID(address, Kp, Ki, Kd, qpps);
  _roboclaw.SetM2VelocityPID(address, Kp, Ki, Kd, qpps);
}

int32_t RoboClawController::getSpeed(uint8_t address, uint8_t channel, bool &valid) {
  bool success;
  int32_t speed = 0;

  if (channel == 0) {
    speed = _roboclaw.ReadSpeedM1(address, nullptr, &success);
    valid = success;
  } else if (channel == 1) {
    speed = _roboclaw.ReadSpeedM2(address, nullptr, &success);
    valid = success;
  } else {
    valid = false;
  }

  // Roboclaws run at 2048 ticks per rotation
  // SpeedM1 sets ticks per second
  // Converting from TPS to RPM
  return speed * 60 / 2048;
}

void RoboClawController::setSpeed(int32_t speed, uint8_t address, uint8_t channel, bool &valid) {

  // Roboclaws run at 2048 ticks per rotation
  // SpeedM1 sets ticks per second
  // Converting from RPM to TPS
  if (channel == 0) {
    valid = _roboclaw.SpeedM1(address, speed / 60 * 2048);
  } else if (channel == 1) {
    valid = _roboclaw.SpeedM2(address, speed / 60 * 2048);
  } else {
    valid = false;
  }
}

int32_t RoboClawController::getRawPosition(uint8_t address, uint8_t channel, bool &valid){
  bool success;
  int32_t position = 0;

  if (channel == 0) {
    position = _roboclaw.ReadEncM1(address, nullptr, &success);
    valid = success;
  } else if (channel == 1) {
    position = _roboclaw.ReadEncM2(address, nullptr, &success);
    valid = success;
  } else {
    valid = false;
  }

  return position;
}

void RoboClawController::setRawPosition(int32_t position, uint32_t maxSpeed, uint8_t address, uint8_t channel, bool &valid) {
  if (channel == 0) {
    valid = _roboclaw.SpeedDistanceM1(address, _roboclaw.ReadSpeedM1(address), position);
  } else if (channel == 1) {
    valid = _roboclaw.SpeedDistanceM2(address, _roboclaw.ReadSpeedM2(address), position);
  } else {
    valid = false;
  }
}

void RoboClawController::setRawPosition(int32_t position, uint8_t address, uint8_t channel, bool &valid) {
  setRawPosition(position, 0, address, channel, valid);
}

}  // namespace motors