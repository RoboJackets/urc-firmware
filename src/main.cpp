#include "main.hpp"

elapsedMillis roboclawTimer;
elapsedMicros serialTimer;

int main() {

  uint32_t myTemp = -1;
  DriveEncodersMessage driveEncodersMessage = DriveEncodersMessage_init_zero;

  Context context;

  setupMotors(context);

  while (true) {
    updateRoboClaw(context, driveEncodersMessage);
    updateNetwork(context, driveEncodersMessage);
  }

  return 0;
}

void updateNetwork(Context &context, DriveEncodersMessage &driveEncodersMessage) {

  ethernet_driver::EthernetDriver ethernetDriver = context.getEthernetDriver();

  // TODO: check if ethernet hardware is OK

  // TODO: check for incoming messages; if there is one, read it

  // send outgoing messages based on TIMER_DURATION
  // if messages are ready, send them
  if (ethernetDriver.sendTimeHasElapsed()) {

    // DEBUG
    
    // create Nanopb message
  
    // read values from drive motor controllers
    // driveEncodersMessage.frontLeftTicks = 12;
    // driveEncodersMessage.frontRightTicks = myTemp;
    driveEncodersMessage.middleLeftTicks = 22;
    driveEncodersMessage.middleRightTicks = 25;
    driveEncodersMessage.backLeftTicks = 32;
    driveEncodersMessage.backRightTicks = 35;
    driveEncodersMessage.timestamp = context.getCurrentTime();

    ethernetDriver.sendEncoderMessages(driveEncodersMessage);

    Serial.print("Temp: ");
    Serial.println(driveEncodersMessage.frontRightTicks);

    ethernetDriver.resetSendTimer();
  }
}

void setupMotors(Context &context) {
  RoboClaw roboclaw = context.getRoboClaw();
  roboclaw.begin(38400);
}

void updateRoboClaw(Context &context, DriveEncodersMessage &driveEncodersMessage) {
  RoboClaw roboclaw = context.getRoboClaw();
  
  if (roboclawTimer >= 100) {

    uint8_t address = 0x80;
    uint16_t temp;
    bool valid;
    
    serialTimer = 0;
    valid = (bool)roboclaw.ReadTemp(address, temp);
    driveEncodersMessage.frontLeftTicks = (uint32_t)serialTimer;

    if (valid)
      driveEncodersMessage.frontRightTicks = (uint32_t)temp;
    else
      driveEncodersMessage.frontRightTicks = -1;

    roboclawTimer -= 10;
  }

}
