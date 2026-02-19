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
    motorSetpoints[MOTOR_IDS[0]] = clampDriveRequest(newMessage.m1Setpoint);
    motorSetpoints[MOTOR_IDS[1]] = clampDriveRequest(newMessage.m2Setpoint);
    motorSetpoints[MOTOR_IDS[2]] = clampDriveRequest(newMessage.m3Setpoint);
    motorSetpoints[MOTOR_IDS[3]] = clampDriveRequest(newMessage.m4Setpoint);
}
