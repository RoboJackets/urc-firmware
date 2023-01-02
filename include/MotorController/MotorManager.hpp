#ifndef MOTORMANAGER_HPP
#define MOTORMANAGER_HPP

#include <Arduino.h>
#include "Context.hpp"
#include "MotorController/MotorController.hpp"

class MotorManager {
public:
    MotorManager() {};
    void setup(Context &context);
private:

    class Motor {
    public:
        Motor() {};
        void init(char *name, motors::MotorController *motorController, uint8_t channel);
        int32_t getSpeed();
    private:
        const static size_t MAX_NAME_LEN = 30;
        motors::MotorController *_motorController;
        uint8_t _address;
        uint8_t _channel;
        char _name[MAX_NAME_LEN];
    };

    Motor motors[motors::NUM_MOTORCONTROLLERS];
};


#endif