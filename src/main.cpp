#include "main.hpp"
#include "RoboClaw.h"
#include "SoftwareSerial.h"

SoftwareSerial serial(10, 11);


int main() {

  Context context;

  setupMotors(context);

  while (true) {
    updateNetwork(context);
    checkRoboClaws(context);
  }

  return 0;
}

void setupMotors(Context &context) {
  RoboClaw roboClaw1 = context.getRoboClawMotor1();
  RoboClaw roboClaw2 = context.getRoboClawMotor2();
  RoboClaw roboClaw3 = context.getRoboClawMotor3();
  
  solo_driver::SoloDriver solo_driver4 = context.getSoloDriver4();
  solo_driver::SoloDriver solo_driver5 = context.getSoloDriver5();
  solo_driver::SoloDriver solo_driver6 = context.getSoloDriver6();

  solo_driver4.begin();
  solo_driver5.begin();
  solo_driver6.begin();

  roboClaw1.begin(38400);
  roboClaw2.begin(38400);
  Serial.begin(57600);
}

void checkRoboClaws(Context &context) {
  RoboClaw roboClaw1 = context.getRoboClawMotor1();
  RoboClaw roboClaw2 = context.getRoboClawMotor2();
  RoboClaw roboClaw3 = context.getRoboClawMotor3();

  Serial.print("Is available: " + roboClaw1.available());

  bool valid = false;
  int address = 0x80;
  uint8_t status = 0;
  uint32_t speed = roboClaw1.ReadSpeedM1(address, &status, &valid);

  Serial.print(valid ? "RoboClaw1 Speed: " + speed : "Invalid read on RoboClaw1");
}

void updateNetwork(Context &context) {

  ethernet_driver::EthernetDriver ethernetDriver = context.getEthernetDriver();

  // TODO: check if ethernet hardware is OK

  // TODO: check for incoming messages; if there is one, read it

  // send outgoing messages based on TIMER_DURATION
  // if messages are ready, send them
  if (ethernetDriver.sendTimeHasElapsed()) {

    // // DEBUG
    //
    // // create Nanopb message
    // DriveEncodersMessage driveEncodersMessage = DriveEncodersMessage_init_zero;

    // // read values from drive motor controllers
    // driveEncodersMessage.frontLeftTicks = 12;
    // driveEncodersMessage.frontRightTicks = 15;
    // driveEncodersMessage.middleLeftTicks = 22;
    // driveEncodersMessage.middleRightTicks = 25;
    // driveEncodersMessage.backLeftTicks = 32;
    // driveEncodersMessage.backRightTicks = 35;
    // driveEncodersMessage.timestamp = context.getCurrentTime();

    // ethernetDriver.sendEncoderMessages(driveEncodersMessage);

    ethernetDriver.resetSendTimer();
  }
}
