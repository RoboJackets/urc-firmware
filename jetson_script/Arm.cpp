// Include relevant libraries from StepperArm C++ file
#include <RoboClaw.h>
#include "urc.ph.h"


// Constants relevant to shoulder swivel
constexpr int ROBOCLAW_SHOULDER_ADDR = 0x82;
constexpr int ROBOCLAW_CHANNEL_1 = 1;
constexpr int ROBOCLAW_CHANNEL_2 = 2;

const long SERIAL_BAUD_RATE = 38400;
const uint8_t RUN_CURRENT_PERCENT = 100;
const uint8_t HOLD_CURRENT_STANDSTILL = 0;

const int32_t RUN_VELOCITY = 40000;
const int32_t STOP_VELOCITY = 0;

const long SERIAL_BAUD_RATE = 38400;
const uint8_t RUN_CURRENT_PERCENT = 100;
const uint8_t HOLD_CURRENT_STANDSTILL = 0;

void run_roboclaw_effort(int address, int channel, int effort) {

    int hash = address * 10 + channel;
    if (lastCommand.count(hash) > 0 && lastCommand[hash] == effort) return;
    lastCommand[hash] = effort;

    uint8_t addr = address;
    bool isReversed = (effort < 0);
    uint8_t requestedSpeed = abs(effort);

    if (channel == 1) {
        if (isReversed) {
            roboclaw.BackwardM1(addr, requestedSpeed);
        } else {
            roboclaw.ForwardM1(addr, requestedSpeed);
        }
    } else if (channel == 2) {
        if (isReversed) {
            roboclaw.BackwardM2(addr, requestedSpeed);
        } else {
            roboclaw.ForwardM2(addr, requestedSpeed);
        }
    }
}

int main() {
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(2, OUTPUT);
    pinMode(SERVO_PWM_PIN, OUTPUT);
    digitalWrite(2, LOW);

    // Ethernet setup
    constexpr char hostName[]{"Arm_Teensy"};
    qindesign::network::Ethernet.setHostname(hostName);
    qindesign::network::Ethernet.begin();
    udp.begin(PORT);

    armEffortRequest = ArmEffortRequest_init_zero;
    armPositionFeedback = ArmPositionFeedback_init_zero;
    // requestMessage = DriveEncodersMessage_init_zero;
    uint8_t requestBuffer[256];
    size_t requestLength;

    roboclaw.begin(38400);

    Serial1.begin(SERIAL_BAUD_RATE);
    stepper_driver.setup(Serial1);
    stepper_driver.setRunCurrent(RUN_CURRENT_PERCENT);
    stepper_driver.setHoldCurrent(HOLD_CURRENT_STANDSTILL);
    stepper_driver.enable();
    stepper_driver.moveAtVelocity(STOP_VELOCITY);
    delay(STOP_DURATION);
    stepper_driver.moveAtVelocity(RUN_VELOCITY);

    while (true) {

        // read incoming UDP messages
        requestLength = udp.parsePacket();
        if (udp.available()) {

            Serial.print("Packet received: ");

            memset(requestBuffer, 0, 256);
            udp.readBytes(requestBuffer, requestLength);
            // protobuf::Messages::decodeRequest(requestBuffer, requestLength, requestMessage);
            protobuf::Messages::decodeRequest(requestBuffer, requestLength, armEffortRequest);

            remoteIP = udp.remoteIP();

            // Serial.print("left: ");
            // Serial.print(requestMessage.leftSpeed);
        }

        if (motorUpdateTimer >= MOTOR_UPDATE_RATE) {
            motorUpdateTimer -= MOTOR_UPDATE_RATE;

            //new arm motor effort
            run_roboclaw_effort(ROBOCLAW_ELBOW_ADDR, ROBOCLAW_CHANNEL_1, armEffortRequest.elbowLiftEffort);
            run_roboclaw_effort(ROBOCLAW_ELBOW_ADDR, ROBOCLAW_CHANNEL_2, armEffortRequest.shoulderLiftEffort);
            run_roboclaw_effort(ROBOCLAW_WRIST_ADDR, ROBOCLAW_CHANNEL_1, armEffortRequest.wristSwivelEffort);
            run_roboclaw_effort(ROBOCLAW_WRIST_ADDR, ROBOCLAW_CHANNEL_2, armEffortRequest.wristLiftEffort);
            run_roboclaw_effort(ROBOCLAW_SHOULDER_ADDR, ROBOCLAW_CHANNEL_1, armEffortRequest.shoulderSwivelEffort);
        }

        // servo expects 50Hz signal, where HIGH time is between 500us and 2500us
        // 1500us means servo stopped
        // 1,000,000us / 50 = 20,000us period
        if (servoUpdateTimer >= servo_wait_us) {
            // reset timer
            servoUpdateTimer -= servo_wait_us;

            // high pulse has ended
            if (servo_pwm_high) {
                servo_wait_us = SERVO_PWM_PERIOD_US - abs(servo_wait_us);
                servo_pwm_high = false;
                digitalWrite(SERVO_PWM_PIN, LOW);
            }
            // low pulse has ended
            // update high timer interval from protobuf
            else {
                servo_wait_us = abs(((1000 * static_cast<int>(armEffortRequest.clawVel)) / 600) + SERVO_STOPPED_PULSE_WIDTH);
                servo_pwm_high = true;
                digitalWrite(SERVO_PWM_PIN, HIGH);
            }
        }

        if (blinkTimer >= BLINK_RATE_MS) {
            blinkTimer -= BLINK_RATE_MS;
            digitalToggle(LED_BUILTIN);
        }
    }
}