#include <FlexCAN_T4.h>

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> myCan1;

elapsedMillis blinkTimer;
elapsedMillis sendTimer;

const int BLINK_RATE = 500;
const int BAUD_RATE = 1000000;
const int SOLO_ID = 0xA4;
const int REQUEST = 0x40;
const int RESPONSE = 0x42;
const int TEMP_CODE = 0x3039;

CAN_message_t rmsg;

struct CanOpenData {
  uint16_t id;
  uint8_t type;
  uint16_t code;
  uint32_t payload;
};

CAN_message_t createMessage(struct CanOpenData data);
struct CanOpenData parseMessage(CAN_message_t msg);

void sendRequest();
void printTemp(char *buf, int len);

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
    
    struct CanOpenData data = parseMessage(rmsg);

    if (data.type == RESPONSE && data.code == TEMP_CODE) {
      Serial.print("Got temp value: ");
      float temp = data.payload / 131072.0;
      Serial.println(temp);
    } else {
      Serial.println("Got something else!");
    }
  
  }

  // send CAN message
  if (sendTimer >= BLINK_RATE) {
    sendTimer -= BLINK_RATE;
    struct CanOpenData data = (struct CanOpenData){
      .id = 0x0600 + SOLO_ID,
      .type = REQUEST,
      .code = TEMP_CODE,
      .payload = 0
    };
    myCan1.write(createMessage(data));
  }

  // blink LED
  if (blinkTimer >= BLINK_RATE) {
    blinkTimer -= BLINK_RATE;
    digitalToggle(LED_BUILTIN);
  }

}

CAN_message_t createMessage(struct CanOpenData data) {
  CAN_message_t msg;
  msg.len = 8;
  msg.id = 0x7FF & data.id;
  msg.buf[0] = 0x00FF & data.type;
  msg.buf[1] = 0x00FF & data.code;
  msg.buf[2] = (0xFF00 & data.code) >> 8;
  msg.buf[3] = 0;
  msg.buf[4] = 0x000000FF & data.payload;
  msg.buf[5] = 0x0000FF00 & data.payload;
  msg.buf[6] = 0x00FF0000 & data.payload;
  msg.buf[7] = 0xFF000000 & data.payload;

  return msg;
}

struct CanOpenData parseMessage(CAN_message_t msg) {
  struct CanOpenData data;
  data.id = msg.id;
  data.type = msg.buf[0];
  data.code = (msg.buf[2] << 8) | msg.buf[1];
  data.payload = (msg.buf[7] << 24) | (msg.buf[6] << 16) | (msg.buf[5] << 8) | msg.buf[4];

  return data;
}
