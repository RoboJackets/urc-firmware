#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <Arduino.h>
#include "Drivers/EthernetDriver.hpp"
#include "Drivers/RoboClawController.hpp"
#include "Drivers/SoloController.hpp"
#include "Messages.hpp"

// #include "SOLOMotorControllersUart.h" 
#include "RoboClaw.h"

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
  motors::SoloController *getSoloController() { return &soloController; }
  SOLOMotorControllersUart solouno1 = SOLOMotorControllersUart((unsigned char)0, Serial3, SOLOMotorControllers::UartBaudrate::rate937500, 200L, 5);
  SOLOMotorControllersUart solouno2 = SOLOMotorControllersUart((unsigned char)0, Serial4, SOLOMotorControllers::UartBaudrate::rate937500, 200L, 5);
  RoboClaw roboclaw = RoboClaw(&Serial2, 10000);
  
private:
  RequestMessage requestMessage = RequestMessage_init_zero;
  DriveEncodersMessage driveEncodersMessage = DriveEncodersMessage_init_zero;
  ethernet::EthernetDriver ethernetDriver;
  elapsedMillis currentTime;
  motors::RoboClawController roboClawController = motors::RoboClawController(roboclaw);
  motors::SoloController soloController = motors::SoloController(solouno1, solouno2);
};

#endif