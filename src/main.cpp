#include "EthernetDriver.h"
#include "main.h"

int main() {

  elapsedMillis elapsedTime;

  Serial.begin(9600);
  Serial.println("Hello World!");

  // Construct UDP instance
  ethernet_driver::EthernetDriver ethernet_driver;
  ethernet_driver.initEthernetHardware();

  printFlag = false;
  timer.begin(setFlag, 100000);

  while (true) {

    // create Nanopb messages
    DriveEncodersMessage driveEncodersMessage = DriveEncodersMessage_init_zero;

    // read values from drive motor controllers
    driveEncodersMessage.frontLeftTicks = 12;
    driveEncodersMessage.frontRightTicks = 15;
    driveEncodersMessage.middleLeftTicks = 22;
    driveEncodersMessage.middleRightTicks = 25;
    driveEncodersMessage.backLeftTicks = 32;
    driveEncodersMessage.backRightTicks = 35;
    driveEncodersMessage.timestamp = elapsedTime;

    noInterrupts();
    if (printFlag) {
      Serial.println(elapsedTime);
      elapsedTime = 0;

      ethernet_driver.sendTestMessage();

      printFlag = false;
    }
    interrupts();

    delay(5);
  }

  return 0;
}

void setFlag() {
  printFlag = true;
}
