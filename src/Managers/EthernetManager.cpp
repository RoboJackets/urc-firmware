#include "Managers/Managers.hpp"
#define DEBUG false

namespace manager {

EthernetManager::EthernetManager(Context &context) {
  ethernet::EthernetDriver &ethernetDriver = context.getEthernetDriver();
  Ethernet.begin(ethernet::MAC, IPAddress(ethernet::LOCAL_IP), IPAddress(ethernet::DNS), IPAddress(ethernet::GATEWAY), IPAddress(ethernet::SUBNET));
  ethernetDriver.begin(ethernet::PORT);

#if DEBUG

  Serial.println("Ethernet set up.");
  Serial.print("\t MAC address: ");

  uint8_t buffer[6];
  Ethernet.MACAddress(buffer);

  Serial.println(String((char *)buffer));

  Serial.print("\t Server IP (Teensy 4.1): ");
  Serial.println(Ethernet.localIP());
  Serial.print("\t Client IP (computer): ");
  Serial.println(IPAddress(ethernet::CLIENT_IP));
  Serial.print("\t DNS: ");
  Serial.println(Ethernet.dnsServerIP());
  Serial.print("\t Gateway: ");
  Serial.println(Ethernet.gatewayIP());
  Serial.println("");

#endif
}

void EthernetManager::update(Context &context) {
  ethernet::EthernetDriver &ethernetDriver = context.getEthernetDriver();
  DriveEncodersMessage &driveEncodersMessage = context.getDriveEncodersMessage();
  RequestMessage &requestMessage = context.getRequestMessage();

  // If request from client is ready, read it into requestMessage

  size_t requestLength = ethernetDriver.parsePacket();
  if (ethernetDriver.available()) {
    uint8_t requestBuffer[256];

    ethernetDriver.read(requestBuffer, requestLength);
    protobuf::Messages::decodeRequest(requestBuffer, requestLength, requestMessage);

#if DEBUG

    Serial.print(context.getCurrentTime());
    Serial.print(": EthernetDriver received ");
    Serial.print(requestLength);
    Serial.println(" bytes");

#endif
  }

  // if TIMER_DURATION_MS has elapsed, send a response
  if (ethernet::EthernetDriver::sendTimeHasElapsed()) {
    uint8_t responseBuffer[256];
    driveEncodersMessage.timestamp = context.getCurrentTime();

    size_t bytesWritten =
        protobuf::Messages::encodeResponse(responseBuffer, sizeof(responseBuffer), driveEncodersMessage);

    ethernetDriver.beginPacket(ethernet::CLIENT_IP, ethernet::PORT);
    ethernetDriver.write(responseBuffer, bytesWritten);
    ethernetDriver.endPacket();

#if DEBUG

    Serial.print(driveEncodersMessage.timestamp);
    Serial.print(": EthernetDriver wrote ");
    Serial.print(bytesWritten);
    Serial.println(" bytes");

#endif

    ethernet::EthernetDriver::resetSendTimer();
  }
}

}  // namespace manager