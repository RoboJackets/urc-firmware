#include <mcp_can.h>
#include <SPI.h>

#define SPI_CS_PIN  10 

MCP_CAN CAN(SPI_CS_PIN);

void setup() {
   Serial.begin(115200);
   while(!Serial);
    
    while (CAN_OK != CAN.begin(CAN_500KBPS)) {
       Serial.println("CAN BUS FAIL!");
        delay(100);
    }
   Serial.println("CAN BUS OK!");
}

unsigned char stmp[8] = {0, 1, 2, 3, 4, 5, 6, 7};

void loop() {
    CAN.sendMsgBuf(0x0A, 0, 8, stmp);
    delay(100);
}

// END FILE
