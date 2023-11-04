#include <FlexCAN_T4.h>
#include "SoloCAN.hpp"

const int BLINK_RATE = 500;
const int BAUD_RATE = 1000000;
const int NUM_MOTORS = 3;
const int MOTOR_IDS[NUM_MOTORS] = {0xA4, 0xA3, 0xA6};

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> myCan1;
elapsedMillis blinkTimer;
elapsedMillis sendTimer;
CAN_message_t rmsg;

int main() {

    pinMode(LED_BUILTIN, OUTPUT);
    myCan1.begin();
    myCan1.setBaudRate(BAUD_RATE);

    solo_can::SoloCan solo = solo_can::SoloCan(myCan1);

    while (true) {
        // parse incoming CAN messages
        if (myCan1.read(rmsg)) {
            struct solo_can::CanOpenData data = solo_can::parseMessage(rmsg);

            if (data.type == solo_can::SDO_READ_RESPONSE) {
                if (data.code == solo_can::TEMP_CODE) {
                    Serial.print("Got board temperature: ");
                    Serial.print("[ID=");
                    Serial.print(data.id, HEX);
                    Serial.print(", TEMP=");
                    Serial.print(solo_can::toFloat(data.payload), 7);
                    Serial.println("]");
                }
            }
        }

        // send CAN message
        if (sendTimer >= BLINK_RATE) {
            sendTimer -= BLINK_RATE;
            for (int i = 0; i < NUM_MOTORS; i++) {
                solo.GetBoardTemperatureCommand(MOTOR_IDS[i]);
            }
        }

        // blink LED
        if (blinkTimer >= BLINK_RATE) {
            blinkTimer -= BLINK_RATE;
            digitalToggle(LED_BUILTIN);
        }
    }

}