#include <Arduino.h>
#include <FlexCAN_T4.h>
#include <QNEthernet.h>
#include <map>
#include "urc.pb.h"
#include "Messages.hpp"
#include "StatusLight.hpp"
#include "SoloCAN.hpp"

#include "pb_encode.h"
#include "pb_decode.h"
#include "urc.pb.h"

// v1.0 PCB pins
// const int GREEN_PIN = 30;
// const int BLUE_PIN = 31;
// const int RED_PIN = 32;

enum class CAN_Send_State {
    Motor_Setpoint,
    Motor_Feedback,
    Motor_Current
};

struct Solo_Feedback_Data {
    uint32_t speedFeedback;
    float quadratureCurrent;
};


// protoboard pins
const int GREEN_PIN = 34;
const int BLUE_PIN = 32;
const int RED_PIN = 35;

const int BLINK_RATE_MS = 500;
const int CAN_READ_RATE_MS = 200;
const int UDP_WRITE_RATE_MS = 50;
const int BAUD_RATE = 500000;
const int NUM_MOTORS = 6;
const int MOTOR_IDS[NUM_MOTORS] = {0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6};
const int PORT = 8443;
const uint8_t CLIENT_IP[] = { 192, 168, 1, 228 };


// variables
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_32> can;
qindesign::network::EthernetUDP udp;
std::map<int, uint32_t> motorSetpoints;
// std::map<int, uint32_t> encoderData;
std::map<int, Solo_Feedback_Data> encoderData;
CAN_Send_State sendState;

// timer variable
elapsedMillis blinkTimer;
elapsedMillis currentTime;
elapsedMillis canReadTimer;
elapsedMillis udpWriteTimer;
int ledPin = -1;
bool blinkEnabled = false;
bool sentSpeed = false;


void handleLEDRequest(NewStatusLightCommand message);
void handleDriveRequest(DriveEncodersMessage message);

