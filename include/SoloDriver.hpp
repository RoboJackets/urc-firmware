#include <Arduino.h>
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>

#include "pb_encode.h"
#include "pb_decode.h"
#include "urc.pb.h"

namespace solo_driver {

static uint8_t SERVER_IP_ADDR[] = { 192, 168, 8, 167 };
static uint8_t CLIENT_IP_ADDR[] = { 192, 168, 8, 255 };
static uint8_t SERVER_MAC_ADDR[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
static uint16_t UDP_PORT = 8443;
static uint32_t TIMER_DURATION_MS = 100;

class SoloDriver {
public:
  SoloDriver();
  ~SoloDriver(){};
  void sendTestMessage();
  bool sendEncoderMessages(DriveEncodersMessage driveEncodersMessage);

private:

};

} 
