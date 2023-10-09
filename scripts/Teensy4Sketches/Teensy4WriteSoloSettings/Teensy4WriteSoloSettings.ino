#include <FlexCAN_T4.h>

// constants
const int BLINK_RATE_MS = 500;
const int READ_RATE_MS = 1000;
const int STATE_UPDATE_RATE_MS = 5000;
const int BAUD_RATE = 1000000;

const int SDO_READ_COMMAND = 0x40;
const int SDO_READ_RESPONSE = 0x42;
const int SDO_WRITE_COMMAND = 0x22;
const int SDO_WRITE_RESPONSE = 0x60;

const int TEMP_CODE = 0x3039;
const int SPEED_REF_CODE = 0x3005;
const int SPEED_FEEDBACK_CODE = 0x3036;
const int POSITION_FEEDBACK_CODE = 0x3037;
const int MOTOR_DIRECTION_CODE = 0x300C;

const int MOTOR_SPEED = 3000;
const int NUM_MOTORS = 3;
const int MOTOR_IDS[NUM_MOTORS] = {0xA4, 0xA3, 0xA6};

struct CanOpenData {
  uint16_t id;
  uint8_t type;
  uint16_t code;
  uint32_t payload;
};

enum MotorState {
  MOTOR_STATE_FORWARD,
  MOTOR_STATE_BACKWARD,
  MOTOR_STATE_STOPPED
};

// variables
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> myCan1;

elapsedMillis blinkTimer;
elapsedMillis sendTimer;
elapsedMillis stateMachineTimer;
CAN_message_t rmsg;

enum MotorState curr = MOTOR_STATE_STOPPED;
enum MotorState prev = MOTOR_STATE_STOPPED;

// functions
void SetSpeedReferenceCommand(int soloID, int speedRef);
void GetSpeedFeedbackCommand(int soloID);
void GetPositionFeedbackCommand(int soloID);
enum MotorState determineNextState(enum MotorState curr, enum MotorState prev);
void printCANMessage(CAN_message_t message);

CAN_message_t createMessage(struct CanOpenData data);
struct CanOpenData parseMessage(CAN_message_t msg);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  myCan1.begin();
  myCan1.setBaudRate(BAUD_RATE);
}

void loop() {

  // parse incoming CAN messages
  if (myCan1.read(rmsg)) {
//    printCANMessage(rmsg);

    struct CanOpenData data = parseMessage(rmsg);

    if (data.type == SDO_READ_RESPONSE) {
      if (data.code == SPEED_FEEDBACK_CODE) {
        Serial.print("Got speed feedback: ");
        Serial.print("[ID=");
        Serial.print(data.id, HEX);
        Serial.print(", SPEED=");
        Serial.print((int)data.payload);
        Serial.println("]");
      } else if (data.code == POSITION_FEEDBACK_CODE) {
        Serial.print("Got position feedback: ");
        int ref = data.payload;
        Serial.println(ref);
      }
    }
  }

  // set speed
  if (stateMachineTimer >= STATE_UPDATE_RATE_MS) {
    stateMachineTimer -= STATE_UPDATE_RATE_MS;

    // determine new state 
    enum MotorState newState = determineNextState(curr, prev);
    
    // determine motor speed
    int motorSpeed;
    if (newState == MOTOR_STATE_FORWARD) {
      motorSpeed = MOTOR_SPEED;
    } else if (newState == MOTOR_STATE_BACKWARD) {
      motorSpeed = -1 * MOTOR_SPEED;
    } else {
      motorSpeed = 0;
    }

    // send command


    for (int i = 0; i < NUM_MOTORS; i++) {
      SetSpeedReferenceCommand(MOTOR_IDS[i], motorSpeed);
    }
    

    // update state
    prev = curr;
    curr = newState;
  }

  // read speed reference
  if (sendTimer >= READ_RATE_MS) {
    sendTimer -= READ_RATE_MS;

    for (int i = 0; i < NUM_MOTORS; i++) {
      GetSpeedFeedbackCommand(MOTOR_IDS[i]);
    }
  }

  // blink LED
  if (blinkTimer >= BLINK_RATE_MS) {
    blinkTimer -= BLINK_RATE_MS;
    digitalToggle(LED_BUILTIN);
  }

}

enum MotorState determineNextState(enum MotorState curr, enum MotorState prev) {
  if (curr == MOTOR_STATE_STOPPED) {
    if (prev == MOTOR_STATE_FORWARD) {
      return MOTOR_STATE_BACKWARD;
    } else {
      return MOTOR_STATE_FORWARD;
    }
  } else {
    return MOTOR_STATE_STOPPED;
  }
}

void printCANMessage(CAN_message_t message) {
  Serial.print("CAN1 ");
  Serial.print("  ID: 0x"); 
  Serial.print(message.id, HEX); 

  Serial.print(" DATA: ");
  char digit[3];
   for (int i = 0; i < message.len; i++) {
    sprintf(digit, "%02x", message.buf[i]);
    Serial.print(digit);
    Serial.print(" ");
   }

  Serial.print("\n");
}

void SetSpeedReferenceCommand(int soloID, int speedRef) {
  
  uint32_t dir = (uint32_t)(speedRef < 0 ? 1 : 0);
  uint32_t speedMag = (uint32_t)abs(speedRef);
  struct CanOpenData data;

  Serial.print("Speed ref: ");
  Serial.println(speedMag);

  // set speed
  data = (struct CanOpenData) {
    .id = (uint16_t)(0x0600 + soloID),
    .type = SDO_WRITE_COMMAND,
    .code = SPEED_REF_CODE,
    .payload = speedMag
  };

  myCan1.write(createMessage(data));
  delayMicroseconds(200);

  // set direction
  data = (struct CanOpenData) {
    .id = (uint16_t)(0x0600 + soloID),
    .type = SDO_WRITE_COMMAND,
    .code = MOTOR_DIRECTION_CODE,
    .payload = dir
  };
  
  myCan1.write(createMessage(data));
  delayMicroseconds(200);
}

void GetSpeedFeedbackCommand(int soloID) {
  struct CanOpenData data = {
    .id = (uint16_t)(0x0600 + soloID),
    .type = SDO_READ_COMMAND,
    .code = SPEED_FEEDBACK_CODE,
    .payload = 0
  };
  myCan1.write(createMessage(data));
  delayMicroseconds(200);
}

void GetPositionFeedbackCommand(int soloID) {
  struct CanOpenData data = {
    .id = (uint16_t)(0x0600 + soloID),
    .type = SDO_READ_COMMAND,
    .code = POSITION_FEEDBACK_CODE,
    .payload = 0
  };
  myCan1.write(createMessage(data));
  delayMicroseconds(200);
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
  msg.buf[5] = (0x0000FF00 & data.payload) >> 8;
  msg.buf[6] = (0x00FF0000 & data.payload) >> 16;
  msg.buf[7] = (0xFF000000 & data.payload) >> 24;

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
