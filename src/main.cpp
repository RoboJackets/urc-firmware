#include "main.hpp"

elapsedMillis roboclawTimer;
elapsedMicros serialTimer;

int main() {

  Context context;

  setupRoboClaw(context);

  while (true) {
    updateNetwork(context);
    updateRoboClaw(context);
  }

  return 0;
}

void updateNetwork(Context &context) {

  ethernet_driver::EthernetDriver ethernetDriver = context.getEthernetDriver();
  DriveEncodersMessage driveEncodersMessage = context.getDriveEncodersMessage();
  RequestMessage requestMessage = context.getRequestMessage();

  if (ethernetDriver.requestReady()) {
    ethernetDriver.receiveRequest(requestMessage);
  }

  driveEncodersMessage.timestamp = context.getCurrentTime();
  ethernetDriver.sendEncoderMessages(driveEncodersMessage);
}

void setupRoboClaw(Context &context) {
  RoboClaw roboclaw = context.getRoboClaw();
  roboclaw.begin(38400);
}

void updateRoboClaw(Context &context) {
  RoboClaw roboclaw = context.getRoboClaw();
  DriveEncodersMessage driveEncodersMessage = context.getDriveEncodersMessage();
  RequestMessage requestMessage = context.getRequestMessage();

  // get encoder ticks from RoboClaws
  const int NUM_ROBOCLAWS = 2;
  uint8_t roboclawAddresses[NUM_ROBOCLAWS] = {0x80, 0x81};

  // if a request comes in, service it
  if (requestMessage.requestSpeed) {
    roboclaw.SpeedM1(roboclawAddresses[0], requestMessage.leftSpeed);
    roboclaw.SpeedM1(roboclawAddresses[1], requestMessage.rightSpeed);
  }
  
  RoboClawData buffer[NUM_ROBOCLAWS];
  
  for (int i = 0; i < NUM_ROBOCLAWS; i++)
    roboclaw.ReadSpeedM1(roboclawAddresses[i], &buffer[i].wheelSpeed, &buffer[i].valid);

  driveEncodersMessage.has_leftSpeed = buffer[0].valid;
  driveEncodersMessage.leftSpeed = buffer[0].wheelSpeed;
  driveEncodersMessage.has_rightSpeed = buffer[1].valid;
  driveEncodersMessage.rightSpeed = buffer[1].wheelSpeed;
}
