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

#include "Arm.h"

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

int main() {
  // Initialize PortHandler instance
  // Set the port path
  // Get methods and members of PortHandlerLinux or PortHandlerWindows
  dynamixel::PortHandler *portHandler = dynamixel::PortHandler::getPortHandler(DEVICENAME);

  // Initialize PacketHandler instance
  // Set the protocol version
  // Get methods and members of Protocol1PacketHandler or Protocol2PacketHandler
  dynamixel::PacketHandler *packetHandler = dynamixel::PacketHandler::getPacketHandler(PROTOCOL_VERSION);

  int index = 0;
  int dxl_comm_result = COMM_TX_FAIL;             // Communication result
  int dxl_goal_position[2] = {MINIMUM_POSITION_LIMIT, MAXIMUM_POSITION_LIMIT};         // Goal position

  uint8_t dxl_error = 0;                          // DYNAMIXEL error
  #if defined(XL320)
  int16_t dxl_present_position = 0;  // XL-320 uses 2 byte Position data
  #else
  int32_t dxl_present_position = 0;  // Read 4 byte Position data
  #endif

  // Open port
  if (portHandler->openPort()) {
    printf("Succeeded to open the port!\n");
  }
  else {
    printf("Failed to open the port!\n");
    printf("Press any key to terminate...\n");
    getch();
    return 0;
  }

  // Set port baudrate
  if (portHandler->setBaudRate(BAUDRATE)) {
    printf("Succeeded to change the baudrate!\n");
  }
  else {
    printf("Failed to change the baudrate!\n");
    printf("Press any key to terminate...\n");
    getch();
    return 0;
  }

  // Enable DYNAMIXEL Torque
  dxl_comm_result = packetHandler->write1ByteTxRx(portHandler, DXL_ID, ADDR_TORQUE_ENABLE, TORQUE_ENABLE, &dxl_error);
  if (dxl_comm_result != COMM_SUCCESS) {
    printf("%s\n", packetHandler->getTxRxResult(dxl_comm_result));
  }
  else if (dxl_error != 0) {
    printf("%s\n", packetHandler->getRxPacketError(dxl_error));
  }
  else {
    printf("Succeeded enabling DYNAMIXEL Torque.\n");
  }

  while(1) {
    printf("Press any key to continue. (Press [ESC] to exit)\n");
    if (getch() == ESC_ASCII_VALUE)
      break;

    // Write goal position
    #if defined(XL320)  // XL-320 uses 2 byte Position data
    dxl_comm_result = packetHandler->write2ByteTxRx(portHandler, DXL_ID, ADDR_GOAL_POSITION, dxl_goal_position[index], &dxl_error);
    #else
    dxl_comm_result = packetHandler->write4ByteTxRx(portHandler, DXL_ID, ADDR_GOAL_POSITION, dxl_goal_position[index], &dxl_error);
    #endif
    if (dxl_comm_result != COMM_SUCCESS) {
      printf("%s\n", packetHandler->getTxRxResult(dxl_comm_result));
    }
    else if (dxl_error != 0) {
      printf("%s\n", packetHandler->getRxPacketError(dxl_error));
    }

    do {
      // Read the Present Position
      #if defined(XL320)  // XL-320 uses 2 byte Position data
      dxl_comm_result = packetHandler->read2ByteTxRx(portHandler, DXL_ID, ADDR_PRESENT_POSITION, (uint16_t*)&dxl_present_position, &dxl_error);
      #else
      dxl_comm_result = packetHandler->read4ByteTxRx(portHandler, DXL_ID, ADDR_PRESENT_POSITION, (uint32_t*)&dxl_present_position, &dxl_error);
      #endif
      if (dxl_comm_result != COMM_SUCCESS) {
        printf("%s\n", packetHandler->getTxRxResult(dxl_comm_result));
      }
      else if (dxl_error != 0) {
        printf("%s\n", packetHandler->getRxPacketError(dxl_error));
      }

      printf("[ID:%03d] Goal Position:%03d  Present Position:%03d\n", DXL_ID, dxl_goal_position[index], dxl_present_position);

    } while((abs(dxl_goal_position[index] - dxl_present_position) > DXL_MOVING_STATUS_THRESHOLD));

    // Switch the Goal Position
    if (index == 0) {
      index = 1;
    }
    else {
      index = 0;
    }
  }

  // Disable DYNAMIXEL Torque
  dxl_comm_result = packetHandler->write1ByteTxRx(portHandler, DXL_ID, ADDR_TORQUE_ENABLE, TORQUE_DISABLE, &dxl_error);
  if (dxl_comm_result != COMM_SUCCESS) {
    printf("%s\n", packetHandler->getTxRxResult(dxl_comm_result));
  }
  else if (dxl_error != 0) {
    printf("%s\n", packetHandler->getRxPacketError(dxl_error));
  }
  else {
    printf("Succeeded disabling DYNAMIXEL Torque.\n");
  }

  // Close port
  portHandler->closePort();
  return 0;

  // START OF URC-FIRMWARE MAIN FUNCTION
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