int main() {
    // LED setup
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);
    pinMode(RED_PIN, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);

    // Ethernet setup
    qindesign::network::Ethernet.begin();
    udp.begin(PORT);

    // CAN setup
    can.begin();
    can.setBaudRate(BAUD_RATE);
    solo_can::SoloCan<CAN1, RX_SIZE_256, TX_SIZE_32> solo(can);
    sendState = CAN_Send_State::Motor_Setpoint;

    // initialize data
    CAN_message_t canMsg;
    solo_can::CanOpenData canResponseMessage; 
    size_t requestLength;
    uint8_t requestBuffer[256];

    int leftSpeed = 0;
    int rightSpeed = 0;

    while (true) {

        // receive UDP packets
        requestLength = udp.parsePacket();
        if (udp.available()) { 

            // Serial.println("Packet received: ");

            memset(requestBuffer, 0, 256);
            udp.readBytes(requestBuffer, requestLength);
            pb_istream_t istream = pb_istream_from_buffer(requestBuffer+1, requestLength-1);

            // status light request
            if (requestBuffer[0] == 0x10) {
                NewStatusLightCommand message;
                if (pb_decode(&istream, NewStatusLightCommand_fields, &message)) {
                    Serial.println("Status Light request");
                    handleLEDRequest(message);
                } else {
                    Serial.println("Decoding status light failed!");
                }
            } 
            // drive encoders request
            else if (requestBuffer[0] == 0x11) {

                DrivetrainRequest message;

                if (pb_decode(&istream, DrivetrainRequest_fields, &message)) {
                    motorSetpoints[MOTOR_IDS[0]] = message.m1Setpoint;
                    motorSetpoints[MOTOR_IDS[1]] = message.m2Setpoint;
                    motorSetpoints[MOTOR_IDS[2]] = message.m3Setpoint;
                    motorSetpoints[MOTOR_IDS[3]] = message.m4Setpoint;
                    motorSetpoints[MOTOR_IDS[4]] = message.m5Setpoint;
                    motorSetpoints[MOTOR_IDS[5]] = message.m6Setpoint;
                } else {
                    Serial.println("Decoding drive request failed!");
                }

                // DriveEncodersMessage message;
                // if (pb_decode(&istream, DriveEncodersMessage_fields, &message)) {
                //     leftSpeed = message.leftSpeed;
                //     rightSpeed = message.rightSpeed;
                //     Serial.print("Drive request: ");
                //     Serial.print(leftSpeed);
                //     Serial.print(", ");
                //     Serial.print(rightSpeed);
                //     Serial.print("\n");
                // } else {
                //     Serial.println("Decoding drive request failed!");
                // }
            } else {
                Serial.println("Command code not recognized!");
            }
            

            // Serial.print("[color=");
            // Serial.print(requestCommand.color);
            // Serial.print(", display=");
            // Serial.print(requestCommand.display);
            // Serial.println("]");


            // // OLD
            // handleLEDRequest(requestCommand.color, requestCommand.display);  
            
            // NEW
            // statusLight.resetLeds();
            // statusLight.setLedState("GREEN", HIGH);
            // statusLight.setLedBlink("GREEN", requestCommand.display);
        }

        // write UDP message at regular interval
        if (udpWriteTimer >= UDP_WRITE_RATE_MS) {
            udpWriteTimer -= UDP_WRITE_RATE_MS;

            DrivetrainResponse responseMessage;
            uint8_t responseBuffer[256];

            responseMessage.m1Feedback = encoderData[MOTOR_IDS[0]].speedFeedback;
            responseMessage.m2Feedback = encoderData[MOTOR_IDS[1]].speedFeedback;
            responseMessage.m3Feedback = encoderData[MOTOR_IDS[2]].speedFeedback;
            responseMessage.m4Feedback = encoderData[MOTOR_IDS[3]].speedFeedback;
            responseMessage.m5Feedback = encoderData[MOTOR_IDS[4]].speedFeedback;
            responseMessage.m6Feedback = encoderData[MOTOR_IDS[5]].speedFeedback;

            responseMessage.m1Current = encoderData[MOTOR_IDS[0]].quadratureCurrent;
            responseMessage.m2Current = encoderData[MOTOR_IDS[1]].quadratureCurrent;
            responseMessage.m3Current = encoderData[MOTOR_IDS[2]].quadratureCurrent;
            responseMessage.m4Current = encoderData[MOTOR_IDS[3]].quadratureCurrent;
            responseMessage.m5Current = encoderData[MOTOR_IDS[4]].quadratureCurrent;
            responseMessage.m6Current = encoderData[MOTOR_IDS[5]].quadratureCurrent;

            pb_ostream_t ostream = pb_ostream_from_buffer(responseBuffer, sizeof(responseBuffer));
            pb_encode(&ostream, DrivetrainResponse_fields, &responseMessage);

            udp.beginPacket(CLIENT_IP, PORT);
            udp.write(responseBuffer, ostream.bytes_written);
            udp.endPacket();
        }

        // read CAN commands
        while (can.read(canMsg)) {
            canResponseMessage = solo_can::parseMessage(canMsg);

            // record speed reference command
            if (canResponseMessage.type == solo_can::SDO_READ_RESPONSE && canResponseMessage.code == solo_can::SPEED_FEEDBACK_CODE) {
                encoderData[canResponseMessage.id - 0x580].speedFeedback = canResponseMessage.payload;
            } else if (canResponseMessage.type == solo_can::SDO_READ_RESPONSE && canResponseMessage.code == solo_can::QUADRATURE_CURRENT_FEEDBACK_CODE) {
                encoderData[canResponseMessage.id - 0x580].quadratureCurrent = canResponseMessage.payload;
            }
        }

        // write CAN commands
        // solo.processMessageQueue();
        can.events();

        // send CAN commands
        if (canReadTimer >= CAN_READ_RATE_MS) {

            if (sendState == CAN_Send_State::Motor_Setpoint) {
                
                // write speed commands
                for (int i = 0; i < 3; i++) {
                    solo.SetSpeedReferenceCommand(MOTOR_IDS[i], motorSetpoints[MOTOR_IDS[i]], false);
                }

                for (int i = 3; i < 6; i++) {
                    solo.SetSpeedReferenceCommand(MOTOR_IDS[i], motorSetpoints[MOTOR_IDS[i]], true);
                }

                canReadTimer -= 10;
                sendState = CAN_Send_State::Motor_Feedback;
            } else if (sendState == CAN_Send_State::Motor_Feedback) {
                solo.GetSpeedFeedbackCommand(161);
                solo.GetSpeedFeedbackCommand(162);
                solo.GetSpeedFeedbackCommand(163);
                solo.GetSpeedFeedbackCommand(164);
                solo.GetSpeedFeedbackCommand(165);
                solo.GetSpeedFeedbackCommand(166);
            
                canReadTimer -= 10;
                sendState = CAN_Send_State::Motor_Current;
            } else if (sendState == CAN_Send_State::Motor_Current) {

                solo.GetCurrentDrawCommand(161);
                solo.GetCurrentDrawCommand(162);
                solo.GetCurrentDrawCommand(163);
                solo.GetCurrentDrawCommand(164);
                solo.GetCurrentDrawCommand(165);
                solo.GetCurrentDrawCommand(166);
                
                canReadTimer -= CAN_READ_RATE_MS;
                sendState = CAN_Send_State::Motor_Setpoint;
            }
        }

        // blink onboard LED
        if (blinkTimer >= BLINK_RATE_MS) {
            blinkTimer -= BLINK_RATE_MS;
            digitalToggle(LED_BUILTIN);
        }
    }
}

void handleLEDRequest(NewStatusLightCommand message) {
    if (message.redEnabled == 0) {
        digitalWrite(RED_PIN, LOW);
    } else {
        digitalWrite(RED_PIN, HIGH);
    }

    if (message.blueEnabled == 0) {
        digitalWrite(BLUE_PIN, LOW);
    } else {
        digitalWrite(BLUE_PIN, HIGH);
    }

    if (message.greenEnabled == 0) {
        digitalWrite(GREEN_PIN, LOW);
    } else {
        digitalWrite(GREEN_PIN, HIGH);
    }
}

void handleDriveRequest(DriveEncodersMessage message) {

}
