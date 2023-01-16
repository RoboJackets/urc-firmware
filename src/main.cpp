#include "main.hpp"
// #include "RoboClaw.h"
// #include "SoftwareSerial.h"

// SoftwareSerial serial(10, 11);
void turnTheMotor(Context &context);

int main() {

  // // DEBUG
  // // Wait until serial monitor is opened before proceeding
  // while (!Serial) {
  // }
  // Serial.println("Serial port connected to Teensy 4.1!");

  Context context;
  manager::MotorManager motorManager(context);
  manager::EthernetManager ethernetManager(context);

  Serial.println("starting");

  delay(10);
  // turnTheMotor(context);

  while (true) {
    ethernetManager.update(context);
    motorManager.update(context);
  }
  // Serial.println("here");
    checkSoloUnos(context);
    delay(500);

  return 0;
}

// int main() {
//   delay(10);

//   Serial.print("here");

//   

//   setupMotors(context);

  

//   while (true) {
//     // updateNetwork(context);
//     // checkRoboClaws(context);
//     Serial.print("here");
//   }

//   return 0;
// }

// void setupMotors(Context &context) {s
  // RoboClaw roboClaw1 = context.getRoboClawMotor1();
  // RoboClaw roboClaw2 = context.getRoboClawMotor2();
  // RoboClaw roboClaw3 = context.getRoboClawMotor3();
  
  // SOLOMotorControllersUart solo_driver4 = context.getSoloDriver4();
  // SOLOMotorControllersUart solo_driver5 = context.getSoloDriver5();
  // SOLOMotorControllersUart solo_driver6 = context.getSoloDriver6();

  // roboClaw1.begin(38400);
  // roboClaw2.begin(38400);
  // Serial.begin(9600);
// }


// TODO: REIMPLEMENT THIS STUFF ------

// void checkSoloUnos(Context &context) {
//   SOLOMotorControllersUart solo_driver4 = context.getSoloUNOController();
//   // SOLOMotorControllersUart solo_driver5 = context.getSoloDriver5();
//   // SOLOMotorControllersUart solo_driver6 = context.getSoloDriver6();

//   bool valid = solo_driver4.CommunicationIsWorking();
//   // uint32_t address = solo_driver4.GetDeviceAddress();

//   float temperature = solo_driver4.GetBoardTemperature();
//   Serial.printf("Temp = %f", temperature);

//   long type = solo_driver4.GetMotorType();
//   Serial.printf("Motor type: %ld\n", type);

//   Serial.println(valid ? "SoloUno Hotness: " : "yikes u ded");

//   Serial.printf("Encoder pos: %d\n", solo_driver4.GetPositionCountsFeedback());
  
//   Serial.printf("Velocity: %d\n", solo_driver4.GetSpeedFeedback());
//   // Serial.printf("", )

// }

// void turnTheMotor(Context &context) {
//   SOLOMotorControllersUart solo_driver4 = context.getSoloDriver4();
  
//   solo_driver4.SetFeedbackControlMode(SOLOMotorControllers::FeedbackControlMode::encoders);
//   solo_driver4.SetControlMode(SOLOMotorControllers::ControlMode::positionMode);
//   solo_driver4.SetPositionControllerKp(0.0195693);
//   solo_driver4.SetPositionControllerKi(0);
//   solo_driver4.ResetPositionToZero();
//   // solo_driver4.SetSpeedReference(900);
//   // solo_driver4.SetSpeedLimit(3000);
//   // solo_driver4.SetPositionReference(250000);
// }

// -------------------------

// void checkRoboClaws(Context &context) {
//   // RoboClaw roboClaw1 = context.getRoboClawMotor1();
//   // RoboClaw roboClaw2 = context.getRoboClawMotor2();
//   // RoboClaw roboClaw3 = context.getRoboClawMotor3();

//   Serial.print("Is available: " + roboClaw1.available());

//   bool valid = false;
//   int address = 0x80;
//   uint8_t status = 0;
//   uint32_t speed = roboClaw1.ReadSpeedM1(address, &status, &valid);

//   Serial.print(valid ? "RoboClaw1 Speed: " + speed : "Invalid read on RoboClaw1");
// }

// void updateNetwork(Context &context) {

//   ethernet_driver::EthernetDriver ethernetDriver = context.getEthernetDriver();

//   // TODO: check if ethernet hardware is OK

//   // TODO: check for incoming messages; if there is one, read it

//   // send outgoing messages based on TIMER_DURATION
//   // if messages are ready, send them
//   if (ethernetDriver.sendTimeHasElapsed()) {

//     // // DEBUG
//     //
//     // // create Nanopb message
//     // DriveEncodersMessage driveEncodersMessage = DriveEncodersMessage_init_zero;

//     // // read values from drive motor controllers
//     // driveEncodersMessage.frontLeftTicks = 12;
//     // driveEncodersMessage.frontRightTicks = 15;
//     // driveEncodersMessage.middleLeftTicks = 22;
//     // driveEncodersMessage.middleRightTicks = 25;
//     // driveEncodersMessage.backLeftTicks = 32;
//     // driveEncodersMessage.backRightTicks = 35;
//     // driveEncodersMessage.timestamp = context.getCurrentTime();

//     // ethernetDriver.sendEncoderMessages(driveEncodersMessage);

//     ethernetDriver.resetSendTimer();
//   }
// }
