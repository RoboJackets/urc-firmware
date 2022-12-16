#include "main.hpp"

elapsedMillis roboclawTimer;

int main() {

  uint32_t myTemp = -1;

  Context context;

  setupMotors(context);

  while (true) {
    updateRoboClaw(context, myTemp);
    updateNetwork(context, myTemp);
  }

  return 0;
}

void updateNetwork(Context &context, uint32_t &myTemp) {

  ethernet_driver::EthernetDriver ethernetDriver = context.getEthernetDriver();

  // TODO: check if ethernet hardware is OK

  // TODO: check for incoming messages; if there is one, read it

  // send outgoing messages based on TIMER_DURATION
  // if messages are ready, send them
  if (ethernetDriver.sendTimeHasElapsed()) {

    // DEBUG
    
    // create Nanopb message
    DriveEncodersMessage driveEncodersMessage = DriveEncodersMessage_init_zero;

    // read values from drive motor controllers
    driveEncodersMessage.frontLeftTicks = 12;
    driveEncodersMessage.frontRightTicks = myTemp;
    driveEncodersMessage.middleLeftTicks = 22;
    driveEncodersMessage.middleRightTicks = 25;
    driveEncodersMessage.backLeftTicks = 32;
    driveEncodersMessage.backRightTicks = 35;
    driveEncodersMessage.timestamp = context.getCurrentTime();

    ethernetDriver.sendEncoderMessages(driveEncodersMessage);

    Serial.print("Temp: ");
    Serial.println(myTemp);

    ethernetDriver.resetSendTimer();
  }
}

void setupMotors(Context &context) {
  RoboClaw roboclaw = context.getRoboClaw();
  roboclaw.begin(38400);
}

void updateRoboClaw(Context &context, uint32_t &myTemp) {
  RoboClaw roboclaw = context.getRoboClaw();
  
  if (roboclawTimer >= 10) {
    
    uint8_t address = 0x80;
    uint16_t temp;
    bool valid;
    
    roboclaw.ReadTemp(address, temp);
  
    myTemp = (uint32_t)temp;

    roboclawTimer -= 10;
  }

}
