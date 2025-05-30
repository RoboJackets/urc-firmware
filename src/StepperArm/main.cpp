/*******************************************************************/
/***************************** LIBRARY *****************************/
/*******************************************************************/

#include <Arduino.h>
#include <FlexCAN_T4.h>         // CAN enabling tool
#include <TMC2209.h>
#include <QNEthernet.h>
#include <map>
#include <vector>
#include <RoboClaw.h>
#include <string>
#include <unordered_map>
#include "urc.pb.h"
#include "Messages.hpp"         // Protobuf decode functions (encodeResponse, decodeResponse)
#include "SoloCAN.hpp"          // SOLO UNO motor controller's library

#include "pb_encode.h"          // Protobuf encode functions
#include "pb_decode.h"          // Protobuf decode functions

/**********************************************************************/
/***************************** DEFINITION *****************************/
/**********************************************************************/

// States of data sent to SOLO UNO motor controllers
enum class CAN_Send_State {
    Motor_Setpoint,
    Motor_Speed,
    Motor_Current,
    Motor_Position
};

// Format of the feedback data from SOLO UNOs
struct Solo_Feedback_Data {
    uint32_t speedFeedback;
    float quadratureCurrent;
    uint32_t positionFeedback;
};

// CONTSTANTS
const int BLINK_RATE_MS = 500;              // blink timer setup
const int CAN_READ_RATE_MS = 30;
const int UDP_WRITE_RATE_MS = 50;
const int CAN_BAUD_RATE = 500000;
const int NUM_MOTORS = 5;
const int MOTOR_IDS[NUM_MOTORS] = {0xA5, 0xA6, 0xA7, 0xA8, 0xA9}; // need to match the IDs assigned via Motion Terminal
const int STEPPER_UPDATE_RATE_MS = 100;     // stepper timer setup
const int MOTOR_UPDATE_RATE = 100;          // motor timer setup
const int ACTUATOR_UPDATE_RATE = 100;       // linear actuator timer setup
const int32_t RUN_VELOCITY = 40000;         // stepper run speed
const int32_t STOP_VELOCITY = 0;            // stepper stop speed
const int RUN_DURATION = 5000;
const int STOP_DURATION = 1000;
const int RUN_COUNT = 4;        
const int PORT = 8443;                      // Ethernet port setup
// const int ROBOCLAW_ADDR = 0x81;

// CAN Setup
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_32> can;      // enabling CAN
std::map<int, int> armSetpoints;
CAN_Send_State sendState;

// INPUT RANGE: -600 to 600
// OUTPUT RANGE: 500us to 2500us, 1500 is stopped
// SERVO SETUP
elapsedMicros servoUpdateTimer;             // servo timer setup
const int SERVO_PWM_PIN = 19;               // servo pwm pin
const int SERVO_STOPPED_PULSE_WIDTH = 1500; // pwm pulse width
const int SERVO_PWM_PERIOD_US = 2000;      // pwm pulse period
bool servo_pwm_high = true;                 // pwm mode: HIGH
int servo_wait_us = SERVO_STOPPED_PULSE_WIDTH;

// LINEAR ACTUATOR SETUP
const int LINEAR_ACTUATOR_TX = 20;                  // linear actuator controller pin 1
const int LINEAR_ACTUATOR_RX = 21;                  // linear actuator cotroller pin 2

// constexpr int ROBOCLAW_SHOULDER_ADDR = 0x82;    // roboclaw controller shoulder address
// constexpr int ROBOCLAW_WRIST_ADDR = 0x81;       // roboclaw controller wrist address
// constexpr int ROBOCLAW_ELBOW_ADDR = 0x80;       // roboclaw controller elbow address

// constexpr int ROBOCLAW_CHANNEL_1 = 1;           // roboclaw channel 1
// constexpr int ROBOCLAW_CHANNEL_2 = 2;           // roboclaw channel 2

const long SERIAL_BAUD_RATE = 38400;            // baud rate for roboclaw & stepper
const uint8_t RUN_CURRENT_PERCENT = 100;        // stepper running current percent
const uint8_t HOLD_CURRENT_STANDSTILL = 0;      // stepper standstill hold current value


