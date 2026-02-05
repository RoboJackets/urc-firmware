#include "Motors.h"

/**************************GENERAL LIBRARIES**************************/
/*******************TO BE UTILIZED FOR ALL MOTORS*******************/

#include <Arduino.h>
#include <FlexCAN_T4.h> // CAN enabling tool
#include "SoloCAN.hpp" // SOLO UNO motor controller's library
#include <QNEthernet.h> // Teensy's ethernet library
#include <map>
#include "urc.ph.h" // Protobuf header file for compiling code
#include "Messages.hpp" // Protobuff decode functions (encodeResponse, decodeResponse)

#include "pb_encode.h" // Protobuf encode functions
#include "pb_decode.h" // Protobuf decode functions

#endif

// Data about each motor to be sent to SOLO
enum class CAN_Send_State {
    Motor_Setpoint,
    Motor_Speed,
    Motor_Current,
    Motor_Position
};

// Format of the feedback data to be sent to SOLO
struct Solo_Feedback_Data {
    uint32_t speedFeedback;
    float quadratureCurrent;
    uint32_t positionFeedback;
};

// Handles input speed and adjusts it to correct range
int clampDriveRequest(int speedReference) {
    if (speedReference > 4000) {
        return 4000;
    } else if (speedReference < -4000) {
        return -4000;
    }
    return speedReference;
}

// Reads Ethernet message and sets motor points in array
void handleDriveRequest(DrivetrainRequest newMessage) {
    motorSetpoints[MOTOR_IDS[0]] = clampDriveRequest(message.m1Setpoint);
    motorSetpoints[MOTOR_IDS[1]] = clampDriveRequest(message.m2Setpoint);
    motorSetpoints[MOTOR_IDS[2]] = clampDriveRequest(message.m3Setpoint);
    motorSetpoints[MOTOR_IDS[3]] = clampDriveRequest(message.m4Setpoint);
}