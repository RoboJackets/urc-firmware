#include <FlexCAN_T4.h>

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> myCan1;

elapsedMillis blinkTimer;
elapsedMillis sendTimer;

const int BLINK_RATE = 500;
const int BAUD_RATE = 1000000;
const int SOLO_ID = 1;

const int SDO_READ_COMMAND = 0x40;
const int SDO_READ_RESPONSE = 0x42;
const int SDO_WRITE_COMMAND = 0x22;
const int SDO_WRITE_RESPONSE = 0x60;

const int TEMP_CODE = 0x3039;
const int SPEED_REF_CODE = 0x3005;

CAN_message_t msg;
CAN_message_t rmsg;

void SetSpeedReference(int speedRef);
void GetSpeedFeedback();

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  myCan1.begin();
  myCan1.setBaudRate(BAUD_RATE);
}

void loop() {

  if (myCan1.read(rmsg)) {
    Serial.print("CAN1 ");
    Serial.print("  ID: 0x"); 
    Serial.print(rmsg.id, HEX); 

    Serial.print(" DATA: ");
    char digit[3];
     for (int i = 0; i < rmsg.len; i++) {
      sprintf(digit, "%02x", rmsg.buf[i]);
      Serial.print(digit);
      Serial.print(" ");
     }

    Serial.print("\n");
    printTemp(rmsg.buf, 8);
  }

  // send CAN message
  if (sendTimer >= BLINK_RATE) {
    sendTimer -= BLINK_RATE;
    sendRequest();
  }

  // blink LED
  if (blinkTimer >= BLINK_RATE) {
    blinkTimer -= BLINK_RATE;
    digitalToggle(LED_BUILTIN);
  }

}

void SetSpeedReference(int speedRef) {
  msg.len = 8;
  msg.id = 0x0600 + SOLO_ID;
  memset(msg.buf, 0, 8);
  msg.buf[0] = 0x00FF & SDO_READ_COMMAND;
  msg.buf[1] = 0x00FF & SPEED_REF_CODE;
  msg.buf[2] = (0xFF00 & SPEED_REF_CODE) >> 8;
  msg.buf[3] = 0;
  msg.buf[4] = 0x00
  myCan1.write(msg);
}


void sendRequest() {
  msg.len = 8;
  msg.id = 0x0600 + SOLO_ID;
  memset(msg.buf, 0, 8);
  msg.buf[0] = 0x00FF & REQUEST;
  msg.buf[1] = 0x00FF & TEMP_CODE;
  msg.buf[2] = (0xFF00 & TEMP_CODE) >> 8;
  myCan1.write(msg);
}

void printTemp(uint8_t *buf, int len) {
  int type = buf[0];
  int code = (buf[2] << 8) | buf[1];

  if (type == RESPONSE && code == TEMP_CODE) {
    Serial.print("Got temp value: ");
  } else {
    Serial.println("Got something else!");
    return;
  }

  double temp = ((buf[7] << 24) | (buf[6] << 16) | (buf[5] << 8) | buf[4]) / 131072.0;
  Serial.println(temp);
}
