#include "EthernetDriver.h"

namespace ethernet_driver {

    EthernetDriver::EthernetDriver() 
        : clientIP(CLIENT_IP_ADDR) {
        
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
    }

    void EthernetDriver::sendMessage() {
        udp.beginPacket(clientIP, UDP_PORT);
        udp.write("hello");
        udp.endPacket();
    }
    
}