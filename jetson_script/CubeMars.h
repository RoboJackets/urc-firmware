#ifndef CUBEMARS_H
#define CUBEMARS_H

#include "Motor.h"
#include <cstring>
#include <cstdint>
#include <stdexcept>
#include <chrono>
#include <linux/can.h>
#include <linux/can/raw.h>


enum CAN_PACKET_ID : uint8_t{
    CAN_PACKET_SET_DUTY = 0,
    CAN_PACKET_SET_CURRENT,
    CAN_PACKET_SET_CURRENT_BRAKE,
    CAN_PACKET_SET_RPM,
    CAN_PACKET_SET_POS,
    CAN_PACKET_SET_ORIGIN_HERE,
    CAN_PACKET_SET_POS_SPD
};

// MIT mode physical limits

#define P_MIN -12.5f
#define P_MAX 12.5f
#define V_MIN -45.0f
#define V_MAX 45.0f
#define T_MIN -18.0f
#define T_MAX 18.0f
#define KP_MIN 0
#define KP_MAX 500.0f
#define KD_MIN 0
#define KD_MAX 5.0f

// CubeMars servo interface
class CubeMarsServo : public Motor {
    public:
        CubeMarsServo(const std::string & iface, uint8_t, int timeout_ms = 100);
        ~CubeMarsServo() override;

        virtual void setSpeed(float rpm)    override;
        virtual void getSpeed()             override;
        virtual void setPosition(float deg) override;
        virtual void getPosition()          override;
        virtual void getEffort(float amps)  override;
        virtual void setEffort()            override;
        virtual void stop()                 override;

    protected:
        int sock;
        uint8_t id;
        int timeout_ms;
};

#endif
