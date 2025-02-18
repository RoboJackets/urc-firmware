#include <Arduino.h>
#include <FlexCAN_T4.h>
#include <QNEthernet.h>
#include <map>
#include "urc.pb.h"
#include "Messages.hpp"
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
    Motor_Speed,
    Motor_Current,
    Motor_Position
};

struct Solo_Feedback_Data {
    uint32_t speedFeedback;
    float quadratureCurrent;
    uint32_t positionFeedback;
};

struct Status_Light_Data {
    uint8_t enabled;
    uint8_t blink;
};


// protoboard pins
const int GREEN_PIN = 34;
const int BLUE_PIN = 32;
const int RED_PIN = 35;

const int BLINK_RATE_MS = 500;
const int CAN_READ_RATE_MS = 30;
const int UDP_WRITE_RATE_MS = 50;
const int BAUD_RATE = 500000;
const int NUM_MOTORS = 4;
const int MOTOR_IDS[NUM_MOTORS] = {0xA1, 0xA2, 0xA3, 0xA4};
const int PORT = 8443;

// change to your local device
// const uint8_t CLIENT_IP[] = { 192, 168, 1, 61 };


// variables
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_32> can;
qindesign::network::EthernetUDP udp;
std::map<int, int> motorSetpoints;
std::map<int, Solo_Feedback_Data> encoderData;
std::map<int, Status_Light_Data> statusLightData;
CAN_Send_State sendState;

// timer variable
elapsedMillis blinkTimer;
elapsedMillis currentTime;
elapsedMillis canReadTimer;
elapsedMillis udpWriteTimer;
int ledPin = -1;
bool blinkEnabled = false;
bool sentSpeed = false;

