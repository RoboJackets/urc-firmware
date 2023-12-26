#include <Arduino.h>
#include <StatusLight.hpp>

const int GREEN_PIN = 5;
const int BLUE_PIN = 10;
const int RED_PIN = 0;


int main() {

    // LED setup
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);
    pinMode(RED_PIN, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);

    status_light::StatusLight light({{"GREEN", GREEN_PIN},{"BLUE", BLUE_PIN},{"RED", RED_PIN}});



}