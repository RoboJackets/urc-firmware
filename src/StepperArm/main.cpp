#include <Arduino.h>
#include <TMC2209.h>
#include <QNEthernet.h>
#include <vector>
#include <RoboClaw.h>
#include <string>
#include <unordered_map>
#include "urc.pb.h"
#include "Messages.hpp"

// constants 
const int BLINK_RATE_MS = 500;
const int STEPPER_UPDATE_RATE_MS = 100;
const int MOTOR_UPDATE_RATE = 100;
const int ACTUATOR_UPDATE_RATE = 100;
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


// INPUT RANGE: -600 to 600
// OUTPUT RANGE: 500us to 2500us, 1500 is stopped
elapsedMicros servoUpdateTimer;
const int SERVO_PWM_PIN = 19;
const int SERVO_STOPPED_PULSE_WIDTH = 1500;
const int SERVO_PWM_PERIOD_US = 20000;
bool servo_pwm_high = true;
int servo_wait_us = SERVO_STOPPED_PULSE_WIDTH;


constexpr int ROBOCLAW_SHOULDER_ADDR = 0x82;
constexpr int ROBOCLAW_WRIST_ADDR = 0x81;
constexpr int ROBOCLAW_ELBOW_ADDR = 0x80;

constexpr int ROBOCLAW_CHANNEL_1 = 1;
constexpr int ROBOCLAW_CHANNEL_2 = 2;

const long SERIAL_BAUD_RATE = 38400;
const uint8_t RUN_CURRENT_PERCENT = 100;
const uint8_t HOLD_CURRENT_STANDSTILL = 0;

// pin setup
const int LINEAR_ACTUATOR_TX = 20;
const int LINEAR_ACTUATOR_RX = 21;
const int ENDEFFECTOR_SERVO_PWM = 19;

// variables
qindesign::network::EthernetUDP udp;
TMC2209 stepper_driver;
DriveEncodersMessage requestMessage;
ArmEffortRequest armEffortRequest;
ArmPositionFeedback armPositionFeedback;

// timer variables
elapsedMillis blinkTimer;
elapsedMillis stepperUpdateTimer;
elapsedMillis actuatorUpdateTimer;
elapsedMillis motorUpdateTimer;
RoboClaw roboclaw(&Serial2, 38400);

std::vector<int>::iterator mySpeed;
std::vector<int> stepperSpeeds;

std::unordered_map<int, int> lastCommand;

IPAddress remoteIP;


void setup_test_stepper();
void test_stepper();
void test_stepper_2();
void run_stepper();
void run_stepper_2();
void run_roboclaw_effort(int address, int channel, int effort);
void run_roboclaw_speed(int address, int channel, int speed);

