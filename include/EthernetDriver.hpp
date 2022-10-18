#ifndef ETHERNETDRIVER_H
#define ETHERNETDRIVER_H

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
static uint32_t TIMER_DURATION = 100000;

class EthernetDriver {
public:
  EthernetDriver();
  ~EthernetDriver(){};
  void sendTestMessage();
  bool sendEncoderMessages(DriveEncodersMessage driveEncodersMessage);

  bool sendTimeHasElapsed() { return sendTimer >= TIMER_DURATION; }
  void resetSendTimer() { sendTimer = TIMER_DURATION; }

private:
  IPAddress clientIP;
  IPAddress serverIP;
  EthernetUDP udp;
  elapsedMillis sendTimer;
};

}  // namespace ethernet_driver

#endif