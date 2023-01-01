#include "main.hpp"

elapsedMillis roboclawTimer;
elapsedMicros serialTimer;

int main() {

  Context context;
  
  setupNetwork(context);
  setupRoboClaw(context);

  while (true) {
    updateNetwork(context);
    updateRoboClaw(context);
  }

  return 0;
}

void setupRoboClaw(Context &context) {
  RoboClaw roboclaw = context.getRoboClaw();
  roboclaw.begin(38400);
}

void setupNetwork(Context &context) {
  ethernet::EthernetDriver &ethernetDriver = context.getEthernetDriver();
  Ethernet.begin(ethernet::MAC, 
                  IPAddress(ethernet::LOCAL_IP),
                  IPAddress(ethernet::DNS),
                  IPAddress(ethernet::GATEWAY),
                  IPAddress(ethernet::SUBNET));
  ethernetDriver.begin(ethernet::PORT);
}


void updateNetwork(Context &context) {

  ethernet::EthernetDriver &ethernetDriver = context.getEthernetDriver();
  DriveEncodersMessage &driveEncodersMessage = context.getDriveEncodersMessage();
  RequestMessage &requestMessage = context.getRequestMessage();

  if (ethernetDriver.available()) {
    uint8_t requestBuffer[256];
    size_t requestLength = ethernetDriver.parsePacket();
    ethernetDriver.read(requestBuffer, requestLength);
  }

  if (ethernetDriver.sendTimeHasElapsed()) {
    
  }

  // if (ethernetDriver.requestReady()) {
  //   ethernetDriver.receiveRequest(requestMessage);
  // }

  // if (ethernetDriver.sendTimeHasElapsed()) {
  //   driveEncodersMessage.timestamp = context.getCurrentTime();
  //   ethernetDriver.sendEncoderMessages(driveEncodersMessage);

  //   ethernetDriver.resetSendTimer();
  // }

  
}

void updateRoboClaw(Context &context) {
  RoboClaw &roboclaw = context.getRoboClaw();
  DriveEncodersMessage &driveEncodersMessage = context.getDriveEncodersMessage();
  RequestMessage &requestMessage = context.getRequestMessage();

  // get encoder ticks from RoboClaws
  const int NUM_ROBOCLAWS = 2;
  uint8_t roboclawAddresses[NUM_ROBOCLAWS] = {0x80, 0x81};

  // // if a request comes in, service it
  // if (requestMessage.requestSpeed) {
  //   roboclaw.SpeedM1(roboclawAddresses[0], requestMessage.leftSpeed);
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
