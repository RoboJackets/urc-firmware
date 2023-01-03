#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <Arduino.h>
#include "EthernetDriver.hpp"
#include "RoboClaw.h"
#include "MotorController/MotorController.hpp"

#include "pb_encode.h"
#include "pb_decode.h"
#include "urc.pb.h"

/**
 * @brief Singleton class containing all instances of peripheral drivers
 *
 */
class Context {
public:
  Context(){};
  ~Context(){};
  RoboClaw &getRoboClaw() {
    return roboclaw;
  }
  DriveEncodersMessage &getDriveEncodersMessage() {
    return driveEncodersMessage;
  }
  RequestMessage &getRequestMessage() {
    return requestMessage;
  }
  ethernet::EthernetDriver &getEthernetDriver() {
    return ethernetDriver;
  }
  elapsedMillis &getCurrentTime() {
    return currentTime;
  }
  motors::RoboClawController *getRoboClawController() {
    return &roboClawController;
  }

private:
  RequestMessage requestMessage = RequestMessage_init_zero;
  DriveEncodersMessage driveEncodersMessage = DriveEncodersMessage_init_zero;
  ethernet::EthernetDriver ethernetDriver;
  elapsedMillis currentTime;
  RoboClaw roboclaw = RoboClaw(&Serial2, 10000);
  motors::RoboClawController roboClawController = motors::RoboClawController(roboclaw);
};

#endif