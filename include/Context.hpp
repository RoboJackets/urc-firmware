#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <Arduino.h>
#include <RoboClaw.h>
#include "EthernetDriver.hpp"

#define HWSERIAL Serial1
#define HWSERIAL Serial2
#define HWSERIAL Serial3

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
  RoboClaw roboclaw_motor = RoboClaw(&Serial1, 0);
  RoboClaw roboclaw_motor = RoboClaw(&Serial2, 0);
  RoboClaw roboclaw_motor = RoboClaw(&Serial3, 0);
  elapsedMillis currentTime;
};

#endif