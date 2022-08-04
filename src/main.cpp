#include <Arduino.h>
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>

#include "pb_encode.h"
#include "pb_decode.h"
#include "urc.pb.h"

IntervalTimer encoderMessageTimer;
elapsedMillis elapsedTime;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 8, 255);
constexpr uint16_t localPort = 8888;
EthernetUDP Udp;

volatile bool printFlag = false;

void sendEncoderEstimates(DriveEncodersMessage driveEncodersMessage);
void updateFlag();
void connectUdp();

void setup()
{
  Serial.begin(9600);
  Serial.println("Hello World!");

  connectUdp();

  encoderMessageTimer.begin([]() { printFlag = true; }, 100000);
}

void loop()
{
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
  if (printFlag)
  {
    Serial.println(elapsedTime);
    elapsedTime = 0;

    Udp.beginPacket(ip, localPort);
    Udp.write("hello");
    Udp.endPacket();

    printFlag = false;
  }
  interrupts();

  delay(5);
}

void connectUdp()
{
  // start the Ethernet
  Ethernet.begin(mac, ip);

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial)
  {
    delay(10);  // wait for serial port to connect. Needed for native USB port only
  }

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware)
  {
    Serial.println("Ethernet shield was not found. Sorry, can't run without hardware. :(");
    while (true)
    {
      delay(10);  // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF)
  {
    Serial.println("Ethernet cable is not connected.");
  }

  // start UDP
  Udp.begin(localPort);
}

void sendEncoderEstimates(DriveEncodersMessage driveEncodersMessage)
{
  uint8_t responsebuffer[256];
  size_t response_length;
  bool ostatus;

  pb_ostream_t ostream = pb_ostream_from_buffer(responsebuffer, sizeof(responsebuffer));
  pb_encode(&ostream, DriveEncodersMessage_fields, &driveEncodersMessage);
  response_length = ostream.bytes_written;
}