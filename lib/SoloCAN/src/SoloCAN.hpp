#pragma once

#include <Arduino.h>
#include <FlexCAN_T4.h>

namespace solo_can {

    const int SDO_READ_COMMAND = 0x40;
    const int SDO_READ_RESPONSE = 0x42;
    const int SDO_WRITE_COMMAND = 0x22;
    const int SDO_WRITE_RESPONSE = 0x60;

    const int TEMP_CODE = 0x3039;
    const int SPEED_REF_CODE = 0x3005;
    const int SPEED_FEEDBACK_CODE = 0x3036;
    const int POSITION_FEEDBACK_CODE = 0x3037;
    const int MOTOR_DIRECTION_CODE = 0x300C;

    struct CanOpenData {
        uint16_t id;
        uint8_t type;
        uint16_t code;
        uint32_t payload;
    };

    CAN_message_t createMessage(struct CanOpenData data);
    struct CanOpenData parseMessage(CAN_message_t msg);

    float toFloat(uint32_t payload);

    class SoloCan {
    public:
        SoloCan(FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> &_can);

        void GetBoardTemperatureCommand(int soloID);
        void GetSpeedFeedbackCommand(int soloID);
        void GetPositionFeedbackCommand(int soloID);
        void SetSpeedReferenceCommand(int soloID, int speedRef);

    private:
        FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> &can;
    };

}