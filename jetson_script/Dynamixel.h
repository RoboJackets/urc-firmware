#ifndef DYNAMIXEL_H
#define DYNAMIXEL_H

#include "Motors.h"
#include "../include/dynamixel_sdk/dynamixel_sdk.h" // Uses DYNAMIXEL SDK library
#include "dynamixel_easy_sdk/motor.hpp"
#include "dynamixel_easy_sdk/connector.hpp"

/************************DYNAMIXEL LIBRARIES************************/
/*****************TO BE USED WITH DYNAMIXEL SERVOS***********************/
#if defined(__linux__)
#include <fcntl.h>
#include <termios.h>
#define STDIN_FILENO 0
#elif defined(_WIN64)
#include <conio.h>
#endif

#include <stdlib.h>
#include <stdio.h>

#define PROTOCOL_VERSION  2.0 // Dynamixel Protocol Version
#define DXL_ID  1 // Default ID for Dynamixel servos
#define DEVICENAME  "/dev/ttyUSB0" // Linux port assigned to U2D2 (USB-to-serial converter)

#define X_SERIES // Type of Dynamixel servos we are using

#define ADDR_TORQUE_ENABLE          64
#define ADDR_GOAL_CURRENT           102
#define ADDR_GOAL_VELOCITY          104
#define ADDR_GOAL_POSITION          116
#define ADDR_PRESENT_CURRENT        126
#define ADDR_PRESENT_VELOCITY       128
#define ADDR_PRESENT_POSITION       132
#define MINIMUM_POSITION_LIMIT      0  // Refer to the Minimum Position Limit of product eManual
#define MAXIMUM_POSITION_LIMIT      4095  // Refer to the Maximum Position Limit of product eManual
#define BAUDRATE                    57600

#define TORQUE_ENABLE                   1
#define TORQUE_DISABLE                  0
#define DXL_MOVING_STATUS_THRESHOLD     20  // DYNAMIXEL moving status threshold
#define ESC_ASCII_VALUE                 0x1b

// UPDATE AS WE GO ALONG
class Dynamixel : public Motors {
    public:
      virtual void setSpeed() = 0;
      virtual void getSpeed() = 0;
      virtual void setPosition() = 0;
      virtual void getPosition() = 0;
      virtual void getEffort() = 0;
      virtual void setEffort() = 0;
      virtual void stop() = 0;

    protected:
      virtual int getch() = 0;
      virtual int kbhit() = 0;
};

#endif