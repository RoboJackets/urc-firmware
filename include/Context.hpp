#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <Arduino.h>
#include "Drivers/EthernetDriver.hpp"
#include "Drivers/RoboClawController.hpp"
#include "RoboClaw.h"
#include "Messages.hpp"

/**
 * @brief Singleton class containing all instances of peripheral drivers.
 * Initalize all drivers and shared data structures in here.
 *
 */
class Context {
public:
  Context(){};
  ~Context(){};
  RoboClaw &getRoboClaw() { return roboclaw; }
  DriveEncodersMessage &getDriveEncodersMessage() { return driveEncodersMessage; }
  RequestMessage &getRequestMessage() { return requestMessage; }
  ethernet::EthernetDriver &getEthernetDriver() { return ethernetDriver; }
  elapsedMillis &getCurrentTime() { return currentTime; }
  motors::RoboClawController *getRoboClawController() { return &roboClawController; }

private:
  RequestMessage requestMessage = RequestMessage_init_zero;
  DriveEncodersMessage driveEncodersMessage = DriveEncodersMessage_init_zero;
  ethernet::EthernetDriver ethernetDriver;
  elapsedMillis currentTime;
  RoboClaw roboclaw = RoboClaw(&Serial2, 10000);
  motors::RoboClawController roboClawController = motors::RoboClawController(roboclaw);
};

#endif