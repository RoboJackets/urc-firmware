#include "SoloCAN.hpp"

namespace solo_can {

SoloCan::SoloCan(FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> &_can) : can(_can) {}

void SoloCan::GetPositionFeedbackCommand(int soloID) {
    struct CanOpenData data = {
        .id = (uint16_t)(0x0600 + soloID),
        .type = SDO_READ_COMMAND,
        .code = POSITION_FEEDBACK_CODE,
        .payload = 0
    };
    can.write(createMessage(data));
    delayMicroseconds(200);
}

void SoloCan::GetSpeedFeedbackCommand(int soloID) {
    struct CanOpenData data = {
        .id = (uint16_t)(0x0600 + soloID),
        .type = SDO_READ_COMMAND,
        .code = SPEED_FEEDBACK_CODE,
        .payload = 0
    };
    can.write(createMessage(data));
    delayMicroseconds(200);
}

void SoloCan::SetSpeedReferenceCommand(int soloID, int speedRef) {
    uint32_t dir = (uint32_t)(speedRef < 0 ? 1 : 0);
    uint32_t speedMag = (uint32_t)abs(speedRef);
    struct CanOpenData data;

    Serial.print("Speed ref: ");
    Serial.println(speedMag);

    // set speed
    data = (struct CanOpenData) {
        .id = (uint16_t)(0x0600 + soloID),
        .type = SDO_WRITE_COMMAND,
        .code = SPEED_REF_CODE,
        .payload = speedMag
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

void SoloCan::GetBoardTemperatureCommand(int soloID) {
    struct CanOpenData data = (struct CanOpenData){
      .id = 0x0600 + soloID,
      .type = SDO_READ_COMMAND,
      .code = TEMP_CODE,
      .payload = 0
    };
    can.write(createMessage(data));
    delayMicroseconds(200);
}

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

}