// VARIABLES
qindesign::network::EthernetUDP udp;                // udp initialization
TMC2209 stepper_driver;                             // stepper driver initialization
// DriveEncodersMessage requestMessage;                // arm encoder feedback message
ArmSpeedRequest armSpeedRequest;                  // arm speed message
// ArmPositionFeedback armPositionFeedback;            // arm position feedback message

// TIMER VARIABLES
elapsedMillis blinkTimer;
elapsedMillis stepperUpdateTimer;
elapsedMillis actuatorUpdateTimer;
elapsedMillis motorUpdateTimer;
// RoboClaw roboclaw(&Serial2, 38400);              // initialize roboclaw

std::vector<int>::iterator mySpeed;
std::vector<int> stepperSpeeds;
// hash code related
// std::unordered_map<int, int> lastCommand;

IPAddress remoteIP;

// FUNCTION DEFINITION
int clampDriveRequest(int speedRef);
void handleDriveRequest(DrivetrainRequest message);

void setup_test_stepper();
void test_stepper();
void test_stepper_2();
void run_stepper();
void run_stepper_2();
void run_roboclaw_effort(int address, int channel, int effort);
void run_roboclaw_speed(int address, int channel, int speed);

/*****************************************************************/
/***************************** SETUP *****************************/
/*****************************************************************/

