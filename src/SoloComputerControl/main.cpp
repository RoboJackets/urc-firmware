#include <Arduino.h>
#include <FlexCAN_T4.h>
#include <QNEthernet.h>
#include <map>
#include "urc.pb.h"
#include "Messages.hpp"
#include "SoloCAN.hpp"

// constants
const int BLINK_RATE_MS = 500;
const int CAN_READ_RATE_MS = 200;
const int UDP_WRITE_RATE_MS = 1000;
const int BAUD_RATE = 500000;
const int NUM_MOTORS = 4;
const int MOTOR_IDS[NUM_MOTORS] = {0xA1, 0xA2, 0xA3, 0xA4};
const int PORT = 8443;
const uint8_t CLIENT_IP[] = { 192, 168, 1, 151 };

// variables
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can;
qindesign::network::EthernetUDP udp;
std::map<uint16_t, uint32_t> encoderData;
DriveEncodersMessage requestMessage;
DriveEncodersMessage responseMessage;

// timer variables
elapsedMillis currentTime;
elapsedMillis blinkTimer;
elapsedMillis canReadTimer;
elapsedMillis udpWriteTimer;

int speedFeedback = 0;

int main()  {
    // LED setup
    pinMode(LED_BUILTIN, OUTPUT);

    // Ethernet setup
    qindesign::network::Ethernet.begin();
    udp.begin(PORT);

    // CAN setup
    can.begin();
    can.setBaudRate(BAUD_RATE);
    solo_can::SoloCan<CAN1, RX_SIZE_256, TX_SIZE_16> solo(can);

    // initialize data
    // requestMessage = RequestMessage_init_zero;
    requestMessage = DriveEncodersMessage_init_zero;
    responseMessage = DriveEncodersMessage_init_zero;
    CAN_message_t canMsg;
    solo_can::CanOpenData canResponseMessage; 
    size_t requestLength;
    uint8_t requestBuffer[256];
    uint8_t responseBuffer[256];

    while (true) {

        // read incoming UDP messages
        requestLength = udp.parsePacket();
        if (udp.available()) { 

            Serial.print("Packet received: ");

            memset(requestBuffer, 0, 256);
            udp.readBytes(requestBuffer, requestLength);
            // protobuf::Messages::decodeRequest(requestBuffer, requestLength, requestMessage);
            protobuf::Messages::decodeRequest(requestBuffer, requestLength, requestMessage);
        
            if (requestMessage.leftSpeed > 4000) requestMessage.leftSpeed = 4000;
            else if (requestMessage.leftSpeed < -4000) requestMessage.leftSpeed = -4000;

            if (requestMessage.rightSpeed > 4000) requestMessage.rightSpeed = 4000;
            else if (requestMessage.rightSpeed < -4000) requestMessage.rightSpeed = -4000;

            Serial.print("left: ");
            Serial.print(requestMessage.leftSpeed);
            Serial.print(", right: ");
            Serial.print(requestMessage.rightSpeed);
            Serial.println("");
        }

        // write CAN messages
        can.events();

        // check for incoming CAN messages
        if (can.read(canMsg)) {
            canResponseMessage = solo_can::parseMessage(canMsg);

            // record speed reference command
            if (canResponseMessage.type == solo_can::SDO_READ_RESPONSE && canResponseMessage.code == solo_can::SPEED_FEEDBACK_CODE) {
                // encoderData[canResponseMessage.id] = canResponseMessage.payload - 0x580;
                speedFeedback = canResponseMessage.payload - 0x580;
            }
        }

        // // write UDP message at regular interval
        // if (udpWriteTimer >= UDP_WRITE_RATE_MS) {
        //     udpWriteTimer -= UDP_WRITE_RATE_MS;

        //     // populate responseMessage
        //     responseMessage.timestamp = currentTime;
            
        //     int avgSpeedLeft = 0;
        //     for (int i = 0; i < 3; i++) {
        //         avgSpeedLeft += encoderData[MOTOR_IDS[i]];
        //     }
        //     avgSpeedLeft /= 3;
        //     responseMessage.leftSpeed = avgSpeedLeft;

        //     int avgSpeedRight = 0;
        //     for (int i = 3; i < 6; i++) {
        //         avgSpeedRight += encoderData[MOTOR_IDS[i]];
        //     }
        //     avgSpeedRight /= 3;
        //     responseMessage.leftSpeed = avgSpeedRight;

        //     size_t responseLength = protobuf::Messages::encodeResponse(responseBuffer, sizeof(responseBuffer), responseMessage);
        //     udp.beginPacket(CLIENT_IP, PORT);
        //     udp.write(responseBuffer, responseLength);
        //     udp.endPacket();

        //     // Serial.println("Packet sent");
        // }

        // send CAN read speed reference command
        if (canReadTimer >= CAN_READ_RATE_MS) {
            canReadTimer -= CAN_READ_RATE_MS;

            Serial.println("Sending!");

            // // speed ref
            // solo.SetSpeedReferenceCommand(MOTOR_IDS[0], requestMessage.leftSpeed, false);
            // solo.GetSpeedFeedbackCommand(MOTOR_IDS[0]);

            // solo.SetSpeedReferenceCommand(MOTOR_IDS[0], requestMessage.leftSpeed, encoderData[MOTOR_IDS[0]], false);
            // solo.GetSpeedFeedbackCommand(MOTOR_IDS[0]);

            // write CAN
            for (int i = 0; i < 2; i++) {
                solo.SetSpeedReferenceCommand(MOTOR_IDS[i], requestMessage.leftSpeed, false);
            }

            for (int i = 2; i < 4; i++) {
                solo.SetSpeedReferenceCommand(MOTOR_IDS[i], requestMessage.rightSpeed, true);
            }

            // // // torque ref
            // solo.SetTorqueReferenceCommand(MOTOR_IDS[0], requestMessage.leftSpeed, false);
            // solo.SetTorqueReferenceCommand(MOTOR_IDS[1], requestMessage.leftSpeed, false);
            // solo.SetTorqueReferenceCommand(MOTOR_IDS[2], requestMessage.leftSpeed, false);
            // solo.SetTorqueReferenceCommand(MOTOR_IDS[3], requestMessage.rightSpeed, true);
            // solo.SetTorqueReferenceCommand(MOTOR_IDS[4], requestMessage.rightSpeed, true);
            // solo.SetTorqueReferenceCommand(MOTOR_IDS[5], requestMessage.rightSpeed, true);

            // read CAN
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