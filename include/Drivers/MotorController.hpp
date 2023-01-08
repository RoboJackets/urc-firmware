#ifndef MOTORCONTROLLER_HPP
#define MOTORCONTROLLER_HPP

#include <Arduino.h>
#include "RoboClaw.h"

namespace motors {

enum MotorMode { TORQUE, SPEED, VELOCITY };

class MotorController {
public:
  MotorController(){};
  virtual void begin(long speed);
  // TODO: virtual void setMode(uint8_t channel, MotorMode mode) = 0;
  virtual int32_t getSpeed(uint8_t address, uint8_t channel, bool &valid) = 0;
  virtual void setSpeed(int32_t speed, uint8_t address, uint8_t channel, bool &valid) = 0;
  // TODO: virtual int32_t getPosition(uint8_t channel) = 0;
  // TODO: virtual void setPosition(uint8_t channel) = 0;
};

class RoboClawController : public MotorController {
public:
  RoboClawController(RoboClaw &roboclaw) : _roboclaw(roboclaw){};
  void begin(long speed);
  int32_t getSpeed(uint8_t address, uint8_t channel, bool &valid) override;
  void setSpeed(int32_t speed, uint8_t address, uint8_t channel, bool &valid) override;

private:
  RoboClaw &_roboclaw;
};

}  // namespace motors

#endif