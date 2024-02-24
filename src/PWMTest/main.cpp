#include <Arduino.h>
// 24: IN2
// 25: IN1
// 32: ENA/PWM

#define IN1 25
#define IN2 24
#define ENA 29

int main() {
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(ENA, OUTPUT);

    digitalWrite(IN1, HIGH);
    analogWrite(ENA, 256);
    digitalWrite(IN2, HIGH);
}