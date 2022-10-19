#include "EthernetDriver.hpp"

namespace ethernet_driver {

elapsedMillis EthernetDriver::sendTimer;

EthernetDriver::EthernetDriver() : clientIP(CLIENT_IP_ADDR), serverIP(SERVER_IP_ADDR), udp() {
  Ethernet.begin(SERVER_MAC_ADDR, serverIP);
  udp.begin(UDP_PORT);
}

unsigned long EthernetDriver::getSendTimer() {
  return sendTimer;
}

void EthernetDriver::setSendTimer(unsigned long time) {
  sendTimer = time;
}

bool EthernetDriver::sendTimeHasElapsed() {
  return sendTimer >= TIMER_DURATION_MS;
}

void EthernetDriver::resetSendTimer() {
  sendTimer -= TIMER_DURATION_MS;
}

void EthernetDriver::sendTestMessage() {
  udp.beginPacket(clientIP, UDP_PORT);
  udp.write("hello");
  udp.endPacket();
}

bool EthernetDriver::sendEncoderMessages(DriveEncodersMessage driveEncodersMessage) {
  uint8_t responsebuffer[256];
  size_t response_length;

  pb_ostream_t ostream = pb_ostream_from_buffer(responsebuffer, sizeof(responsebuffer));
  bool status = pb_encode(&ostream, DriveEncodersMessage_fields, &driveEncodersMessage);
  response_length = ostream.bytes_written;

  udp.beginPacket(clientIP, UDP_PORT);
  udp.write(responsebuffer, response_length);
  udp.endPacket();

  return status;
}

}  // namespace ethernet_driver