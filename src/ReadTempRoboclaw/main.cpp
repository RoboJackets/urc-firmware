#include <Arduino.h>
// #include <QNEthernet.h>
#include <RoboClaw.h>
#include "Messages.hpp"

// constants
const int BLINK_RATE = 1000;
const int SEND_RATE = 1000;
// const int STATE_UPDATE_RATE_MS = 5000;
const int ROBOCLAW_ADDR = 0x80;
// const int MOTOR_SPEED = 3000000;
// const int PORT = 8443;
// const uint8_t CLIENT_IP[] = { 192, 168, 1, 151 };

// variables
// qindesign::network::EthernetUDP udp;
RequestMessage requestMessage;
DriveEncodersMessage responseMessage;

// timer variables
elapsedMillis blinkTimer;
elapsedMillis sendTimer;
elapsedMillis stateMachineTimer;
RoboClaw roboclaw(&Serial2, 10000);

enum MotorState {
  MOTOR_STATE_FORWARD,
  MOTOR_STATE_BACKWARD,
  MOTOR_STATE_STOPPED
};

enum MotorState curr = MOTOR_STATE_STOPPED;
enum MotorState prev = MOTOR_STATE_STOPPED;

enum MotorState determineNextState(enum MotorState curr, enum MotorState prev);

int main() {
    // LED setup
    pinMode(LED_BUILTIN, OUTPUT);

    // Ethernet setup
    // qindesign::network::Ethernet.begin();
    // udp.begin(PORT);

    // Roboclaw setup
    roboclaw.begin(38400);

    char version[32];

    size_t requestLength;
    uint8_t requestBuffer[256];
    uint8_t responseBuffer[256];

    while (true) {


        // // // TEST 1: read version
        // if (sendTimer >= SEND_RATE) {
        //     sendTimer -= BLINK_RATE;

        //     roboclaw.ForwardM1(ROBOCLAW_ADDR,0); //start Motor1 forward at half speed
        //     // roboclaw.BackwardM2(ROBOCLAW_ADDR,64); //start Motor2 backward at half speed

        //     // if (roboclaw.ReadVersion(ROBOCLAW_ADDR, version)){
        //     //     Serial.println(version);
        //     // } else {
        //     //   Serial.println("Error!");
        //     // }
        // }

        // // TEST 2: set speed
        // if (stateMachineTimer >= STATE_UPDATE_RATE_MS) {
        //     stateMachineTimer -= STATE_UPDATE_RATE_MS;

        //     // determine new state 
        //     enum MotorState newState = determineNextState(curr, prev);
            
        //     // determine motor speed
        //     int motorSpeed;
        //     if (newState == MOTOR_STATE_FORWARD) {
        //         motorSpeed = MOTOR_SPEED;
        //     } else if (newState == MOTOR_STATE_BACKWARD) {
        //         motorSpeed = -1 * MOTOR_SPEED;
        //     } else {
        //         motorSpeed = 0;
        //     }

        //     // send command
        //     roboclaw.SpeedM1(ROBOCLAW_ADDR, motorSpeed);
        //     // update state
        //     prev = curr;
        //     curr = newState;
        // }

        // // read incoming UDP messages
        // requestLength = udp.parsePacket();
        // if (udp.available()) { 

        //     Serial.print("Packet received: ");

        //     memset(requestBuffer, 0, 256);
        //     udp.readBytes(requestBuffer, requestLength);
        //     protobuf::Messages::decodeRequest(requestBuffer, requestLength, requestMessage);
        
        //     // // write Speed
        //     roboclaw.SpeedM1(ROBOCLAW_ADDR, requestMessage.leftSpeed * 1000);

        //     // value is between -3000 and 3000

        //     // int position = 1138688 * (((requestMessage.leftSpeed / 3000.0) + 1.0) / 2.0);
        //     // Serial.println(position);

        //     // // write position
        //     // roboclaw.SpeedAccelDeccelPositionM1(ROBOCLAW_ADDR, 50000, 120000, 50000, position, 0);
        //     // roboclaw.SpeedAccelDeccelPositionM1(ROBOCLAW_ADDR, 0, 956625, 0, position, 0);
        // }


        // blink LED
        if (blinkTimer >= BLINK_RATE) {
            blinkTimer -= BLINK_RATE;
            roboclaw.ForwardM1(0x80,0); 
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