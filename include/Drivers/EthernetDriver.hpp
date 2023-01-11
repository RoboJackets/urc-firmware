#ifndef ETHERNETDRIVER_HPP
#define ETHERNETDRIVER_HPP

#include <Arduino.h>
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>

namespace ethernet {

const uint8_t MAC[] = { 0xC0, 0xFF, 0xEE, 0x01 };
const uint8_t LOCAL_IP[] = { 192, 168, 8, 167 };
const uint8_t CLIENT_IP[] = { 192, 168, 8, 150 };
const uint8_t GATEWAY[] = { 0, 0, 0, 0 };
const uint8_t DNS[] = { 0, 0, 0, 0 };
const uint8_t SUBNET[] = { 255, 255, 255, 0 };
const uint16_t PORT = 8443;

class EthernetDriver : public EthernetUDP {

public:
  const static uint32_t TIMER_DURATION_MS = 100;

  static bool sendTimeHasElapsed();
  static void resetSendTimer();

private:
  static elapsedMillis _sendTimer;
};

}  // namespace ethernet

#endif