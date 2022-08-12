#include "EthernetDriver.h"

namespace ethernet_driver {

EthernetDriver::EthernetDriver() : clientIP(CLIENT_IP_ADDR), udp() {
}

void EthernetDriver::initEthernetHardware() {
  Ethernet.begin(SERVER_MAC_ADDR, clientIP);

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found. Sorry, can't run without hardware. :(");
    while (true) {
      delay(10);  // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  // start UDP
  udp.begin(UDP_PORT);
}

void EthernetDriver::sendTestMessage() {
  udp.beginPacket(clientIP, UDP_PORT);
  udp.write("hello");
  udp.endPacket();
}

void EthernetDriver::sendEncoderMessages(DriveEncodersMessage driveEncodersMessage) {
  uint8_t responsebuffer[256];
  size_t response_length;

  pb_ostream_t ostream = pb_ostream_from_buffer(responsebuffer, sizeof(responsebuffer));
  pb_encode(&ostream, DriveEncodersMessage_fields, &driveEncodersMessage);
  response_length = ostream.bytes_written;

  udp.beginPacket(clientIP, UDP_PORT);
  udp.write(responsebuffer, response_length);
  udp.endPacket();
}

}  // namespace ethernet_driver