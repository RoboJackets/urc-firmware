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
  RoboClaw &getRoboClawMotor() {
    return roboclaw_motor;
  }
  elapsedMillis &getCurrentTime() {
    return currentTime;
  }

private:
  ethernet_driver::EthernetDriver ethernet_driver;
  RoboClaw roboclaw_motor;
  elapsedMillis currentTime;
};

#endif