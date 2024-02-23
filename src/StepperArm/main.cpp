#include <Arduino.h>
#include <TMC2209.h>
#include <QNEthernet.h>
#include <vector>
#include <RoboClaw.h>
#include "urc.pb.h"
#include "Messages.hpp"

// constants 
const int BLINK_RATE_MS = 500;
const int STEPPER_UPDATE_RATE_MS = 100;
const int MOTOR_UPDATE_RATE = 100;
// const int32_t RUN_VELOCITY = 20000;
// const int32_t RUN_VELOCITY = 30000;
const int32_t RUN_VELOCITY = 40000;
// const int32_t RUN_VELOCITY = 120000;
const int32_t STOP_VELOCITY = 0;
const int RUN_DURATION = 5000;
const int STOP_DURATION = 1000;
const int RUN_COUNT = 4;
const int PORT = 8443;
const int ROBOCLAW_ADDR = 0x81;

const long SERIAL_BAUD_RATE = 115200;
const uint8_t RUN_CURRENT_PERCENT = 100;
const uint8_t HOLD_CURRENT_STANDSTILL = 0;

// variables
qindesign::network::EthernetUDP udp;
TMC2209 stepper_driver;
DriveEncodersMessage requestMessage;

// timer variables
elapsedMillis blinkTimer;
elapsedMillis stepperUpdateTimer;
elapsedMillis motorUpdateTimer;
RoboClaw roboclaw(&Serial2, 38400);

std::vector<int>::iterator mySpeed;
std::vector<int> stepperSpeeds;


void setup_test_stepper();
void test_stepper();
void test_stepper_2();
void run_stepper();

int main() {
    // LED setup
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(2, OUTPUT);
    digitalWrite(2, LOW);

    // Ethernet setup
    qindesign::network::Ethernet.begin();
    udp.begin(PORT);
    requestMessage = DriveEncodersMessage_init_zero;
    uint8_t requestBuffer[256];
    size_t requestLength;

     // Roboclaw setup
    roboclaw.begin(38400);


    // Stepper setup
    Serial1.begin(SERIAL_BAUD_RATE);
    stepper_driver.setup(Serial1);
    stepper_driver.setRunCurrent(RUN_CURRENT_PERCENT);
    stepper_driver.setHoldCurrent(HOLD_CURRENT_STANDSTILL);
    stepper_driver.enable();
    stepper_driver.moveAtVelocity(STOP_VELOCITY);
    delay(STOP_DURATION);
    stepper_driver.moveAtVelocity(RUN_VELOCITY);
    
    // // test
    setup_test_stepper();


    while (true) {

        // read incoming UDP messages
        requestLength = udp.parsePacket();
        if (udp.available()) { 

            Serial.print("Packet received: ");

            memset(requestBuffer, 0, 256);
            udp.readBytes(requestBuffer, requestLength);
            protobuf::Messages::decodeRequest(requestBuffer, requestLength, requestMessage);
        
            Serial.print("left: ");
            Serial.print(requestMessage.leftSpeed);
        }

        if (motorUpdateTimer >= MOTOR_UPDATE_RATE) {
            motorUpdateTimer -= MOTOR_UPDATE_RATE;

            bool isReversed = (requestMessage.rightSpeed < 0); 
            uint8_t requestedSpeed = abs((requestMessage.rightSpeed / 3000.0) * 100.0);
            Serial.printf("Requested speed: %d\n", requestedSpeed);
            if (isReversed) {
              roboclaw.BackwardM2(ROBOCLAW_ADDR, requestedSpeed);
            } else {
              roboclaw.ForwardM2(ROBOCLAW_ADDR, requestedSpeed);
            }
            
            // roboclaw.ForwardM1(ROBOCLAW_ADDR,0); //start Motor1 forward at half speed
            // roboclaw.BackwardM2(ROBOCLAW_ADDR,64); //start Motor2 backward at half speed

            // if (roboclaw.ReadVersion(ROBOCLAW_ADDR, version)){
            //     Serial.println(version);
            // } else {
            //   Serial.println("Error!");
            // }
        }

        if (stepperUpdateTimer >= STEPPER_UPDATE_RATE_MS) {
            stepperUpdateTimer -= STEPPER_UPDATE_RATE_MS;
            // run_stepper();
            test_stepper_2();
        }


        if (blinkTimer >= BLINK_RATE_MS) {
            blinkTimer -= BLINK_RATE_MS;
            digitalToggle(LED_BUILTIN);
        }
    }
}

void setup_test_stepper() {
    stepperSpeeds.push_back(0);
    stepperSpeeds.push_back(-1000);
    stepperSpeeds.push_back(0);
    stepperSpeeds.push_back(-4000);
    stepperSpeeds.push_back(0);
    stepperSpeeds.push_back(-8000);
    stepperSpeeds.push_back(1000);
    stepperSpeeds.push_back(0);
    stepperSpeeds.push_back(4000);
    stepperSpeeds.push_back(8000);

    mySpeed = stepperSpeeds.begin();
}

void test_stepper() {
    stepper_driver.moveAtVelocity(RUN_VELOCITY);
    stepper_driver.disableInverseMotorDirection();
    stepper_driver.enable();
    delay(RUN_DURATION);
    stepper_driver.disable();
    delay(STOP_DURATION);
    stepper_driver.enableInverseMotorDirection();
    stepper_driver.enable();
    delay(RUN_DURATION);
    stepper_driver.disable();
    delay(STOP_DURATION);
}

void test_stepper_2() {
    // stepper_driver.disable();
    if (*mySpeed < 0) {
        stepper_driver.disableInverseMotorDirection();
    } else {
        stepper_driver.enableInverseMotorDirection();
    }
    
    int run_speed = abs(*mySpeed) * 10;
    stepper_driver.moveAtVelocity(run_speed);
    stepper_driver.enable();

    mySpeed++;
    if (mySpeed == stepperSpeeds.end()) {
        mySpeed = stepperSpeeds.begin();
    }
}

void run_stepper() {
    // stepper_driver.disable();
    if (requestMessage.leftSpeed < 0) {
        stepper_driver.disableInverseMotorDirection();
    } else {
        stepper_driver.enableInverseMotorDirection();
    }
    
    int run_speed = abs(requestMessage.leftSpeed) * 50;

    if (run_speed >= 1000) {
        stepper_driver.moveAtVelocity(run_speed);
        stepper_driver.enable();
    }  else {
        stepper_driver.disable();
    }
}

