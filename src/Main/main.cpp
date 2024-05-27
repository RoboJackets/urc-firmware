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
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can;
qindesign::network::EthernetUDP udp;
std::map<int, uint32_t> encoderData;

// timer variable
elapsedMillis blinkTimer;
elapsedMillis currentTime;
elapsedMillis canReadTimer;
elapsedMillis udpWriteTimer;
int ledPin = -1;
bool blinkEnabled = false;

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
    solo_can::SoloCan<CAN1, RX_SIZE_256, TX_SIZE_16> solo(can);

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
                DriveEncodersMessage message;
                if (pb_decode(&istream, DriveEncodersMessage_fields, &message)) {
                    leftSpeed = message.leftSpeed;
                    rightSpeed = message.rightSpeed;
                    Serial.print("Drive request: ");
                    Serial.print(leftSpeed);
                    Serial.print(", ");
                    Serial.print(rightSpeed);
                    Serial.print("\n");
                } else {
                    Serial.println("Decoding drive request failed!");
                }
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

            // populate responseMessage
            DriveEncodersMessage responseMessage;
            responseMessage.timestamp = currentTime;
            uint8_t responseBuffer[256];
            
            // int avgSpeedLeft = 0;
            // for (int i = 0; i < 3; i++) {
            //     avgSpeedLeft += encoderData[MOTOR_IDS[i]];
            // }
            // avgSpeedLeft /= 3;
            responseMessage.leftSpeed = encoderData[MOTOR_IDS[1]];

            // int avgSpeedRight = 0;
            // for (int i = 3; i < 6; i++) {
            //     avgSpeedRight += encoderData[MOTOR_IDS[i]];
            // }
            // avgSpeedRight /= 3;
            responseMessage.rightSpeed = encoderData[MOTOR_IDS[4]];

            size_t responseLength = protobuf::Messages::encodeResponse(responseBuffer, sizeof(responseBuffer), responseMessage);
            udp.beginPacket(CLIENT_IP, PORT);
            udp.write(responseBuffer, responseLength);
            udp.endPacket();

            // Serial.println("Packet sent");
        }

        // read CAN commands
        if (can.read(canMsg)) {
            canResponseMessage = solo_can::parseMessage(canMsg);

            // record speed reference command
            if (canResponseMessage.type == solo_can::SDO_READ_RESPONSE && canResponseMessage.code == solo_can::SPEED_FEEDBACK_CODE) {
                encoderData[canResponseMessage.id - 0x580] = canResponseMessage.payload;
            }
        }

        // send CAN commands
        if (canReadTimer >= CAN_READ_RATE_MS) {
            canReadTimer -= CAN_READ_RATE_MS;

            // write speed commands
            for (int i = 0; i < 3; i++) {
                solo.SetSpeedReferenceCommand(MOTOR_IDS[i], leftSpeed, false);
            }

            for (int i = 3; i < 6; i++) {
                solo.SetSpeedReferenceCommand(MOTOR_IDS[i], rightSpeed, true);
            }

            leftSpeed = 0;
            rightSpeed = 0;

            // read speed feedback
            for (int i = 0; i < NUM_MOTORS; i++) {
                solo.GetSpeedFeedbackCommand(MOTOR_IDS[i]);
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
