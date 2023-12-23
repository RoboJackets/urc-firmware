#include <Arduino.h>

int main() {
    pinMode(LED_BUILTIN, OUTPUT);

    while (true) {
        pinMode(LED_BUILTIN, HIGH);
        delay(500);
        pinMode(LED_BUILTIN, LOW);
        delay(500);
    }
}