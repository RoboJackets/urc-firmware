#ifdef TMOTOR_H
#define TMOTOR_H

#include "Motors.h"

class TMotor : public Motors {
    public:
        virtual void setSpeed() = 0;
        virtual void getSpeed() = 0;
        virtual void setPosition() = 0;
        virtual void getPosition() = 0;
        virtual void getEffort() = 0;
        virtual void setEffort() = 0;
        virtual void stop() = 0;
}

#endif