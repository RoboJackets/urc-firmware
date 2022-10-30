#include <Arduino.h>
#include "SoloDriver.hpp"

namespace solo_driver {
  SoloDriver::SoloDriver(){
  }
  void SoloDriver::begin(){
    Serial3.begin(115200);
  }

  


}