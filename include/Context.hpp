#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <Arduino.h>
#include "EthernetDriver.hpp"

class Context {
    public:
        ethernet_driver::EthernetDriver& getEthernetDriver() { return ethernet_driver; }
        elapsedMillis& getCurrentTime() { return currentTime; }
    protected:
        ethernet_driver::EthernetDriver ethernet_driver;
        elapsedMillis currentTime;
};


#endif