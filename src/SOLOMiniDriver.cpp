#include <Arduino.h>
#include "SoloDriver.hpp"

namespace solo_driver {

  void SoloDriver::begin(){
    serial->begin(937500);
    delay(50);
    sendPacket(2, 1);
  }

  void SoloDriver::sendPacket(int command, int data) {
    serial->write(65535);
    serial->write(0);
    serial->write(command);
    serial->write(data);
    serial->write(0);
    serial->write(254);
  }


}