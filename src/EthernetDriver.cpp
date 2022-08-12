#include "EthernetDriver.h"

namespace ethernet_driver {

    EthernetDriver::EthernetDriver() 
        : clientIP(CLIENT_IP_ADDR), messageTimer() {
        
        printFlag = false;
    }

    void EthernetDriver::initDriver() {
        Ethernet.begin(SERVER_MAC_ADDR, clientIP);

        // Check for Ethernet hardware present
        if (Ethernet.hardwareStatus() == EthernetNoHardware) {
            Serial.println("Ethernet shield was not found. Sorry, can't run without hardware. :(");
            while (true) {
                delay(10); // do nothing, no point running without Ethernet hardware
            }
        }
        if (Ethernet.linkStatus() == LinkOFF) {
            Serial.println("Ethernet cable is not connected.");
        }

        // start UDP
        udp.begin(UDP_PORT);
        messageTimer.begin(setPrintFlag, TIMER_DURATION);
    }
    
}