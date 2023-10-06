#include <FlexCAN_T4.h>

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> myCan1;

elapsedMillis blinkTimer;
const int BLINK_RATE = 500;

CAN_message_t rmsg;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  myCan1.begin();
  myCan1.setBaudRate(250 * 1000);
}
void loop() {

  if (myCan1.read(rmsg)) {
    Serial.print("CAN1 ");
    Serial.print("  ID: 0x"); 
    Serial.print(rmsg.id, HEX ); 
    //Serial.print(int(rmsg.buf));
    Serial.print("\n");
  }
  

  // blink LED
  if (blinkTimer >= BLINK_RATE) {
    blinkTimer -= BLINK_RATE;
    digitalToggle(LED_BUILTIN);
  }

}
