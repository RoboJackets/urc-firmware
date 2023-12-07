#include <FlexCAN_T4.h>
#include "SoloCAN.hpp"

// constants
const int BLINK_RATE_MS = 500;
const int READ_RATE_MS = 1000;
const int STATE_UPDATE_RATE_MS = 5000;
const int BAUD_RATE = 1000000;

const int MOTOR_SPEED = 3000;
const int NUM_MOTORS = 3;
const int MOTOR_IDS[NUM_MOTORS] = {0xA1, 0xA2};

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

enum MotorState determineNextState(enum MotorState curr, enum MotorState prev);

int main() {
    pinMode(LED_BUILTIN, OUTPUT);
    myCan1.begin();
    myCan1.setBaudRate(BAUD_RATE);

    solo_can::SoloCan solo = solo_can::SoloCan(myCan1);

    while (true) {

        // parse incoming CAN messages
        if (myCan1.read(rmsg)) {
            struct solo_can::CanOpenData data = solo_can::parseMessage(rmsg);

            if (data.type == solo_can::SDO_READ_RESPONSE) {
                if (data.code == solo_can::SPEED_FEEDBACK_CODE) {
                    Serial.print("Got speed feedback: ");
                    Serial.print("[ID=");
                    Serial.print(data.id, HEX);
                    Serial.print(", SPEED=");
                    Serial.print((int)data.payload);
                    Serial.println("]");
                } else if (data.code == solo_can::POSITION_FEEDBACK_CODE) {
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
                solo.SetSpeedReferenceCommand(MOTOR_IDS[i], motorSpeed);
            }
            

            // update state
            prev = curr;
            curr = newState;
        }

        // read speed reference
        if (sendTimer >= READ_RATE_MS) {
            sendTimer -= READ_RATE_MS;

            for (int i = 0; i < NUM_MOTORS; i++) {
                solo.GetSpeedFeedbackCommand(MOTOR_IDS[i]);
            }
        }

        // blink LED
        if (blinkTimer >= BLINK_RATE_MS) {
            blinkTimer -= BLINK_RATE_MS;
            digitalToggle(LED_BUILTIN);
        }
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
