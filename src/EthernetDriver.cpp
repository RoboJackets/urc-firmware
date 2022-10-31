#include "EthernetDriver.hpp"

namespace ethernet_driver {

elapsedMillis EthernetDriver::sendTimer;

EthernetDriver::EthernetDriver() : clientIP(CLIENT_IP_ADDR), serverIP(SERVER_IP_ADDR), udp() {
  Ethernet.begin(SERVER_MAC_ADDR, serverIP);
  udp.begin(UDP_PORT);
}


bool EthernetDriver::write(uint8_t buffer[], size_t len) {

  sendBufferLength = len;
  size_t i;

  for (i = 0; i < len; i++)
    sendBuffer[i] = buffer[i];

  for (; i < BUFFER_LENGTH; i++)
    sendBuffer[i] = 0;

  return true;
}


void EthernetDriver::sendDataInBuffer() {
  udp.beginPacket(CLIENT_IP_ADDR, UDP_PORT);
  udp.write(sendBuffer, sendBufferLength);
  udp.endPacket();
}

void EthernetDriver::execute() {
  
  if (sendTimeHasElapsed()) {

    sendDataInBuffer();
    resetSendTimer();
  }
}

}  // namespace ethernet_driver