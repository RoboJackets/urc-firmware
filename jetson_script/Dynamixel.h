#ifndef DYNAMIXEL_H
#define DYNAMIXEL_H

#include <cstdint>
#include <stdio.h>

#include "Motor.h"
#include "dynamixel_sdk/port_handler.h"
#include "dynamixel_sdk/packet_handler.h"

/************************DYNAMIXEL LIBRARIES************************/
/*****************TO BE USED WITH DYNAMIXEL SERVOS***********************/

#define PROTOCOL_VERSION        2.0
#define BAUDRATE                57600

#define ADDR_TORQUE_ENABLE      64
#define ADDR_GOAL_CURRENT       102
#define ADDR_GOAL_VELOCITY      104
#define ADDR_GOAL_POSITION      116
#define ADDR_PRESENT_CURRENT    126
#define ADDR_PRESENT_VELOCITY   128
#define ADDR_PRESENT_POSITION   132

#define TORQUE_ENABLE           1
#define TORQUE_DISABLE          0

#define DEBUG                   0

class Dynamixel : public Motor {
    public:
        virtual void setSpeed()    = 0;
        virtual void getSpeed()    = 0;
        virtual void setPosition() = 0;
        virtual void getPosition() = 0;
        virtual void getEffort()   = 0;
        virtual void setEffort()   = 0;
        virtual void stop()        = 0;

    protected:
};

// --- Helper function declarations ---

void init(const char *deviceName, int baudrate = BAUDRATE);

void enableTorque(uint8_t id);
void disableTorque(uint8_t id);

void setSpeed(uint8_t id, int32_t velocity);
int32_t getSpeed(uint8_t id);

void setPosition(uint8_t id, int32_t position);
int32_t getPosition(uint8_t id);

void setEffort(uint8_t id, int16_t current);
int16_t getEffort(uint8_t id);

void stop(uint8_t id);

#endif
