#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <Arduino.h>
#include "EthernetDriver.hpp"
#include "RoboClaw.h"

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
  ethernet_driver::EthernetDriver &getEthernetDriver() { return ethernet_driver; }
  elapsedMillis &getCurrentTime() { return currentTime; }
  RoboClaw &getRoboClaw() { return roboclaw; }
  DriveEncodersMessage &getDriveEncodersMessage() { return driveEncodersMessage; }
  RequestMessage &getRequestMessage() { return requestMessage; }

private:
  ethernet_driver::EthernetDriver ethernet_driver;
  elapsedMillis currentTime;
  RoboClaw roboclaw = RoboClaw(&Serial2, 10000);
  RequestMessage requestMessage = RequestMessage_init_zero;
  DriveEncodersMessage driveEncodersMessage = DriveEncodersMessage_init_zero;
};

#endif