#include "EthernetDriver.hpp"

namespace ethernet_driver {

elapsedMillis EthernetDriver::sendTimer;

EthernetDriver::EthernetDriver() : clientIP(CLIENT_IP_ADDR), serverIP(SERVER_IP_ADDR), udp() {
  Ethernet.begin(SERVER_MAC_ADDR, serverIP);
  udp.begin(UDP_PORT);
}

void EthernetDriver::sendTestMessage() {
  udp.beginPacket(clientIP, UDP_PORT);
  udp.write("hello");
  udp.endPacket();
}

bool EthernetDriver::requestReady() {
  udp.parsePacket();
  return udp.available() != 0;
}

bool EthernetDriver::receiveRequest(RequestMessage requestMessage) {
  uint8_t requestBuffer[256];
  size_t requestLength = udp.parsePacket();

  udp.read(requestBuffer, requestLength);

  pb_istream_t istream = pb_istream_from_buffer(requestBuffer, sizeof(requestBuffer));
  return pb_decode(&istream, RequestMessage_fields, &requestMessage);
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