int main() {
    // LED SETUP
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(2, OUTPUT);
    pinMode(SERVO_PWM_PIN, OUTPUT);
    digitalWrite(2, LOW);

    // ETHERNET SETUP
    constexpr char hostName[]{"Arm_Teensy"};
    qindesign::network::Ethernet.setHostname(hostName);
    qindesign::network::Ethernet.begin();               // begin ethernet
    udp.begin(PORT);                                    // open a port with udp

    // CAN Setup
    can.begin();
    can.setBaudRate(CAN_BAUD_RATE);
    solo_can::SoloCan<CAN1, RX_SIZE_256, TX_SIZE_32> solo(can);
    CAN_message_t canMsg;

    // Initializing protobuf messages
    armSpeedRequest = ArmSpeedRequest_init_zero;
    // armPositionFeedback = ArmPositionFeedback_init_zero;
    // requestMessage = DriveEncodersMessage_init_zero;

    // BUFFER SETUP
    uint8_t requestBuffer[256];
    size_t requestLength;
    uint8_t responseBuffer[256];
    size_t responseLength;

    // ROBOCLAW SETUP
    // roboclaw.begin(38400);

    // STEPPER SETUP
    Serial1.begin(SERIAL_BAUD_RATE);
    stepper_driver.setup(Serial1);
    stepper_driver.setRunCurrent(RUN_CURRENT_PERCENT);
    stepper_driver.setHoldCurrent(HOLD_CURRENT_STANDSTILL);
    stepper_driver.enable();
    stepper_driver.moveAtVelocity(STOP_VELOCITY);
    delay(STOP_DURATION);
    stepper_driver.moveAtVelocity(RUN_VELOCITY);
    // TEST Stepper
    setup_test_stepper();

/****************************************************************/
/***************************** LOOP *****************************/
/****************************************************************/

    while (true) {

        // UDP DECODING
        requestLength = udp.parsePacket();      // read incoming UDP messages
        if (udp.available()) { 

            Serial.print("Packet received: ");
            //Serial.println("packet");

            // protobuf decode the packet, saved to armSpeedRequest
            memset(requestBuffer, 0, 256);                  // prepare the buffer
            udp.readBytes(requestBuffer, requestLength);    // read udp packet to the buffer
            protobuf::Messages::decodeRequest(requestBuffer, requestLength, armSpeedRequest); // decode the message from the buffer

            remoteIP = udp.remoteIP();
            
            armSetpoints[MOTOR_IDS[0]] = clampDriveRequest(armSpeedRequest.wristLeftSpeed);
            armSetpoints[MOTOR_IDS[1]] = clampDriveRequest(armSpeedRequest.wristRightSpeed);
            armSetpoints[MOTOR_IDS[2]] = clampDriveRequest(armSpeedRequest.shoulderLiftSpeed);
            armSetpoints[MOTOR_IDS[3]] = clampDriveRequest(armSpeedRequest.elbowLiftSpeed);
            armSetpoints[MOTOR_IDS[4]] = clampDriveRequest(armSpeedRequest.shoulderSwivelSpeed);
        }

        // write CAN commands
        // solo.processMessageQueue();
        can.events();

        // MOTOR CONTROL
        if (motorUpdateTimer >= MOTOR_UPDATE_RATE) {
            motorUpdateTimer -= MOTOR_UPDATE_RATE;

            // // debugging code
            // solo.SetSpeedReferenceCommand(167, 800, false);

            solo.SetSpeedReferenceCommand(MOTOR_IDS[0], armSetpoints[MOTOR_IDS[0]], false); 
            solo.SetSpeedReferenceCommand(MOTOR_IDS[1], armSetpoints[MOTOR_IDS[1]], false); 
            solo.SetSpeedReferenceCommand(MOTOR_IDS[2], armSetpoints[MOTOR_IDS[2]], false); 
            solo.SetSpeedReferenceCommand(MOTOR_IDS[3], armSetpoints[MOTOR_IDS[3]], false); 
            solo.SetSpeedReferenceCommand(MOTOR_IDS[4], armSetpoints[MOTOR_IDS[4]], false); 

            /*||||||||||||||||| ROBOCLAW CODE ||||||||||||||||||*/

            // Roboclaw effort drive
            // run_roboclaw_effort(ROBOCLAW_ELBOW_ADDR, ROBOCLAW_CHANNEL_1, armSpeedRequest.elbowLiftEffort);
            // run_roboclaw_effort(ROBOCLAW_ELBOW_ADDR, ROBOCLAW_CHANNEL_2, armSpeedRequest.shoulderLiftEffort);
            // run_roboclaw_effort(ROBOCLAW_WRIST_ADDR, ROBOCLAW_CHANNEL_1, armSpeedRequest.wristSwivelEffort);
            // run_roboclaw_effort(ROBOCLAW_WRIST_ADDR, ROBOCLAW_CHANNEL_2, armSpeedRequest.wristLiftEffort);
            // run_roboclaw_effort(ROBOCLAW_SHOULDER_ADDR, ROBOCLAW_CHANNEL_1, armSpeedRequest.shoulderSwivelEffort);

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

            // if (roboclaw.ReadVersion(ROBOCLAW_ADDR, version)){
            //     Serial.println(version);
            // } else {
            //   Serial.println("Error!");
            // }

            /*||||||||||||||||| ROBOCLAW CODE ENDS ||||||||||||||||||*/

        }

        // STEPPER CONTROL
        if (stepperUpdateTimer >= STEPPER_UPDATE_RATE_MS) {
            stepperUpdateTimer -= STEPPER_UPDATE_RATE_MS;
             
            if (armSpeedRequest.has_clawVel) {
                run_stepper_2();
            }
        }

        // SERVO CONTROL
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
                servo_wait_us = abs(((1000 * static_cast<int>(armSpeedRequest.clawVel)) / 600) + SERVO_STOPPED_PULSE_WIDTH);
                servo_pwm_high = true;
                digitalWrite(SERVO_PWM_PIN, HIGH);
            }   
        }
        
        // LINEAR ACTUATOR CONTROL
        if (actuatorUpdateTimer >= ACTUATOR_UPDATE_RATE) {
            actuatorUpdateTimer -= ACTUATOR_UPDATE_RATE;

            if (armSpeedRequest.has_linearActuator) {
                // get the request for the linear actuator
                int act_idx = armSpeedRequest.linearActuator;
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

        // LED BLINKING
        if (blinkTimer >= BLINK_RATE_MS) {
            blinkTimer -= BLINK_RATE_MS;
            digitalToggle(LED_BUILTIN);
        }
    }
}

/*********************************************************************/
/***************************** FUNCTIONS *****************************/
/*********************************************************************/

// Stepper setup; the stepper should return to the original position after turning
void setup_test_stepper() {
    // stepperSpeeds.push_back(0);
    // stepperSpeeds.push_back(-1000);
    // stepperSpeeds.push_back(0);
    // stepperSpeeds.push_back(-4000);
    // stepperSpeeds.push_back(0);
    // stepperSpeeds.push_back(-8000);
    // stepperSpeeds.push_back(1000);
    // stepperSpeeds.push_back(0);
    // stepperSpeeds.push_back(4000);
    // stepperSpeeds.push_back(8000);

    mySpeed = stepperSpeeds.begin();
}

