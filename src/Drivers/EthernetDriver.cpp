#include "Drivers/EthernetDriver.hpp"

namespace ethernet {

elapsedMillis EthernetDriver::_sendTimer;

bool EthernetDriver::sendTimeHasElapsed() {
  return _sendTimer >= TIMER_DURATION_MS;
}

void EthernetDriver::resetSendTimer() {
  _sendTimer -= TIMER_DURATION_MS;
}

}  // namespace ethernet