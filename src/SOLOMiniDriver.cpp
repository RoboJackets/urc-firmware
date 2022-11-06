#include <Arduino.h>
#include "SoloDriver.hpp"

namespace solo_driver {
  SoloDriver::SoloDriver(){
  }
  
  void SoloDriver::begin(){
    Serial3.begin(115200);
    delay(50);
    sendPacket(2, 1);


  }

  void SoloDriver::sendPacket(int command, int data) {
    Serial3.write(65535);
    Serial3.write(0);
    Serial3.write(command);
    Serial3.write(data);
    Serial3.write(0);
    Serial3.write(254);
  }

  void So




}