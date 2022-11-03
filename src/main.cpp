#include "main.hpp"
#include "RoboClaw.h"

int main() {

  Context context;

  setupRoboClaw(context);

  while (true) {
    updateNetwork(context);
  }

  return 0;
}

void setupRoboClaw(Context &context) {
  // HardwareSerial roboclaw_serial =  // = context.getSerial();
  RoboClaw roboclaw_motor = context.getRoboClawMotor1();
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
