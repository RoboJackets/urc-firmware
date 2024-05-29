#pragma once

#include <Arduino.h>
#include <FlexCAN_T4.h>
#include <cmath>
#include <queue>

namespace solo_can {

    const int SDO_READ_COMMAND = 0x40;
    const int SDO_READ_RESPONSE = 0x42;
    const int SDO_WRITE_COMMAND = 0x22;
    const int SDO_WRITE_RESPONSE = 0x60;

    const int TEMP_CODE = 0x3039;
    const int SPEED_REF_CODE = 0x3005;
    const int TORQUE_REF_CODE = 0x3004;
    const int SPEED_FEEDBACK_CODE = 0x3036;
    const int POSITION_FEEDBACK_CODE = 0x3037;
    const int MOTOR_DIRECTION_CODE = 0x300C;
    const int CONTROL_MODE_CODE = 0x3016;

    const uint32_t MODE_SPEED = 0;
    const uint32_t MODE_TORQUE = 1; 
    const uint32_t MODE_POSITION = 2; 

    struct CanOpenData {
        uint16_t id;
        uint8_t type;
        uint16_t code;
        uint32_t payload;
    };

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

    float toFloat(uint32_t payload) {
        if (payload <=  0x7FFE0000) {
            return payload / 131072.0;
        } else {
            return (0xFFFFFFFF - payload + 0x1) / -131072.0;
        }
    }

    template<CAN_DEV_TABLE _bus, FLEXCAN_RXQUEUE_TABLE _rxSize, FLEXCAN_TXQUEUE_TABLE _txSize> 
    class SoloCan {
    public:
        SoloCan(FlexCAN_T4<_bus, _rxSize, _txSize> &_can) : can(_can) {}

        void processMessageQueue() {
            while (!sendQueue.empty() && can.getTXQueueCount() < 12) {
                can.write(sendQueue.front());
                can.
                sendQueue.pop();
            }
        }

        void GetBoardTemperatureCommand(int soloID) {
            struct CanOpenData data = (struct CanOpenData){
                .id = 0x0600 + soloID,
                .type = SDO_READ_COMMAND,
                .code = TEMP_CODE,
                .payload = 0
            };
            // sendQueue.push(createMessage(data));
            can.write(createMessage(data));
            // delayMicroseconds(200);
        }

        void GetSpeedFeedbackCommand(int soloID) {
            struct CanOpenData data = {
                .id = (uint16_t)(0x0600 + soloID),
                .type = SDO_READ_COMMAND,
                .code = SPEED_FEEDBACK_CODE,
                .payload = 0
            };
            // sendQueue.push(createMessage(data));
            can.write(createMessage(data));
            // delayMicroseconds(600);
        }

        void GetPositionFeedbackCommand(int soloID) {
            struct CanOpenData data = {
                .id = (uint16_t)(0x0600 + soloID),
                .type = SDO_READ_COMMAND,
                .code = POSITION_FEEDBACK_CODE,
                .payload = 0
            };
            // sendQueue.push(createMessage(data));
            can.write(createMessage(data));
            // delayMicroseconds(200);
        }
        
        void SetSpeedReferenceCommand(int soloID, int speedRef, bool isReversed) {
            uint32_t dir = (uint32_t)(speedRef < 0 ? 1 : 0);
            if (isReversed) dir ^= 1;
            uint32_t speedMag = (uint32_t)abs(speedRef);
            struct CanOpenData data;

            // Serial.print("Speed ref: ");
            // Serial.println(speedMag);

            // // set control mode
            // if (speedRef == 0 && (abs(speedFeedback) <= 200)) {
            //     data = (struct CanOpenData) {
            //         .id = (uint16_t)(0x0600 + soloID),
            //         .type = SDO_WRITE_COMMAND,
            //         .code = CONTROL_MODE_CODE,
            //         .payload = MODE_TORQUE
            //     };

            // } else {
            //     data = (struct CanOpenData) {
            //         .id = (uint16_t)(0x0600 + soloID),
            //         .type = SDO_WRITE_COMMAND,
            //         .code = CONTROL_MODE_CODE,
            //         .payload = MODE_SPEED
            //     };
            // }

            data = (struct CanOpenData) {
                .id = (uint16_t)(0x0600 + soloID),
                .type = SDO_WRITE_COMMAND,
                .code = CONTROL_MODE_CODE,
                .payload = MODE_SPEED
            };
            // sendQueue.push(createMessage(data));
            can.write(createMessage(data));
            // delayMicroseconds(200);

            // // set control mode
            // data = (struct CanOpenData) {
            //     .id = (uint16_t)(0x0600 + soloID),
            //     .type = SDO_WRITE_COMMAND,
            //     .code = CONTROL_MODE_CODE,
            //     .payload = MODE_SPEED
            // };
            // can.write(createMessage(data));
            // delayMicroseconds(200);

            // set speed
            data = (struct CanOpenData) {
                .id = (uint16_t)(0x0600 + soloID),
                .type = SDO_WRITE_COMMAND,
                .code = SPEED_REF_CODE,
                .payload = speedMag
            };
            // sendQueue.push(createMessage(data));
            can.write(createMessage(data));
            // delayMicroseconds(200);

            // set direction
            data = (struct CanOpenData) {
                .id = (uint16_t)(0x0600 + soloID),
                .type = SDO_WRITE_COMMAND,
                .code = MOTOR_DIRECTION_CODE,
                .payload = dir
            };
            // sendQueue.push(createMessage(data));
            can.write(createMessage(data));
            // delayMicroseconds(200);
        }

        void SetTorqueReferenceCommand(int soloID, int torqueRef, bool isReversed) {
            struct CanOpenData data;

            double torqueActual = ((double)torqueRef / 1500.0);

            uint32_t dir = (uint32_t)(torqueActual < 0.0 ? 1 : 0);
            if (isReversed) dir ^= 1;

            uint32_t torque;
            if (abs(torqueActual) >= 2.0) torqueActual = 2.0;

            torque = std::floor(abs(torqueActual) * 131072.0);


            // set torque mode
            data = (struct CanOpenData) {
                .id = (uint16_t)(0x0600 + soloID),
                .type = SDO_WRITE_COMMAND,
                .code = CONTROL_MODE_CODE,
                .payload = MODE_TORQUE
            };

            can.write(createMessage(data));
            delayMicroseconds(200);

            // set torque
            data = (struct CanOpenData) {
                .id = (uint16_t)(0x0600 + soloID),
                .type = SDO_WRITE_COMMAND,
                .code = TORQUE_REF_CODE,
                .payload = torque
            };

            can.write(createMessage(data));
            delayMicroseconds(200);

            // set direction
            data = (struct CanOpenData) {
                .id = (uint16_t)(0x0600 + soloID),
                .type = SDO_WRITE_COMMAND,
                .code = MOTOR_DIRECTION_CODE,
                .payload = dir
            };

            can.write(createMessage(data));
            delayMicroseconds(200);
        }


    private:
        FlexCAN_T4<_bus, _rxSize, _txSize> &can;
        std::queue<CAN_message_t> sendQueue;
    };
}