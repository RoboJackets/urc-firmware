#include <Arduino.h>

#include "pb_encode.h"
#include "pb_decode.h"
#include "urc.pb.h"

#define HWSERIAL Serial3


namespace solo_driver {

class SoloDriver {
public:
  SoloDriver();
  ~SoloDriver(){};
  void begin();
  bool sendEncoderMessages(DriveEncodersMessage driveEncodersMessage);

private:

};

} 
