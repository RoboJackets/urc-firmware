#include "main.hpp"

int main() {

  Context context;

  while (true) {
    updateNetwork(context);
  }

  return 0;
}

void updateNetwork(Context &context) {

  ethernet_driver::EthernetDriver ethernetDriver = context.getEthernetDriver();

  // check if ethernet hardware is OK

  // check for incoming messages; if there is one, read it

  // send outgoing messages based on TIMER_DURATION
  // if messages are ready, send them
  if (ethernet_driver::EthernetDriver::sendTimeHasElapsed()) {

    // // DEBUG
    // elapsedMicros driverTimer;

    // create Nanopb messages
    DriveEncodersMessage driveEncodersMessage = DriveEncodersMessage_init_zero;

    // read values from drive motor controllers
    driveEncodersMessage.frontLeftTicks = 12;
    driveEncodersMessage.frontRightTicks = 15;
    driveEncodersMessage.middleLeftTicks = 22;
    driveEncodersMessage.middleRightTicks = 25;
    driveEncodersMessage.backLeftTicks = 32;
    driveEncodersMessage.backRightTicks = 35;
    driveEncodersMessage.timestamp = context.getCurrentTime();

    ethernetDriver.sendEncoderMessages(driveEncodersMessage);

    // // DEBUG
    // unsigned long elapsedTime = driverTimer;
    // Serial.print("Exec time: ");
    // Serial.println(elapsedTime);

    ethernet_driver::EthernetDriver::resetSendTimer();
  }
}
