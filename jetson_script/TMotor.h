#ifndef TMOTOR_H
#define TMOTOR_H

#include "Motor.h"
#include <cstring>
#include <cstdint>
#include <stdexcept>
#include <chrono>
// NOTE: will show errors since these are linux default packages
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

class TMotor : public Motor {
    public:
        TMotor(const std::string & iface, uint8_t, int timeout_ms = 100);
        ~TMotor() override;

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
