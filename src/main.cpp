#include <Arduino.h>
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>

#include "pb_encode.h"
#include "pb_decode.h"
#include "urc.pb.h"

IntervalTimer encoderMessageTimer;
elapsedMillis elapsedTime;

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

unsigned int localPort = 8888;
EthernetUDP Udp;

volatile bool printFlag = false;

void sendEncoderEstimates(DriveEncodersMessage driveEncodersMessage);
void updateFlag();

void setup() {
  Serial.begin(9600);
  Serial.println("Hello World!");
  encoderMessageTimer.begin(updateFlag, 100000);
}

void loop() {
  // create Nanopb messages
  DriveEncodersMessage driveEncodersMessage = DriveEncodersMessage_init_zero;

  // read values from drive motor controllers
  driveEncodersMessage.frontLeftTicks = 12;
  driveEncodersMessage.frontRightTicks = 15;
  driveEncodersMessage.middleLeftTicks = 22;
  driveEncodersMessage.middleRightTicks = 25;
  driveEncodersMessage.backLeftTicks = 32;
  driveEncodersMessage.backRightTicks = 35;

  // sendEncoderEstimates(driveEncodersMessage);

  // send out encoder message
  noInterrupts();
  if (printFlag) {
    Serial.println(elapsedTime);
    elapsedTime = 0;
    printFlag = false;
  }
  interrupts();

  delay(5);
}

void sendEncoderEstimates(DriveEncodersMessage driveEncodersMessage) {
  uint8_t responsebuffer[256];
  size_t response_length;
  bool ostatus;

  pb_ostream_t ostream = pb_ostream_from_buffer(responsebuffer, sizeof(responsebuffer));
  pb_encode(&ostream, DriveEncodersMessage_fields, &driveEncodersMessage);
  response_length = ostream.bytes_written;
}

void updateFlag() {
  printFlag = true;
}