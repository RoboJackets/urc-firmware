#include <FlexCAN_T4.h>

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> myCan1;
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> myCan2;

elapsedMillis blinkTimer;
const int BLINK_RATE = 500;

elapsedMillis sendTimer;
const int SEND_TIME = 1000;

CAN_message_t msg;
CAN_message_t rmsg;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  myCan1.begin();
  myCan2.begin();
  myCan1.setBaudRate(250 * 1000);
  myCan2.setBaudRate(250 * 1000);
}
void loop() {

  if (sendTimer >= SEND_TIME) {
    sendTimer -= SEND_TIME;
    //setup message
    msg.len = 8;
    msg.id = 10;
    msg.buf[0] = 1;
    msg.buf[1] = 2;
    msg.buf[2] = 3;
    msg.buf[3] = 4;
    msg.buf[4] = 5;
    msg.buf[5] = 6;
    msg.buf[6] = 7;
    msg.buf[7] = 8;
    myCan1.write(msg);
  }
  
//  //modify message
//  msg.id = 2;
//  msg.buf[6] = 4;
//  msg.buf[7] = 2;
//  
//  myCan2.write(msg);



  
  if (myCan1.read(rmsg)) {
    Serial.print("CAN1 ");
    Serial.print("  ID: 0x"); 
    Serial.print(rmsg.id, HEX ); 
    //Serial.print(int(rmsg.buf));
    Serial.print("\n");
  }
  
  if (myCan2.read(rmsg)) {
    Serial.print("CAN2 ");
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
