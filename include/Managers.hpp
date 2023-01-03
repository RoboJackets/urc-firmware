#ifndef MANAGERS_HPP
#define MANAGERS_HPP

#include <Arduino.h>
#include "Context.hpp"
#include "MotorController/MotorController.hpp"

namespace manager {

const size_t NUM_MOTORS = 2;

class MotorManager {
public:
    MotorManager(Context &context);
    void update(Context &context);
private:

    class Motor {
    public:
        Motor() {};
        void init(const char *name, motors::MotorController *motorController, uint32_t channel);
        int32_t getSpeed(bool &valid);
        void setSpeed(int32_t speed, bool &valid);
    private:
        const static size_t MAX_NAME_LEN = 30;
        motors::MotorController *_motorController;
        uint32_t _address;
        uint32_t _channel;
        char _name[MAX_NAME_LEN];
    };
    Motor _motors[NUM_MOTORS];
};
    
}

#endif