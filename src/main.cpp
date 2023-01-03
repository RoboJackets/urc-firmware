#include "main.hpp"

#define DEBUG 0

elapsedMillis roboclawTimer;
elapsedMicros serialTimer;

int main() {

#if DEBUG

  // Wait until serial monitor is opened before proceeding
  while (!Serial) {
  }
  Serial.println("Serial port connected to Teensy 4.1!");

#endif

  Context context;
  manager::MotorManager motorManager(context);

  setupNetwork(context);
  // setupRoboClaw(context);

  while (true) {
    updateNetwork(context);
    motorManager.update(context);

    // updateRoboClaw(context);
  }

  return 0;
}

void setupRoboClaw(Context &context) {
  RoboClaw roboclaw = context.getRoboClaw();
  roboclaw.begin(38400);
}

void setupNetwork(Context &context) {
  ethernet::EthernetDriver &ethernetDriver = context.getEthernetDriver();
  Ethernet.begin(ethernet::MAC, IPAddress(ethernet::LOCAL_IP), IPAddress(ethernet::DNS), IPAddress(ethernet::GATEWAY),
                 IPAddress(ethernet::SUBNET));
  ethernetDriver.begin(ethernet::PORT);

#if DEBUG

  Serial.println("Ethernet set up.");
  Serial.print("\t MAC address: ");

  uint8_t buffer[6];
  Ethernet.MACAddress(buffer);

  Serial.println(String((char *)buffer));

  Serial.print("\t Server IP (Teensy 4.1): ");
  Serial.println(Ethernet.localIP());
  Serial.print("\t Client IP (computer): ");
  Serial.println(IPAddress(ethernet::CLIENT_IP));
  Serial.print("\t DNS: ");
  Serial.println(Ethernet.dnsServerIP());
  Serial.print("\t Gateway: ");
  Serial.println(Ethernet.gatewayIP());
  Serial.println("");

#endif
}

// void setupMotors(Context &context) {
//   motors::RoboClawController &roboClawController = context.getRoboClawController();

//   motors::Motor motors[2];
//   motors[0].init("Left Motor", roboClawController, 0);
//   motors[1].init("Right Motor", roboClawController, 1);

//   for (motors::Motor &motor : motors) {
//     motor.getSpeed();
//   }
// }

void updateNetwork(Context &context) {

  ethernet::EthernetDriver &ethernetDriver = context.getEthernetDriver();
  DriveEncodersMessage &driveEncodersMessage = context.getDriveEncodersMessage();
  RequestMessage &requestMessage = context.getRequestMessage();

  // If request from client is ready, read it into requestMessage
  if (ethernetDriver.available()) {
    uint8_t requestBuffer[256];
    size_t requestLength = ethernetDriver.parsePacket();

    ethernetDriver.read(requestBuffer, requestLength);
    protobuf::Messages::decodeRequest(requestBuffer, requestLength, requestMessage);

#if DEBUG

    Serial.print(context.getCurrentTime());
    Serial.print(": EthernetDriver received ");
    Serial.print(requestLength);
    Serial.println(" bytes");

#endif
  }

  // if TIMER_DURATION_MS has elapsed, send a response
  if (ethernet::EthernetDriver::sendTimeHasElapsed()) {
    uint8_t responseBuffer[256];
    driveEncodersMessage.timestamp = context.getCurrentTime();

    size_t bytesWritten =
        protobuf::Messages::encodeResponse(responseBuffer, sizeof(responseBuffer), driveEncodersMessage);

    ethernetDriver.beginPacket(ethernet::CLIENT_IP, ethernet::PORT);
    ethernetDriver.write(responseBuffer, bytesWritten);
    ethernetDriver.endPacket();

#if DEBUG

    Serial.print(driveEncodersMessage.timestamp);
    Serial.print(": EthernetDriver wrote ");
    Serial.print(bytesWritten);
    Serial.println(" bytes");

#endif

    ethernet::EthernetDriver::resetSendTimer();
  }
}

void updateRoboClaw(Context &context) {
  RoboClaw &roboclaw = context.getRoboClaw();
  DriveEncodersMessage &driveEncodersMessage = context.getDriveEncodersMessage();
  RequestMessage &requestMessage = context.getRequestMessage();

  // get encoder ticks from RoboClaws
  const int NUM_ROBOCLAWS = 2;
  uint8_t roboclawAddresses[NUM_ROBOCLAWS] = { 0x80, 0x81 };

  // // if a request comes in, service it
  // if (requestMessage.requestSpeed) {
    // roboclaw.SpeedM1(roboclawAddresses[0], requestMessage.leftSpeed);
  //   roboclaw.SpeedM1(roboclawAddresses[1], requestMessage.rightSpeed);
  // }

  uint16_t buffer;
  uint16_t &bufferRef = buffer;

  uint32_t enc1;
  uint32_t enc2;
  uint32_t &enc1Ref = enc1;
  uint32_t &enc2Ref = enc2;

  uint8_t status;
  bool valid;

  // reads the raw output of the encoder (waveform)
  // roboclaw.ReadEncM1(roboclawAddresses[0], &status, &valid);

  // reads the temperature
  // roboclaw.ReadTemp(roboclawAddresses[0], bufferRef);

  // reads encoder counts
  valid = roboclaw.ReadEncoders(roboclawAddresses[0], enc1Ref, enc2Ref);

  driveEncodersMessage.has_leftSpeed = valid;
  driveEncodersMessage.leftSpeed = enc1;

  // reads the raw output of the encoder (waveform)
  // roboclaw.ReadEncM1(roboclawAddresses[1], &status, &valid);

  // reads the temperature
  // roboclaw.ReadTemp(roboclawAddresses[1], bufferRef);

  valid = roboclaw.ReadEncoders(roboclawAddresses[1], enc1Ref, enc2Ref);

  driveEncodersMessage.has_rightSpeed = valid;
  driveEncodersMessage.rightSpeed = enc1;
}

void updateMotors(Context &context) {

}