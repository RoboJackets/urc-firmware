#include <Arduino.h>

int main() {

    pinMode(LED_BUILTIN, OUTPUT);

    while (true) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(500);
        digitalWrite(LED_BUILTIN, LOW);
        delay(500);
    }
}