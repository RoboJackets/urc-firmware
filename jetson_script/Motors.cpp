#include "Motors.h" // Includes all essential libraries for debugging and running motors

// Pins for each status light may not be the same, change if needed
const int GREEN_PIN = 32;
const int BLUE_PIN = 30;
const int RED_PIN = 31;

const int BLINK_RATE_MS = 500;
const int CAN_READ_RATE_MS = 30;
const int UDP_WRITE_RATE_MS = 50;
const int BAUD_RATE = 500000;
const int NUM_MOTORS = 4;
const int PORT = 8443;

// Need to match the IDs assigned via Motion Terminal
/* NOTE: As of 1/21/2026, there have been issues with testing motors on Motion Terminal
    due to firmware update issues. Will have to check motors with traditional firmware code
    or potentially using new SOLOs with updated firmware*/
const int MOTOR_IDS[NUM_MOTORS] = {0xA1, 0xA2, 0xA3, 0xA4};

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

// Format of status light's data
struct Status_Light_Data {
    uint8_t enabled;
    uint8_t blink;
};

// Control status light (but cannot link the status light)
void handleLEDRequest(NewStatusLightCommand newMessage) {
    if (newMessage.redEnabled == 0) {
        digitalWrite(RED_PIN, LOW);
    } else {
        digitalWrite(RED_PIN, HIGH);
    }

    if (newMessage.blueEnabled == 0) {
        digitalWrite(BLUE_PIN, LOW);
    } else {
        digitalWrite(BLUE_PIN, HIGH);
    }

    if (newMessage.greeEnabled == 0) {
        digitalWrite(GREEN_PIN, LOW);
    } else {
        digitalWrite(GREEN_PIN, HIGH);
    }
}

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