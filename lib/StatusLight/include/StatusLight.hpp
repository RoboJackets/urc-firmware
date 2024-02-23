#pragma once

#include <Arduino.h>
#include <unordered_map>

namespace status_light {

const double DEFAULT_PERIOD = 1.0;

class StatusLight {
public:
    StatusLight(std::unordered_map<std::string, uint8_t> pin_map) {

        blinkPeriod = DEFAULT_PERIOD;
        for (auto& pair : pin_map) {
            led[pair.first] = (struct Led){.pin=pair.second, .state=0, .blink=0};
        }
    }

    void setPeriod(double period) { period > 0 ? blinkPeriod = period: blinkPeriod = DEFAULT_PERIOD; }
    bool setLedState(const std::string& name, uint8_t state) {
        if (led.find(name) != led.end()) {
            led[name].state = state;
            return true;
        } else {
            return false;
        }
    }
    bool setLedBlink(const std::string& name, uint8_t blink) {
        if (led.find(name) != led.end()) {
            led[name].blink = blink;
            return true;
        } else {
            return false;
        }
    }
    void resetLeds() {
        for (auto& pair : led) {
            pair.second.state = 0;
            pair.second.blink = 0;
        }
    }
    void update() {
        if (blinkPeriod >= blinkPeriod) {
            // LOW
            blinkPeriod -= blinkPeriod;
            for (const auto& pair : led) {
                if (!pair.second.state) {
                    pinMode(pair.second.pin, LOW);
                } else if (pair.second.blink) {
                    pinMode(pair.second.pin, LOW);
                } else {
                    pinMode(pair.second.pin, HIGH);
                }
            }

        } else if (blinkPeriod >= blinkPeriod / 2.0) {
            // HIGH
            for (const auto& pair : led) {
                if (!pair.second.state) {
                    pinMode(pair.second.pin, LOW);
                } else if (pair.second.blink) {
                    pinMode(pair.second.pin, HIGH);
                } else {
                    pinMode(pair.second.pin, HIGH);
                }
            }
        }
    }

private:

    struct Led {
        uint8_t pin;
        uint8_t state;
        uint8_t blink;
    };

    std::unordered_map<std::string, struct Led> led;
    double blinkPeriod;
    static elapsedMillis blinkTimer;
};

}