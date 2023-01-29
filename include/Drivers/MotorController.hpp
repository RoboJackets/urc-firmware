#ifndef MOTORCONTROLLER_HPP
#define MOTORCONTROLLER_HPP

#include <Arduino.h>
#include "RoboClaw.h"
#include "SOLOMotorControllersUart.h" 

namespace motors {

class MotorController {
public:
  int32_t zeroPosition = 0;

  MotorController(){};

  virtual void begin(long speed);
  // TODO: virtual void setMode(uint8_t channel, MotorMode mode) = 0;
  virtual int32_t getSpeed(uint8_t address, uint8_t channel, bool &valid) = 0;
  virtual void setSpeed(int32_t speed, uint8_t address, uint8_t channel, bool &valid) = 0;

  virtual int32_t getRawPosition(uint8_t address, uint8_t channel, bool &valid) = 0;
  virtual void setRawPosition(int32_t position, uint8_t address, uint8_t channel, bool &valid) = 0;

  virtual int32_t getPosition(uint8_t address, uint8_t channel, bool &valid) {
    return getRawPosition(address, channel, valid) - zeroPosition;
  }

  virtual void setPosition(int32_t position, uint8_t address, uint8_t channel, bool &valid) {
    setRawPosition(position + zeroPosition, address, channel, valid);
  }

  virtual void zero(uint8_t address, uint8_t channel, bool &valid){
    zeroPosition = getRawPosition(address, channel, valid);
  }

};

}  // namespace motors

#endif