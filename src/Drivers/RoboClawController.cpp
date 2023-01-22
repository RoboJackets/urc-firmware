#include "Drivers/RoboClawController.hpp"

namespace motors {

void RoboClawController::begin(long speed) {
  _roboclaw.begin(speed);
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

  return speed;
}

void RoboClawController::setSpeed(int32_t speed, uint8_t address, uint8_t channel, bool &valid) {

  if (channel == 0) {
    valid = _roboclaw.SpeedM1(address, speed);
  } else if (channel == 1) {
    valid = _roboclaw.SpeedM2(address, speed);
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

void RoboClawController::setRawPosition(int32_t position, uint8_t address, uint8_t channel, bool &valid) {
  if (channel == 0) {
    // if (valid)
    valid = _roboclaw.SpeedDistanceM1(address, _roboclaw.ReadSpeedM1(address), position);
  } else if (channel == 1) {
    valid = _roboclaw.SpeedDistanceM2(address, _roboclaw.ReadSpeedM2(address), position);
  } else {
    valid = false;
  }
}

}  // namespace motors