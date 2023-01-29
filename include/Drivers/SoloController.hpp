#ifndef SOLOCONTROLLER_HPP
#define SOLOCONTROLLER_HPP

#include "MotorController.hpp"

namespace motors {

class SoloController : public MotorController {
public:
  SoloController(SOLOMotorControllersUart &solouno1, SOLOMotorControllersUart &solouno2) : _solouno1(solouno1), _solouno2(solouno2){};
  void begin(long speed);
  void begin(SOLOMotorControllers::ControlMode controlMode, long speed);
  int32_t getSpeed(uint8_t address, uint8_t channel, bool &valid) override;
  void setSpeed(int32_t speed, uint8_t address, uint8_t channel, bool &valid) override;
  int32_t getRawPosition(uint8_t address, uint8_t channel, bool &valid) override;
  void setRawPosition(int32_t position, uint8_t address, uint8_t channel, bool &valid) override;

  // void sendPacket(int command, int data);
  // void setUp();

private:
 SOLOMotorControllersUart &_solouno1;
 SOLOMotorControllersUart &_solouno2;
};

} 

#endif