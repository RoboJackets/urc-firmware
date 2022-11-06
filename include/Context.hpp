#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <Arduino.h>
#include <RoboClaw.h>
#include "EthernetDriver.hpp"
#include "SoloDriver.hpp"
#include "SOLOMotorControllersUart.h" 
#include "SOLOMotorControllers.h" 


/**
 * @brief Singleton class containing all instances of peripheral drivers
 *
 */
class Context {
public:
  Context(){};
  ~Context(){};
  ethernet_driver::EthernetDriver &getEthernetDriver() {
    return ethernet_driver;
  }

  
  RoboClaw &getRoboClawMotor1() {
    return roboclaw_motor1;
  }
  RoboClaw &getRoboClawMotor2() {
    return roboclaw_motor2;
  }
  RoboClaw &getRoboClawMotor3() {
    return roboclaw_motor3;
  }
  SOLOMotorControllersUart &getSoloDriver4() {
    return solo_driver4;
  }
  SOLOMotorControllersUart &getSoloDriver5() {
    return solo_driver5;
  }
  SOLOMotorControllersUart &getSoloDriver6() {
    return solo_driver6;
  }


  elapsedMillis &getCurrentTime() {
    return currentTime;
  }

private:
  ethernet_driver::EthernetDriver ethernet_driver;

  RoboClaw roboclaw_motor1 = RoboClaw(&Serial1, 0);
  RoboClaw roboclaw_motor2 = RoboClaw(&Serial2, 0);
  RoboClaw roboclaw_motor3 = RoboClaw(&Serial3, 0);
  // solo_driver::SoloDriver solo_driver4 = solo_driver::SoloDriver(&Serial4);
  // solo_driver::SoloDriver solo_driver5 = solo_driver::SoloDriver(&Serial5);
  // solo_driver::SoloDriver solo_driver6 = solo_driver::SoloDriver(&Serial6);


  SOLOMotorControllersUart solo_driver4 = SOLOMotorControllersUart((unsigned char)'n',Serial4, SOLOMotorControllers::UartBaudrate(), 100000L, 10000);
  SOLOMotorControllersUart solo_driver5 = SOLOMotorControllersUart((unsigned char)'a',Serial5, SOLOMotorControllers::UartBaudrate(), 100000L, 10000);
  SOLOMotorControllersUart solo_driver6 = SOLOMotorControllersUart((unsigned char)'t',Serial6, SOLOMotorControllers::UartBaudrate(), 100000L, 10000);

  
  elapsedMillis currentTime;
};

#endif