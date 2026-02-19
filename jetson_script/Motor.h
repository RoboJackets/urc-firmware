/*
Utilizing this header file as an interface to house empty skeletons for
all the general motor functions that will be used across different
subsystems (arm, science, drivetrain)
*/

#ifndef MOTORS_H
#define MOTORS_H

#include <cstdint>

class Motor {
    public:
        // Essential so that children classes are cleaned up properly
        virtual ~Motor() = default;

        // Disable slicing as safety precaution
        Motor(const Motor&) = delete;
        Motor& operator = (const Motor&) = delete;

    protected:
        Motor() = default;

        // General motor methods that each motor in every subsystem may need
        // UPDATE AS WE GO ALONG
        virtual void setSpeed() = 0;
        virtual void getSpeed() = 0;
        virtual void setPosition() = 0;
        virtual void getPosition() = 0;
        virtual void getEffort() = 0;
        virtual void setEffort() = 0;
        virtual void stop() = 0;
};

#endif
