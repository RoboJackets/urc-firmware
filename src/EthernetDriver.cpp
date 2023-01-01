#include "EthernetDriver.hpp"

namespace ethernet_driver {

elapsedMillis EthernetDriver::_sendTimer;

EthernetDriver::EthernetDriver(String serverIP, String clientIP, String macAddress, uint16_t port) {
    this->setServerIP(serverIP);
    this->setClientIP(clientIP);
}

void EthernetDriver::sendTestMessage() {
  _udp.beginPacket(_clientIP, _port);
  _udp.write("hello");
  _udp.endPacket();
}

bool EthernetDriver::requestReady() {
  _udp.parsePacket();
  return _udp.available() != 0;
}

bool EthernetDriver::receiveRequest(RequestMessage requestMessage) {
  uint8_t requestBuffer[256];
  size_t requestLength = _udp.parsePacket();

  _udp.read(requestBuffer, requestLength);

  pb_istream_t istream = pb_istream_from_buffer(requestBuffer, sizeof(requestBuffer));
  return pb_decode(&istream, RequestMessage_fields, &requestMessage);
}


bool EthernetDriver::sendEncoderMessages(DriveEncodersMessage driveEncodersMessage) {
  uint8_t responsebuffer[256];
  size_t response_length;

  pb_ostream_t ostream = pb_ostream_from_buffer(responsebuffer, sizeof(responsebuffer));
  bool status = pb_encode(&ostream, DriveEncodersMessage_fields, &driveEncodersMessage);
  response_length = ostream.bytes_written;

  _udp.beginPacket(_clientIP, _port);
  _udp.write(responsebuffer, response_length);
  _udp.endPacket();

  return status;
}

}  // namespace ethernet_driver