// The real stepper test function
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

// the real run stepper function. Need to be improved with input vairables
void run_stepper_2() {
    // stepper_driver.disable();
    if (armSpeedRequest.clawVel < 0) {
        stepper_driver.disableInverseMotorDirection();
    } else {
        stepper_driver.enableInverseMotorDirection();
    }
    
    int run_speed = abs(armSpeedRequest.clawVel) * 50;

    if (run_speed >= 1000) {
        stepper_driver.moveAtVelocity(run_speed);
        stepper_driver.enable();
    }  else {
        stepper_driver.disable();
    }
}

// Handles the input speed and adjust it to the right range
int clampDriveRequest(int speedRef) {
    if (speedRef > 4000) 
        return 4000;
    else if (speedRef < -4000) 
        return -4000;
    else
        return speedRef; 
}

// Reads the Ethernet message and set the motor points in the array
// void handleDriveRequest(DrivetrainRequest message) {
//     armSetpoints[MOTOR_IDS[0]] = clampDriveRequest(message.m1Setpoint);
//     armSetpoints[MOTOR_IDS[1]] = clampDriveRequest(message.m2Setpoint);
//     armSetpoints[MOTOR_IDS[2]] = clampDriveRequest(message.m3Setpoint);
//     armSetpoints[MOTOR_IDS[3]] = clampDriveRequest(message.m4Setpoint);
// }

// run roboclaw controlled motors at given torque
// void run_roboclaw_effort(int address, int channel, int effort) {
    
//     // hash function to avoid repeated commands
//     // int hash = address * 10 + channel;
//     // if (lastCommand.count(hash) > 0 && lastCommand[hash] == effort) return;
//     // lastCommand[hash] = effort;
    
//     uint8_t addr = address;
//     bool isReversed = (effort < 0); 
//     uint8_t requestedSpeed = abs(effort);

//     if (channel == 1) {
//         if (isReversed) {
//             roboclaw.BackwardM1(addr, requestedSpeed);
//         } else {
//             roboclaw.ForwardM1(addr, requestedSpeed);
//         }
//     } else if (channel == 2) {
//         if (isReversed) {
//             roboclaw.BackwardM2(addr, requestedSpeed);
//         } else {
//             roboclaw.ForwardM2(addr, requestedSpeed);
//         }
//     }
// }

// run roboclaw controlled motors at the given speed
// void run_roboclaw_speed(int address, int channel, int speed) {

//     // hash function to avoid repeated commands
//     int hash = address * 10 + channel;
//     if (lastCommand.count(hash) > 0 && lastCommand[hash] == speed) return;
//     lastCommand[hash] = speed;

//     uint8_t addr = address;

//     if (channel == 1) {
//         roboclaw.SpeedM1(addr, speed);
//     } else if (channel == 2) {
//         roboclaw.SpeedM2(addr, speed);
//     }
// }

// run the stepper with the give speed. Need to be improved with input vairables
// void run_stepper() {
//     // stepper_driver.disable();
//     if (requestMessage.leftSpeed < 0) {
//         stepper_driver.disableInverseMotorDirection();
//     } else {
//         stepper_driver.enableInverseMotorDirection();
//     }
    
//     int run_speed = abs(requestMessage.leftSpeed) * 50;

//     if (run_speed >= 1000) {
//         stepper_driver.moveAtVelocity(run_speed);
//         stepper_driver.enable();
//     }  else {
//         stepper_driver.disable();
//     }
// }

// Stepper test
// void test_stepper() {
//     stepper_driver.moveAtVelocity(RUN_VELOCITY);
//     stepper_driver.disableInverseMotorDirection();
//     stepper_driver.enable();
//     delay(RUN_DURATION);
//     stepper_driver.disable();
//     delay(STOP_DURATION);
//     stepper_driver.enableInverseMotorDirection();
//     stepper_driver.enable();
//     delay(RUN_DURATION);
//     stepper_driver.disable();
//     delay(STOP_DURATION);
// }
