#include "main.hpp"
// #include "RoboClaw.h"
// #include "SoftwareSerial.h"

// SoftwareSerial serial(10, 11);

Context context;
manager::MotorManager motorManager(context);
// manager::EthernetManager ethernetManager(context);

int main() {

  // // DEBUG
  // // Wait until serial monitor is opened before proceeding
  // while (!Serial) {
  // }
  // Serial.println("Serial port connected to Teensy 4.1!");

  delay(1000);

  Serial.println("starting");

  delay(1000);
  // turnTheMotor(context);

  while (true) {
    testUpdate();
    // ethernetManager.update(context);
    // motorManager.update(context);
  }
  // Serial.println("here");
  // checkSoloUnos(context);
  delay(500);

  return 0;
}

void testUpdate() {
  RequestMessage message = {1,0,0,1000,0,1000,0,0,0};
  motorManager.update(context, message);
  
  Serial.printf("Speed: %d\n", context.solouno1.GetSpeedFeedback());
}