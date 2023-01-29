#ifndef ROBOCLAWCONTROLLER_HPP
#define ROBOCLAWCONTROLLER_HPP

#include "MotorController.hpp"

namespace motors {

class RoboClawController : public MotorController {
public:
  RoboClawController(RoboClaw &roboclaw) : _roboclaw(roboclaw){};
  void begin(long baudRate);
  int32_t getSpeed(uint8_t address, uint8_t channel, bool &valid) override;
  void setSpeed(int32_t speed, uint8_t address, uint8_t channel, bool &valid) override;
  int32_t getRawPosition(uint8_t address, uint8_t channel, bool &valid) override;
  void setRawPosition(int32_t position, uint8_t address, uint8_t channel, bool &valid) override;

private:
  RoboClaw &_roboclaw;
};
}  // namespace motors

#endif