int main() {
    // LED setup
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(2, OUTPUT);
    pinMode(SERVO_PWM_PIN, OUTPUT);
    digitalWrite(2, LOW);

    // Ethernet setup
    qindesign::network::Ethernet.begin();
    udp.begin(PORT);

    armEffortRequest = ArmEffortRequest_init_zero;
    armPositionFeedback = ArmPositionFeedback_init_zero;
    // requestMessage = DriveEncodersMessage_init_zero;
    uint8_t requestBuffer[256];
    size_t requestLength;

    uint8_t responseBuffer[256];
    size_t responseLength;

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
            // protobuf::Messages::decodeRequest(requestBuffer, requestLength, requestMessage);
            protobuf::Messages::decodeRequest(requestBuffer, requestLength, armEffortRequest);

            remoteIP = udp.remoteIP();
        
            // Serial.print("left: ");
            // Serial.print(requestMessage.leftSpeed);
        }

        if (motorUpdateTimer >= MOTOR_UPDATE_RATE) {
            motorUpdateTimer -= MOTOR_UPDATE_RATE;

            //new arm motor effort
            run_roboclaw_effort(ROBOCLAW_ELBOW_ADDR, ROBOCLAW_CHANNEL_1, armEffortRequest.elbowLiftEffort);
            run_roboclaw_effort(ROBOCLAW_ELBOW_ADDR, ROBOCLAW_CHANNEL_2, armEffortRequest.shoulderLiftEffort);
            run_roboclaw_effort(ROBOCLAW_WRIST_ADDR, ROBOCLAW_CHANNEL_1, armEffortRequest.wristSwivelEffort);
            run_roboclaw_effort(ROBOCLAW_WRIST_ADDR, ROBOCLAW_CHANNEL_2, armEffortRequest.wristLiftEffort);
            run_roboclaw_effort(ROBOCLAW_SHOULDER_ADDR, ROBOCLAW_CHANNEL_1, armEffortRequest.shoulderSwivelEffort);

            // // run motor effort
            // run_roboclaw_effort(ROBOCLAW_WRIST_ADDR, ROBOCLAW_CHANNEL_2, armEffortRequest.wristSwivelEffort);
            // run_roboclaw_effort(ROBOCLAW_WRIST_ADDR, ROBOCLAW_CHANNEL_1, armEffortRequest.wristLiftEffort);
            // run_roboclaw_effort(ROBOCLAW_ELBOW_ADDR, ROBOCLAW_CHANNEL_1, armEffortRequest.elbowLiftEffort);
            // run_roboclaw_effort(ROBOCLAW_SHOULDER_ADDR, ROBOCLAW_CHANNEL_1, armEffortRequest.shoulderLiftEffort);
            // run_roboclaw_effort(ROBOCLAW_SHOULDER_ADDR, ROBOCLAW_CHANNEL_2, armEffortRequest.shoulderSwivelEffort);

            // run motor speed
            // run_roboclaw_speed(ROBOCLAW_WRIST_ADDR, ROBOCLAW_CHANNEL_2, armEffortRequest.wristSwivelEffort);
            // run_roboclaw_speed(ROBOCLAW_WRIST_ADDR, ROBOCLAW_CHANNEL_1, armEffortRequest.wristLiftEffort);
            // run_roboclaw_speed(ROBOCLAW_ELBOW_ADDR, ROBOCLAW_CHANNEL_1, armEffortRequest.elbowLiftEffort);
            // run_roboclaw_speed(ROBOCLAW_SHOULDER_ADDR, ROBOCLAW_CHANNEL_1, armEffortRequest.shoulderLiftEffort);
            // run_roboclaw_effort(ROBOCLAW_SHOULDER_ADDR, ROBOCLAW_CHANNEL_2, armEffortRequest.shoulderSwivelEffort);

            // get encoder feedback
            // armPositionFeedback.has_wristSwivelTicks = true;
            // armPositionFeedback.has_wristLiftTicks = true;
            // armPositionFeedback.has_elbowLiftTicks = true;
            // armPositionFeedback.has_shoulderLiftTicks = true;
            // armPositionFeedback.has_shoulderSwivelTicks = true;

            // uint32_t ticks1;
            // uint32_t ticks2;
            // roboclaw.ReadEncoders(ROBOCLAW_WRIST_ADDR, ticks1, ticks2);
            // armPositionFeedback.wristLiftTicks = ticks1;
            // armPositionFeedback.wristSwivelTicks = ticks2;
            // roboclaw.ReadEncoders(ROBOCLAW_ELBOW_ADDR, ticks1, ticks2);
            // armPositionFeedback.elbowLiftTicks = ticks1;
            // roboclaw.ReadEncoders(ROBOCLAW_SHOULDER_ADDR, ticks1, ticks2);
            // armPositionFeedback.shoulderLiftTicks = ticks1;
            // armPositionFeedback.shoulderSwivelTicks = ticks2; 


            // // get encoder feedback
            // uint16_t temp;
            // int32_t enc1, enc2;
            // uint8_t status1, status2;
            // bool valid1, valid2;

            // armPositionFeedback.wristLiftTicks = roboclaw.ReadEncM1(ROBOCLAW_WRIST_ADDR, &status1, &valid1);
            // armPositionFeedback.wristSwivelTicks = roboclaw.ReadEncM2(ROBOCLAW_WRIST_ADDR, &status2, &valid2);
            // armPositionFeedback.elbowLiftTicks = roboclaw.ReadEncM1(ROBOCLAW_ELBOW_ADDR);
            // armPositionFeedback.shoulderLiftTicks = roboclaw.ReadEncM1(ROBOCLAW_SHOULDER_ADDR);
            // armPositionFeedback.shoulderSwivelTicks = roboclaw.ReadEncM2(ROBOCLAW_SHOULDER_ADDR);


            // // write encoder feedback
            // memset(responseBuffer, 0, 256);
            // responseLength = protobuf::Messages::encodeResponse(responseBuffer, 256, armPositionFeedback);
            // udp.beginPacket(remoteIP, PORT);
            // udp.write(responseBuffer, responseLength);
            // udp.endPacket();

            // bool isReversed = (armEffortRequest.wristSwivelEffort < 0); 
            // uint8_t requestedSpeed = armEffortRequest.wristSwivelEffort;
            // // Serial.printf("Requested speed: %d\n", requestedSpeed);
            // if (isReversed) {
            //   roboclaw.BackwardM2(ROBOCLAW_ADDR, requestedSpeed);
            // } else {
            //   roboclaw.ForwardM2(ROBOCLAW_ADDR, requestedSpeed);
            // }

            // bool isReversed = (requestMessage.rightSpeed < 0); 
            // uint8_t requestedSpeed = abs((requestMessage.rightSpeed / 3000.0) * 100.0);
            // Serial.printf("Requested speed: %d\n", requestedSpeed);
            // if (isReversed) {
            //   roboclaw.BackwardM2(ROBOCLAW_ADDR, requestedSpeed);
            // } else {
            //   roboclaw.ForwardM2(ROBOCLAW_ADDR, requestedSpeed);
            // }
            
            // roboclaw.ForwardM1(ROBOCLAW_ADDR,0); //start Motor1 forward at half speed
            // roboclaw.BackwardM2(ROBOCLAW_ADDR,64); //start Motor2 backward at half speed

            // if (roboclaw.ReadVersion(ROBOCLAW_ADDR, version)){
            //     Serial.println(version);
            // } else {
            //   Serial.println("Error!");
            // }
        }

        // if (stepperUpdateTimer >= STEPPER_UPDATE_RATE_MS) {
        //     stepperUpdateTimer -= STEPPER_UPDATE_RATE_MS;
             
            // if (armEffortRequest.has_clawVel) {
            //     run_stepper_2();
            // }
        // }

        // servo expects 50Hz signal, where HIGH time is between 500us and 2500us
        // 1500us means servo stopped
        // 1,000,000us / 50 = 20,000us period
        if (servoUpdateTimer >= servo_wait_us) {
            // reset timer
            servoUpdateTimer -= servo_wait_us;

            // high pulse has ended
            if (servo_pwm_high) {
                servo_wait_us = SERVO_PWM_PERIOD_US - abs(servo_wait_us);
                servo_pwm_high = false;
                digitalWrite(SERVO_PWM_PIN, LOW);
            } 
            // low pulse has ended
            // update high timer interval from protobuf
            else {
                servo_wait_us = abs(((1000 * static_cast<int>(armEffortRequest.clawVel)) / 600) + SERVO_STOPPED_PULSE_WIDTH);
                servo_pwm_high = true;
                digitalWrite(SERVO_PWM_PIN, HIGH);
            }   
        }
        
        if (actuatorUpdateTimer >= ACTUATOR_UPDATE_RATE) {
            actuatorUpdateTimer -= ACTUATOR_UPDATE_RATE;

            if (armEffortRequest.has_linearActuator) {
                // get the request for the linear actuator
                int act_idx = armEffortRequest.linearActuator;
                if (act_idx == 1) {
                    digitalWrite(LINEAR_ACTUATOR_RX, LOW);
                    digitalWrite(LINEAR_ACTUATOR_TX, HIGH);

                } else if (act_idx == -1) {
                    digitalWrite(LINEAR_ACTUATOR_RX, HIGH);
                    digitalWrite(LINEAR_ACTUATOR_TX, LOW);

                } else {
                    digitalWrite(LINEAR_ACTUATOR_RX, HIGH);
                    digitalWrite(LINEAR_ACTUATOR_TX, HIGH);
                }

            } else {
                digitalWrite(LINEAR_ACTUATOR_RX, HIGH);
                digitalWrite(LINEAR_ACTUATOR_TX, HIGH);
            }
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

void run_stepper_2() {
    // stepper_driver.disable();
    if (armEffortRequest.clawVel < 0) {
        stepper_driver.disableInverseMotorDirection();
    } else {
        stepper_driver.enableInverseMotorDirection();
    }
    
    int run_speed = abs(armEffortRequest.clawVel) * 50;

    if (run_speed >= 1000) {
        stepper_driver.moveAtVelocity(run_speed);
        stepper_driver.enable();
    }  else {
        stepper_driver.disable();
    }
}

void run_roboclaw_effort(int address, int channel, int effort) {
    

    int hash = address * 10 + channel;
    if (lastCommand.count(hash) > 0 && lastCommand[hash] == effort) return;
    lastCommand[hash] = effort;
    
    uint8_t addr = address;
    bool isReversed = (effort < 0); 
    uint8_t requestedSpeed = abs(effort);

    
    if (channel == 1) {
        if (isReversed) {
            roboclaw.BackwardM1(addr, requestedSpeed);
        } else {
            roboclaw.ForwardM1(addr, requestedSpeed);
        }
    } else if (channel == 2) {
        if (isReversed) {
            roboclaw.BackwardM2(addr, requestedSpeed);
        } else {
            roboclaw.ForwardM2(addr, requestedSpeed);
        }
    }
    
}

void run_roboclaw_speed(int address, int channel, int speed) {
    int hash = address * 10 + channel;
    if (lastCommand.count(hash) > 0 && lastCommand[hash] == speed) return;
    lastCommand[hash] = speed;

    uint8_t addr = address;

    if (channel == 1) {
        roboclaw.SpeedM1(addr, speed);
    } else if (channel == 2) {
        roboclaw.SpeedM2(addr, speed);
    }
}

