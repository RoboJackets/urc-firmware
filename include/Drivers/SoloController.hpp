#ifndef SOLOCONTROLLER_HPP
#define SOLOCONTROLLER_HPP

#include "MotorController.hpp"

namespace motors {

class SoloController : public MotorController {
public:
  SoloController(SOLOMotorControllersUart &solouno) : _solouno(solouno){};
  void begin(long baudRate);
  void begin(SOLOMotorControllers::ControlMode controlMode, long baudRate);
  int32_t getSpeed(uint8_t address, uint8_t channel, bool &valid) override;
  void setSpeed(int32_t speed, uint8_t address, uint8_t channel, bool &valid) override;
  int32_t getRawPosition(uint8_t address, uint8_t channel, bool &valid) override;
  void setRawPosition(int32_t position, uint8_t address, uint8_t channel, bool &valid) override;

  // void sendPacket(int command, int data);
  // void setUp();

private:
 SOLOMotorControllersUart &_solouno;
};

} 

#endif