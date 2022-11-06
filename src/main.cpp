#include "main.hpp"
#include "RoboClaw.h"

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

  roboClaw1.begin(38400);
  roboClaw2.begin(38400);
}

void checkRoboClaws(Context &context) {
  RoboClaw roboClaw1 = context.getRoboClawMotor1();
  RoboClaw roboClaw2 = context.getRoboClawMotor2();
  RoboClaw roboClaw3 = context.getRoboClawMotor3();

  roboClaw1.available();
  bool valid = false;
  int address = 0x80;
  uint8_t status = 0;
  roboClaw1.ReadSpeedM1(address, &status, &valid);
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
