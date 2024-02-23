#include <Arduino.h>
#include <FlexCAN_T4.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

const int BLINK_RATE_MS = 500;
const int READ_RATE_MS = 1000;
const int SCREEN_RATE_MS = 5000;
const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 64;

elapsedMillis readTimer;
elapsedMillis blinkTimer;
elapsedMillis screenTimer;

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can;
CAN_message_t rmsg;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

struct BMSInfo {
    float soc;
    float voltage;
    float current;
    bool is_discharging;
    int capacity;
};

int countVar = 0;
struct BMSInfo batteryInfo;

int main() {

    pinMode(LED_BUILTIN, OUTPUT);

    can.begin();
    can.setBaudRate(250000);

    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    delay(500);
    display.clearDisplay();
    display.display();

    while (true) {

        // if (can.read(rmsg)) {
        //     Serial.print("Got data: ");
        //     Serial.printf("[ID=0x%.4x", rmsg.id);
        //     Serial.println(", Data=");
        //     for (int i = 0; i < rmsg.len; i++) {
        //         Serial.printf("0x%.2x", rmsg.buf[i]);
        //         if (i != rmsg.len) Serial.print(" ");
        //     }
        //     Serial.print("]");
        // }

        if (can.read(rmsg)) {
            if (rmsg.id == 0x18904001) {
                batteryInfo.voltage = (float)((rmsg.buf[0] << 8 | rmsg.buf[1]) / 10.0f);
                batteryInfo.current = (float)(((rmsg.buf[4] << 8 | rmsg.buf[5]) - 30000) / 10.0f);
                batteryInfo.soc =  (float)((rmsg.buf[6] << 8 | rmsg.buf[7]) / 10.0f);
            } else if (rmsg.id == 0x18934001) {
                batteryInfo.is_discharging = (bool)(rmsg.buf[2]);
                batteryInfo.capacity = (rmsg.buf[4] << 24 | rmsg.buf[5] << 16 | rmsg.buf[6] << 8 | rmsg.buf[7]);
            }
        }

        if (screenTimer >= SCREEN_RATE_MS) {
            screenTimer -= SCREEN_RATE_MS;

            countVar += 5;

            display.clearDisplay();
            display.setTextSize(1);
            display.setTextColor(WHITE);
            display.setCursor(0, 0);
            // soc
            display.print("SoC: ");
            display.print(batteryInfo.soc, 2);
            display.println("%");
            // volts
            display.print("VCC: ");
            display.print(batteryInfo.voltage, 2);
            display.println("V");
            // amps
            display.print("Amp: ");
            display.print(batteryInfo.current, 2);
            display.println("A");
            // discharge
            display.print("Out: ");
            if (batteryInfo.is_discharging) {
                display.println("ON");
            } else {
                display.println("OFF");
            }
            // capacity
            display.print("Cap: ");
            display.print(batteryInfo.capacity);
            display.println("mAh");
            // count
            display.print("Cnt: ");
            display.println(countVar);
            // display
            display.display(); 
        }

        if (readTimer >= READ_RATE_MS) {
            readTimer -= READ_RATE_MS;

            CAN_message_t msg;
            msg.len = 8;
            msg.flags.extended = 1;
            msg.buf[0] = 0;
            msg.buf[1] = 0;
            msg.buf[2] = 0;
            msg.buf[3] = 0;
            msg.buf[4] = 0;
            msg.buf[5] = 0;
            msg.buf[6] = 0;
            msg.buf[7] = 0;

            // ID: 0x90
            msg.id = 0x18900140;

            can.write(msg);
            delayMicroseconds(200);

            // ID: 0x93
            msg.id = 0x18930140;
            can.write(msg);
            delayMicroseconds(200);
        }

        if (blinkTimer >= BLINK_RATE_MS) {
            blinkTimer -= BLINK_RATE_MS;

            digitalToggle(LED_BUILTIN);
        }
    }
}