#include <Arduino.h>

#include "pb_encode.h"
#include "pb_decode.h"
#include "urc.pb.h"


namespace solo_driver {

class SoloDriver {
public:
  SoloDriver(HardwareSerial *inputSerial) {
    serial = inputSerial;
  };

  void begin();
  void sendPacket(int command, int data);
  void setUp();

private:
 HardwareSerial *serial;
};

} 
