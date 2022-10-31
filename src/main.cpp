#include "main.hpp"

Context context;

int main() {

  auto ethernetDriver = context.getEthernetDriver();

  //DEBUG
  writeToEthernet();

  while (true) {
    ethernetDriver.execute();
  }

  return 0;
}

// DEBUG
void writeToEthernet() {
  
  auto ethernetDriver = context.getEthernetDriver();

  uint8_t responsebuffer[256];
  size_t response_length;
  
  // // Test 1: Send "hello"
  responsebuffer[0] = 'h';
  responsebuffer[1] = 'e';
  responsebuffer[2] = 'l';
  responsebuffer[3] = 'l';
  responsebuffer[4] = 'o';
  responsebuffer[5] = '\0';

  response_length = 5;

  // // Test 2: Send dummy encoder values
  // DriveEncodersMessage driveEncodersMessage = DriveEncodersMessage_init_zero;

  // driveEncodersMessage.frontLeftTicks = 12;
  // driveEncodersMessage.frontRightTicks = 15;
  // driveEncodersMessage.middleLeftTicks = 22;
  // driveEncodersMessage.middleRightTicks = 25;
  // driveEncodersMessage.backLeftTicks = 32;
  // driveEncodersMessage.backRightTicks = 35;
  // driveEncodersMessage.timestamp = context.getCurrentTime();

  // pb_ostream_t ostream = pb_ostream_from_buffer(responsebuffer, sizeof(responsebuffer));
  // bool status = pb_encode(&ostream, DriveEncodersMessage_fields, &driveEncodersMessage);
  // response_length = ostream.bytes_written;

  ethernetDriver.write(responsebuffer, response_length);
}
