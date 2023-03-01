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
  RequestMessage message;
  message.requestSpeed = 1;
  message.requestPosition = 0;
  message.requestDiagnostics = 0;
  message.leftSpeed = 50000;
  message.rightSpeed = 50000;
  message.timestamp = 0;
  
  motorManager.update(context, message);

  uint8_t rc_status = 0;
  bool rc_valid = false;
  bool &rc_valid_addr = rc_valid;

  // Serial.printf("SoloUNO 1 V: %f\n", context.solouno1.GetDcMotorVoltageVm());
  // Serial.printf("SoloUNO 1 Speed: %d\n", context.solouno1.GetSpeedFeedback());
  Serial.printf("RoboClaw 1 Speed: %d\n", context.getRoboClawController()->getSpeed(0x081, 0, rc_valid_addr));

  bool idk_valid = false;
  bool &idk_valid_addr = idk_valid;
  Serial.printf("Roboclaw 1 raw pos: %d\n", context.getRoboClawController()->getRawPosition(0x081, 0, idk_valid_addr));

  // Serial.printf("Roboclaw 1 corrected pos: %d\n", context.getRoboClawController()->getPosition(0x081, 0, idk_valid_addr));
  // context.roboclaw.SpeedAccelDeccelPositionM1(0x081, 0, 10000, 0, 80000, 1);
  delay(500);
  // context.roboclaw.SpeedAccelDeccelPositionM1(0x081, 0, 10000, 0, 80000, 0);

  context.roboclaw.SpeedAccelDeccelPositionM1(0x081,110000,12000,110000,-110000,1);
  // context.roboclaw.SpeedM1(0x081, 0);
  if (rc_valid) {
    Serial.printf("RoboClaw read valid, status = %d\n\n", rc_status);

  } else {
    Serial.printf("Roboclaw read INVALID\n");
  }
}