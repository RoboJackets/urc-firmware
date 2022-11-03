#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <Arduino.h>
#include <RoboClaw.h>
#include "EthernetDriver.hpp"

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
  elapsedMillis &getCurrentTime() {
    return currentTime;
  }

private:
  ethernet_driver::EthernetDriver ethernet_driver;
  RoboClaw roboclaw_motor1 = RoboClaw(&Serial1, 0);
  RoboClaw roboclaw_motor2 = RoboClaw(&Serial2, 0);
  RoboClaw roboclaw_motor3 = RoboClaw(&Serial3, 0);
  elapsedMillis currentTime;
};

#endif