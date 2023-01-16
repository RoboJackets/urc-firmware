#ifndef SOLOCONTROLLER_HPP
#define SOLOCONTROLLER_HPP

#include "MotorController.hpp"

namespace motors {

class SoloController : public MotorController {
public:
  SoloController(SOLOMotorControllersUart &solouno) : _solouno(solouno){};
  void begin(long speed);
  int32_t getSpeed(uint8_t address, uint8_t channel, bool &valid) override;
  void setSpeed(int32_t speed, uint8_t address, uint8_t channel, bool &valid) override;

  // void sendPacket(int command, int data);
  // void setUp();

private:
 SOLOMotorControllersUart &_solouno;
};

} 

#endif