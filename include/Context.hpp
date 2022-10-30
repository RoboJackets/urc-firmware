#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <Arduino.h>
#include "EthernetDriver.hpp"
#include "SoloDriver.hpp"
/**
 * @brief Singleton class containing all instances of peripheral drivers
 *
 */
class Context {
public:
  Context(){};
  ~Context(){};
  ethernet_driver::EthernetDriver &getEthernetDriver() {
    return ethernet_driver;
  }

  solo_driver::SoloDriver &getSoloDriver() {
    return solo_driver;
  }

  
  elapsedMillis &getCurrentTime() {
    return currentTime;
  }

private:
  ethernet_driver::EthernetDriver ethernet_driver;
  solo_driver::SoloDriver solo_driver;
  elapsedMillis currentTime;
};

#endif