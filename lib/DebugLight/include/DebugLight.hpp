#pragma once

#include <Arduino.h>
#include <string>
#include <cstring>

namespace debug_light {

const double DEFAULT_UPDATE_INTERVAL_MS = 300;
const double MIN_UPDATE_INTERVAL_MS = 100;

const std::string morse[] = {
    ".-",   // A
    "-...", // B
    "-.-.", // C
    "-..",  // D
    ".",    // E
    "..-.", // F
    "--.",  // G
    "....", // H
    "..",   // I
    ".---", // J
    "-.-"   // K
    ".-..", // L
    "--",   // M
    "-.",   // N
    "---",  // O
    ".--.", // P
    "--.-", // Q
    ".-.",  // R
    "...",  // S
    "-",    // T
    "..-",  // U
    "...-", // V
    ".--",  // W
    "-..-", // X
    "-.--", // Y
    "--.."  // Z   
};

static const std::string& toMorse(char c) {
    int ord = static_cast<int>(toupper(c)) - static_cast<int>('A');
    if (ord >= 0 && ord < 26) {
        return morse[ord];
    } else {
        return morse[0];
    }
}


class DebugLight {
public:

    DebugLight() {
        updateInterval = DEFAULT_UPDATE_INTERVAL_MS;
        pin = LED_BUILTIN;
        patternActive = false;
    }

    DebugLight(uint8_t _pin) {
        updateInterval = DEFAULT_UPDATE_INTERVAL_MS;
        pin = _pin;
        patternActive = false;
    }

    DebugLight(uint8_t _pin, double _updateInterval) {
        if (_updateInterval >= MIN_UPDATE_INTERVAL_MS) {
            updateInterval = _updateInterval;
        } else {
            updateInterval = DEFAULT_UPDATE_INTERVAL_MS;
        }

        pin = _pin;
        patternActive = false;
    }

    void update() {
        if (patternActive) {
            if (timer >= updateInterval) {
                timer -= updateInterval;
                digitalWrite(pin, updatePattern(currentPattern));
            }
        } else {
            if (timer >= updateInterval * 3) {
                timer -= updateInterval * 3;
                digitalToggle(pin);
            }
        }
    }

    void setPattern(const std::string code) {
        currentPattern = (struct Pattern){.str = code};
        patternActive = true;
    } 

    void disablePattern() {
        patternActive = false;
    }

private:
    static elapsedMillis timer;    
    double updateInterval;
    uint8_t pin;

    // Patterns (for error codes)
    struct Pattern {
        std::string str;
        int strIdx;
        int morseIdx;
        int count;
    };

    struct Pattern currentPattern;
    bool patternActive;

    uint8_t updatePattern(struct Pattern& pattern) {

        // end of string
        if (pattern.strIdx >= pattern.str.length()) {
            // wait 3 counts at end of pattern before resetting
            if (pattern.count >= 2) {
                resetPattern(pattern);
            } else {
                pattern.count += 1;
            }
            return LOW;
        }

        // get morse code string for char at strIdx
        char curr = pattern.str[pattern.strIdx];
        const std::string& code = debug_light::toMorse(curr);

        // end of character
        if (pattern.morseIdx >= code.length()) {
            advancePattern(pattern);
            return LOW;
        }

        if (code[pattern.morseIdx] == '.') {
            // dot is 1 count
            pattern.morseIdx += 1;
            return HIGH;
        } else if (code[pattern.morseIdx] == '-') {
            // dash is 3 counts
            if (pattern.count >= 2) {
                pattern.morseIdx += 1;
                pattern.count = 0;
            } else {
                pattern.count += 1;
            }
            return HIGH;
        } else {
            // unknown character
            pattern.morseIdx += 1;
            return LOW;
        }
    }
    void resetPattern(struct Pattern& pattern) {
        pattern.strIdx = 0;
        pattern.morseIdx = 0;
        pattern.count = 0;
    }
    void advancePattern(struct Pattern& pattern) {
        pattern.strIdx += 1;
        pattern.morseIdx = 0;
        pattern.count = 0;
    }
};

}