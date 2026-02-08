/*******************************************************************************
* Copyright 2017 ROBOTIS CO., LTD.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

/*Instead of using Roboclaw controller, we will be shifting to using either
ODrive or Solo. As of 2/8/26, no specific decision has been made yet*/

#include "Arm.h"
#include "MotorControl.h"

const long SERIAL_BAUD_RATE = 38400;
const uint8_t RUN_CURRENT_PERCENT = 100;
const uint8_t HOLD_CURRENT_STANDSTILL = 0;

const int32_t RUN_VELOCITY = 40000;
const int32_t STOP_VELOCITY = 0;

const long SERIAL_BAUD_RATE = 38400;
const uint8_t RUN_CURRENT_PERCENT = 100;
const uint8_t HOLD_CURRENT_STANDSTILL = 0;

// variables
qindesign::network::EthernetUDP udp;
TMC2209 stepper_driver;
DriveEncodersMessage requestMessage;
ArmEffortRequest armEffortRequest;
ArmPositionFeedback armPositionFeedback;

// timer variables
elapsedMillis blinkTimer;
elapsedMillis stepperUpdateTimer;
elapsedMillis motorUpdateTimer;

std::vector<int>::iterator mySpeed;
std::vector<int> stepperSpeeds;

std::unordered_map<int, int> lastCommand;

IPAddress remoteIP;


int main() {
  /*********************START OF URC-FIRMWARE MAIN FUNCTION**********************/
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
        }

        // servo expects 50Hz signal, where HIGH time is between 500us and 2500us
        // 1500 us = servo stopped
        // 1,000,000us / 50 = 20,000 us period
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