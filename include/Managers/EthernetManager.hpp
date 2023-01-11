#ifndef ETHERNETMANAGER_HPP
#define ETHERNETMANAGER_HPP

#include <Arduino.h>
#include "Context.hpp"

namespace manager {

class EthernetManager {
public:
  EthernetManager(Context &context);
  void update(Context &context);
};

}  // namespace manager

#endif