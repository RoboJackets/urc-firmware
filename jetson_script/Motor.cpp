#include "Motor.h"

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