int clampDriveRequest(int speedRef);
void handleLEDRequest(NewStatusLightCommand message);
void handleDriveRequest(DrivetrainRequest message);

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

    Status_Light_Data stat;
    statusLightData[GREEN_PIN] = stat;
    statusLightData[BLUE_PIN] = stat;
    statusLightData[RED_PIN] = stat;

    int leftSpeed = 0;
    int rightSpeed = 0;

    while (true) {

        // receive UDP packets
        requestLength = udp.parsePacket();
        if (udp.available()) { 

            // new protobuf
            TeensyMessage message;
            memset(requestBuffer, 0, 256);
            udp.readBytes(requestBuffer, requestLength);
            pb_istream_t istream = pb_istream_from_buffer(requestBuffer, requestLength);

            Serial.println(requestLength);
            pb_decode(&istream, TeensyMessage_fields, &message);

            Serial.print("Message type: ");
            Serial.println(message.which_messageType);
            Serial.print("Left setpoint: ");
            Serial.println(message.messageType.setpointMessage.leftSetpoint);
            Serial.print("Right setpoint: ");
            Serial.println(message.messageType.setpointMessage.rightSetpoint);
            Serial.print("Status Light data: ");
            Serial.println(message.messageType.statusLightMessage.lightCommand);
            // status light
            if (message.which_messageType == 1) {
                // handleLEDRequest(message.payload.statusLightCommand);
                // handleLEDRequest(message.statusLightCommand);

                // if (message.redEnabled == 0) {
                //     digitalWrite(RED_PIN, LOW);
                // } else {
                //     digitalWrite(RED_PIN, HIGH);
                // }

                // if (message.blueEnabled == 0) {
                //     digitalWrite(BLUE_PIN, LOW);
                // } else {
                //     digitalWrite(BLUE_PIN, HIGH);
                // }

                // if (message.greenEnabled == 0) {
                //     digitalWrite(GREEN_PIN, LOW);
                // } else {
                //     digitalWrite(GREEN_PIN, HIGH);
                // }

                statusLightData[RED_PIN].enabled = ((message.messageType.statusLightMessage.lightCommand & 0x0FFF) >> 0) & 0x1;
                statusLightData[RED_PIN].blink = ((message.messageType.statusLightMessage.lightCommand & 0x0FFF) >> 0) & 0x2;
                statusLightData[GREEN_PIN].enabled = ((message.messageType.statusLightMessage.lightCommand & 0x0FFF) >> 0) & 0x3;
                statusLightData[GREEN_PIN].blink = ((message.messageType.statusLightMessage.lightCommand & 0x0FFF) >> 0) & 0x4;
                statusLightData[BLUE_PIN].enabled = ((message.messageType.statusLightMessage.lightCommand & 0x0FFF) >> 0) & 0x5;
                statusLightData[BLUE_PIN].blink = ((message.messageType.statusLightMessage.lightCommand & 0x0FFF) >> 0) & 0x6;


                // Serial.println("Status light");
            } 
            // drivetrain
            else if (message.which_messageType == 2) {
                // handleDriveRequest(message.payload.driveEncodersMessage);
                // handleDriveRequest(message.driveEncodersMessage);

                motorSetpoints[MOTOR_IDS[0]] = clampDriveRequest(message.messageType.setpointMessage.leftSetpoint);
                motorSetpoints[MOTOR_IDS[1]] = clampDriveRequest(message.messageType.setpointMessage.leftSetpoint);
                motorSetpoints[MOTOR_IDS[2]] = clampDriveRequest(message.messageType.setpointMessage.rightSetpoint);
                motorSetpoints[MOTOR_IDS[3]] = clampDriveRequest(message.messageType.setpointMessage.rightSetpoint);
                // Serial.println("Drivetrain");
            }
            // invalid message ID
            else {
                Serial.println("Invalid message ID");
            }

            // if (pb_decode(&istream, TeensyMessage_fields, &message)) {
            //     // status light
            //     if (message.messageID == 1) {
            //         // handleLEDRequest(message.payload.statusLightCommand);
            //         // handleLEDRequest(message.statusLightCommand);
            //         Serial.println("Status light");
            //     } 
            //     // drivetrain
            //     else if (message.messageID == 0) {
            //         // handleDriveRequest(message.payload.driveEncodersMessage);
            //         // handleDriveRequest(message.driveEncodersMessage);
            //         Serial.println("Drivetrain");
            //     }
            //     // invalid message ID
            //     else {
            //         Serial.println("Invalid message ID");
            //     }
            // } else {
            //     Serial.println("Decoding failed!");
            //     Serial.println(PB_GET_ERROR(&istream));
            // }


            // // currently working protobuf
            // memset(requestBuffer, 0, 256);
            // udp.readBytes(requestBuffer, requestLength);
            // pb_istream_t istream = pb_istream_from_buffer(requestBuffer+1, requestLength-1);

            // // status light request
            // if (requestBuffer[0] == 0x10) {
            //     NewStatusLightCommand message;
            //     if (pb_decode(&istream, NewStatusLightCommand_fields, &message)) {
            //         Serial.println("Status Light request");
            //         handleLEDRequest(message);
            //     } else {
            //         Serial.println("Decoding status light failed!");
            //     }
            // } 
            // // drive encoders request
            // else if (requestBuffer[0] == 0x11) {

            //     DrivetrainRequest message;

            //     if (pb_decode(&istream, DrivetrainRequest_fields, &message)) {
            //         motorSetpoints[MOTOR_IDS[0]] = message.m1Setpoint;
            //         motorSetpoints[MOTOR_IDS[1]] = message.m2Setpoint;
            //         motorSetpoints[MOTOR_IDS[2]] = message.m3Setpoint;
            //         motorSetpoints[MOTOR_IDS[3]] = message.m4Setpoint;
            //         motorSetpoints[MOTOR_IDS[4]] = message.m5Setpoint;
            //         motorSetpoints[MOTOR_IDS[5]] = message.m6Setpoint;
            //     } else {
            //         Serial.println("Decoding drive request failed!");
            //     }
            // } else {
            //     Serial.println("Command code not recognized!");
            // }
        }

        // write UDP message at regular interval
        if (udpWriteTimer >= UDP_WRITE_RATE_MS) {
            udpWriteTimer -= UDP_WRITE_RATE_MS;

            DrivetrainResponse responseMessage;
            uint8_t responseBuffer[256];

            responseMessage.m1SpeedFeedback = encoderData[MOTOR_IDS[0]].speedFeedback;
            responseMessage.m2SpeedFeedback = encoderData[MOTOR_IDS[1]].speedFeedback;
            responseMessage.m3SpeedFeedback = encoderData[MOTOR_IDS[2]].speedFeedback;
            responseMessage.m4SpeedFeedback = encoderData[MOTOR_IDS[3]].speedFeedback;

            responseMessage.m1Current = encoderData[MOTOR_IDS[0]].quadratureCurrent;
            responseMessage.m2Current = encoderData[MOTOR_IDS[1]].quadratureCurrent;
            responseMessage.m3Current = encoderData[MOTOR_IDS[2]].quadratureCurrent;
            responseMessage.m4Current = encoderData[MOTOR_IDS[3]].quadratureCurrent;

            responseMessage.m1PositionFeedback = encoderData[MOTOR_IDS[0]].positionFeedback;
            responseMessage.m2PositionFeedback = encoderData[MOTOR_IDS[1]].positionFeedback;
            responseMessage.m3PositionFeedback = encoderData[MOTOR_IDS[2]].positionFeedback;
            responseMessage.m4PositionFeedback = encoderData[MOTOR_IDS[3]].positionFeedback;

            pb_ostream_t ostream = pb_ostream_from_buffer(responseBuffer, sizeof(responseBuffer));
            pb_encode(&ostream, DrivetrainResponse_fields, &responseMessage);

            udp.beginPacket(udp.remoteIP(), PORT);
            udp.write(responseBuffer, ostream.bytes_written);
            udp.endPacket();
        }

        // read CAN responses
        while (can.read(canMsg)) {
            canResponseMessage = solo_can::parseMessage(canMsg);

            // record speed reference command
            if (canResponseMessage.type == solo_can::SDO_READ_RESPONSE && canResponseMessage.code == solo_can::SPEED_FEEDBACK_CODE) {
                int id = canResponseMessage.id - 0x580;

                if (id == 0xA1 || id == 0xA2 || id == 0xA3) {
                    encoderData[id].speedFeedback = -1 * canResponseMessage.payload;
                } else if (id == 0xA4 || id == 0xA5 || id == 0xA6) {
                    encoderData[id].speedFeedback = canResponseMessage.payload;
                }
                
            } else if (canResponseMessage.type == solo_can::SDO_READ_RESPONSE && canResponseMessage.code == solo_can::QUADRATURE_CURRENT_FEEDBACK_CODE) {
                encoderData[canResponseMessage.id - 0x580].quadratureCurrent = canResponseMessage.payload;
            } else if (canResponseMessage.type == solo_can::SDO_READ_RESPONSE && canResponseMessage.code == solo_can::POSITION_FEEDBACK_CODE) {
                int id = canResponseMessage.id - 0x580;

                if (id == 0xA1 || id == 0xA2 || id == 0xA3) {
                    encoderData[id].positionFeedback = -1 * canResponseMessage.payload;
                } else if (id == 0xA4 || id == 0xA5 || id == 0xA6) {
                    encoderData[id].positionFeedback = canResponseMessage.payload;
                }
            }
        }

        // write CAN commands
        // solo.processMessageQueue();
        can.events();

        // send CAN commands
        if (canReadTimer >= CAN_READ_RATE_MS) {
            if (sendState == CAN_Send_State::Motor_Setpoint) {
                // write speed commands
                for (int i = 0; i < 2; i++) {
                    solo.SetSpeedReferenceCommand(MOTOR_IDS[i], motorSetpoints[MOTOR_IDS[i]], false);
                }

                for (int i = 2; i < 4; i++) {
                    solo.SetSpeedReferenceCommand(MOTOR_IDS[i], motorSetpoints[MOTOR_IDS[i]], true);
                }
                canReadTimer -= CAN_READ_RATE_MS;
                sendState = CAN_Send_State::Motor_Speed;
            } else if (sendState == CAN_Send_State::Motor_Speed) {
                solo.GetSpeedFeedbackCommand(161);
                solo.GetSpeedFeedbackCommand(162);
                solo.GetSpeedFeedbackCommand(163);
                solo.GetSpeedFeedbackCommand(164);
            
                canReadTimer -= CAN_READ_RATE_MS;
                sendState = CAN_Send_State::Motor_Current;
            } else if (sendState == CAN_Send_State::Motor_Current) {

                solo.GetCurrentDrawCommand(161);
                solo.GetCurrentDrawCommand(162);
                solo.GetCurrentDrawCommand(163);
                solo.GetCurrentDrawCommand(164);
                
                canReadTimer -= CAN_READ_RATE_MS; // ??
                sendState = CAN_Send_State::Motor_Position;
            } else if (sendState == CAN_Send_State::Motor_Position) {
                solo.GetPositionFeedbackCommand(161);
                solo.GetPositionFeedbackCommand(162);
                solo.GetPositionFeedbackCommand(163);
                solo.GetPositionFeedbackCommand(164);

                canReadTimer -= CAN_READ_RATE_MS; // ??
                sendState = CAN_Send_State::Motor_Setpoint;
            }
        }

        // blink onboard LED
        if (blinkTimer >= BLINK_RATE_MS) {
            blinkTimer -= BLINK_RATE_MS;

            if (statusLightData[GREEN_PIN].blink) {
                digitalWrite(GREEN_PIN, blinkEnabled);
            } else {
                digitalWrite(GREEN_PIN, statusLightData[GREEN_PIN].enabled);
            }

            if (statusLightData[RED_PIN].blink) {
                digitalWrite(RED_PIN, blinkEnabled);
            } else {
                digitalWrite(RED_PIN, statusLightData[RED_PIN].enabled);
            }

            if (statusLightData[BLUE_PIN].blink) {
                digitalWrite(BLUE_PIN, blinkEnabled);
            } else {
                digitalWrite(BLUE_PIN, statusLightData[BLUE_PIN].enabled);
            }

            blinkEnabled = !blinkEnabled;

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

int clampDriveRequest(int speedRef) {
    if (speedRef > 4000) 
        return 4000;
    else if (speedRef < -4000) 
        return -4000;
    else
        return speedRef; 
}

void handleDriveRequest(DrivetrainRequest message) {
    motorSetpoints[MOTOR_IDS[0]] = clampDriveRequest(message.m1Setpoint);
    motorSetpoints[MOTOR_IDS[1]] = clampDriveRequest(message.m2Setpoint);
    motorSetpoints[MOTOR_IDS[2]] = clampDriveRequest(message.m3Setpoint);
    motorSetpoints[MOTOR_IDS[3]] = clampDriveRequest(message.m4Setpoint);
}
