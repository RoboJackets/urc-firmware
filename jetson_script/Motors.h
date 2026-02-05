/*
Utilizing this header file as an interface to house empty skeletons for
all the general motor functions that will be used across different
subsystems (arm, science, drivetrain)
*/

#ifdef MOTORS_H
#define MOTORS_H

class Motor {
    public:
        // Essential so that children classes are cleaned up properly
        virtual ~Motor() = default;

        // Disable slicing as safety precaution
        Motor(const Motor&) = delete;
        Motor& operator = (const Motor&) = delete;

        // General motor methods that each motor in every subsystem may need
        // UPDATE AS WE GO ALONG
        virtual void setSpeed(double speed) = 0;
        virtual void setPosition(double position) = 0;
        virtual void setEffort(double torque) = 0;
        virtual void stop() = 0;
};

#endif