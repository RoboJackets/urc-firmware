#include <SPI.h>
#include "mcp_can.h"

#define SPI_CS_PIN  10 

MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

void setup() {
    Serial.begin(115200);
    while(!Serial);
    
    while (CAN_OK != CAN.begin(CAN_1000KBPS)) {
        Serial.println("CAN BUS FAIL!");
        delay(100);
    }

    Serial.println("CAN BUS OK!");
}


void loop() {
    unsigned char len = 0;
    unsigned char buf[8];

    if(CAN_MSGAVAIL == CAN.checkReceive()) {
        CAN.readMsgBuf(&len, buf);

        unsigned long canId = CAN.getCanId();
        
        Serial.println("-----------------------------");
        Serial.print("Get data from ID: ");
        Serial.println(canId, HEX);

        for(int i = 0; i < len; i++) {
            Serial.print(buf[i], HEX);
            Serial.print("\t");
        }

        Serial.println();
    }
}

// END FILE
