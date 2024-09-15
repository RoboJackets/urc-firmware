#include <Arduino.h>
#include <QNEthernet.h>
#include "urc.pb.h"
#include <Messages.hpp>
#include "StatusLight.hpp"

// v1.0 PCB pins
// const int GREEN_PIN = 30;
// const int BLUE_PIN = 31;
// const int RED_PIN = 32;

// protoboard pins
const int GREEN_PIN = 35;
const int BLUE_PIN = 34;
const int RED_PIN = 32;

const int GREEN_PROTO = 0;
const int BLUE_PROTO = 1;
const int RED_PROTO = 2;

const int BLINK_RATE_MS = 500;
const int PORT = 8443;
const uint8_t CLIENT_IP[] = { 192, 168, 1, 84 };
StatusLightCommand requestCommand;

qindesign::network::EthernetUDP udp;


// timer variable
elapsedMillis blinkTimer;
int ledPin = -1;
bool blinkEnabled = false;


void handleLEDRequest(int color, int display);

int main() {
    // LED setup
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);
    pinMode(RED_PIN, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    // status_light::StatusLight statusLight({{"GREEN", GREEN_PIN},{"BLUE", BLUE_PIN},{"RED", RED_PIN}});

    // Ethernet setup
    qindesign::network::Ethernet.begin();
    udp.begin(PORT);

    // initialize data
    size_t requestLength;
    uint8_t requestBuffer[256];

    // handleLEDRequest(RED_PROTO, 1);

    while (true) {
        // OLD
        digitalWrite(GREEN_PIN, HIGH);
        delay(500);
        digitalWrite(BLUE_PIN, HIGH);
        delay(500);
        digitalWrite(RED_PIN, HIGH);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(2000);
        digitalWrite(GREEN_PIN, LOW);
        delay(500);
        digitalWrite(BLUE_PIN, LOW);
        delay(500);
        digitalWrite(RED_PIN, LOW);
        digitalWrite(LED_BUILTIN, LOW);
        delay(2000);

        // // // read incoming UDP messages
        // requestLength = udp.parsePacket();
        // if (udp.available()) { 

        //     Serial.println("Packet received: ");

        //     memset(requestBuffer, 0, 256);
        //     udp.readBytes(requestBuffer, requestLength);
        //     bool success = protobuf::Messages::decodeStatusLightRequest(requestBuffer, requestLength, requestCommand);
        
        //     Serial.print("[color=");
        //     Serial.print(requestCommand.color);
        //     Serial.print(", display=");
        //     Serial.print(requestCommand.display);
        //     Serial.println("]");

        //     if (requestCommand.color == 0) {
        //         digitalWrite(GREEN_PIN, HIGH);
        //     } else {
        //         digitalWrite(GREEN_PIN, LOW);
        //     }

        //     // // OLD
        //     // handleLEDRequest(requestCommand.color, requestCommand.display);  
            
        //     // NEW
        //     // statusLight.resetLeds();
        //     // statusLight.setLedState("GREEN", HIGH);
        //     // statusLight.setLedBlink("GREEN", requestCommand.display);
        // }

        // // OLD
        // if (blinkTimer >= BLINK_RATE_MS) {
        //     blinkTimer -= BLINK_RATE_MS;

        //     if (ledPin >= 0) {
        //         if (blinkEnabled) {
        //             digitalToggle(ledPin);
        //         } else {
        //             digitalWrite(ledPin, HIGH);
        //         }
        //     }

        //     digitalToggle(LED_BUILTIN);
        // }

        // NEW 
        // statusLight.update();

        // if (blinkTimer >= BLINK_RATE_MS) {
        //     blinkTimer -= BLINK_RATE_MS;
        //     digitalToggle(LED_BUILTIN);
        // }
    }
}

void handleLEDRequest(int color, int display) {
    int newPin;

    if (color == GREEN_PROTO) {
        newPin = GREEN_PIN;
    } else if (color == BLUE_PROTO) {
        newPin = BLUE_PIN;
    } else if (color == RED_PROTO) {
        newPin = RED_PIN;
    } else {
        newPin = -1;
    }

    if (display == 0) {
        blinkEnabled = false;
    } else {
        blinkEnabled = true;
    }

    if (newPin != ledPin) {
        if (ledPin >= 0) {
            digitalWrite(ledPin, HIGH);
        }

        ledPin = newPin;
    }
}