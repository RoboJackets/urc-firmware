#ifndef ETHERNETDRIVER_HPP
#define ETHERNETDRIVER_HPP

#include <Arduino.h>
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>

#include "pb_encode.h"
#include "pb_decode.h"
#include "urc.pb.h"

namespace ethernet_driver {

static uint8_t SERVER_IP_ADDR[] = { 192, 168, 8, 167 };
static uint8_t CLIENT_IP_ADDR[] = { 192, 168, 8, 255 };
static uint8_t SERVER_MAC_ADDR[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
static uint16_t UDP_PORT = 8443;
static uint32_t TIMER_DURATION_MS = 100;

class EthernetDriver {
public:
  EthernetDriver();
  ~EthernetDriver(){};
  void sendTestMessage();
  bool requestReady();
  bool receiveRequest(RequestMessage requestMessage);
  bool sendEncoderMessages(DriveEncodersMessage driveEncodersMessage);

  unsigned long getSendTimer() { return sendTimer; }
  void setSendTimer(unsigned long time) { sendTimer = time; }
  bool sendTimeHasElapsed() { return sendTimer >= TIMER_DURATION_MS; }
  void resetSendTimer() { sendTimer -= TIMER_DURATION_MS; }

private:
  IPAddress clientIP;
  IPAddress serverIP;
  EthernetUDP udp;
  static elapsedMillis sendTimer;
};

}  // namespace ethernet_driver

#endif