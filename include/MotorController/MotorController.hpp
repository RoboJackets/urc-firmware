#ifndef MOTORCONTROLLER_HPP
#define MOTORCONTROLLER_HPP

#include <Arduino.h>
#include "RoboClaw.h"

namespace motors {

enum MotorMode { TORQUE, SPEED, VELOCITY };

class MotorController {
public:
    MotorController() {};
  void begin(uint8_t numChannels, HardwareSerial *serial, long speed) {
    _numChannels = numChannels;
    _serial = serial;
  }
//   virtual void setMode(uint8_t channel, MotorMode mode) = 0;
  virtual int32_t getSpeed(uint8_t channel, bool &valid) = 0;
  virtual void setSpeed(int32_t speed, uint8_t channel, bool &valid) = 0;
//   virtual int32_t getPosition(uint8_t channel) = 0;
//   virtual void setPosition(uint8_t channel) = 0;

protected:
  uint8_t _address;
  uint8_t _numChannels;
  HardwareSerial *_serial;
};

class RoboClawController : public MotorController {
public:
    RoboClawController(RoboClaw &roboclaw) : _roboclaw(roboclaw) {};
    int32_t getSpeed(uint8_t channel, bool &valid) override;
    void setSpeed(int32_t speed, uint8_t channel, bool &valid) override;
private:
    RoboClaw &_roboclaw;
};

}  // namespace motors

#endif