#ifndef TMOTOR_H
#define TMOTOR_H

#include "Motors.h"
#include "../../lib/SoloCAN/include/SoloCAN.hpp"

/************************SOLOCAN LIBRARY************************/
/***************TO BE USED WITH T-MOTOR BRUSHLESS MOTORS***************/

#define SOLO_DEFAULT_BAUD 1000000  // 1 Mbps CAN bus speed

// TMotor abstract interface
class TMotor : public Motors {
    public:
        virtual void setSpeed() = 0;
        virtual void getSpeed() = 0;
        virtual void setPosition() = 0;
        virtual void getPosition() = 0;
        virtual void getEffort() = 0;
        virtual void setEffort() = 0;
        virtual void stop() = 0;

    protected:
};

// Initialize CAN bus; call once at startup
void init(long baudrate = SOLO_DEFAULT_BAUD);

// Enable/disable motor (mirrors Dynamixel's enableTorque/disableTorque)
void enableMotor(uint8_t id);
void disableMotor(uint8_t id);

// Speed control (RPM)
void setSpeed(uint8_t id, int rpm);
long getSpeed(uint8_t id);

// Position control (encoder counts)
void setPosition(uint8_t id, long counts);
long getPosition(uint8_t id);

// Torque/current control
void setEffort(uint8_t id, int effort);
float getEffort(uint8_t id);

// Stop motor
void stop(uint8_t id);

#endif
