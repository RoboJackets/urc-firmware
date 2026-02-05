#if defined(__linux__)
#include <fcntl.h>
#include <termios.h>
#define STDIN_FILENO 0
#elif defined(_WIN64)
#include <conio.h>
#endif

#include <stdlib.h>
#include <stdio.h>

#include "dynamixel_sdk.h"  // Uses DYNAMIXEL SDK library
#define PROTOCOL_VERSION  2.0 // Dynamixel Protocol Version
#define DXL_ID  1 // Default ID for Dynamixel servos
#define DEVICENAME  "/dev/ttyUSB0" // Linux port assigned to U2D2 (USB-to-serial converter)

#define X_SERIES // Type of Dynamixel servos we are using

#define ADDR_TORQUE_ENABLE          64
#define ADDR_GOAL_POSITION          116
#define ADDR_PRESENT_POSITION       132
#define MINIMUM_POSITION_LIMIT      0  // Refer to the Minimum Position Limit of product eManual
#define MAXIMUM_POSITION_LIMIT      4095  // Refer to the Maximum Position Limit of product eManual
#define BAUDRATE                    57600

#define TORQUE_ENABLE                   1
#define TORQUE_DISABLE                  0
#define DXL_MOVING_STATUS_THRESHOLD     20  // DYNAMIXEL moving status threshold
#define ESC_ASCII_VALUE                 0x1b

void test_stepper();
void test_stepper_2();
void run_stepper();
void run_stepper_2();
void run_roboclaw_speed(int address, int channel, int speed);
void run_roboclaw_effort(int address, int channel, int effort);