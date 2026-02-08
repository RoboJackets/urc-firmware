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

private:
    Motor* shoulder_swivel;
    Motor* end_effector;

int main() {
    /**********************START OF DYNAMIXEL MAIN FUNCTION**************************/
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
  int dxl_goal_position[2] = {MINIMUM_POSITION_LIMIT, MAXIMUM_POSITION_LIMIT}; // Goal position

  uint8_t dxl_error = 0; // DYNAMIXEL error
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

// Dynamixel-made method
int getch() {
#if defined(__linux__) || defined(__APPLE__)
  struct termios oldt, newt;
  int ch;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  ch = getchar();
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  return ch;
#elif defined(_WIN32) || defined(_WIN64)
  return _getch();
#endif
}

// Dynxamiel-made method
int kbhit(void) {
#if defined(__linux__) || defined(__APPLE__)
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if (ch != EOF) {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
#elif defined(_WIN32) || defined(_WIN64)
  return _kbhit();
#endif
}

void test_stepper() {
    stepper_driver.moveAtVelocity(RUN_VELOCITY);
    stepper_driver.disableInverseMotorDirection();
    stepper_driver.enable();
    delay(RUN_DURATION);
    stepper_driver.disable();
    delay(STOP_DURATION);
    stepper_driver.enableInverseMotorDirection();
    stepper_driver.enable();
    delay(RUN_DURATION);
    stepper_driver.disable();
    delay(STOP_DURATION);
}

void test_stepper_2() {
    // stepper_driver.disable();
    if (*mySpeed < 0) {
        stepper_driver.disableInverseMotorDirection();
    } else {
        stepper_driver.enableInverseMotorDirection();
    }

    int run_speed = abs(*mySpeed) * 10;
    stepper_driver.moveAtVelocity(run_speed);
    stepper_driver.enable();

    mySpeed++;
    if (mySpeed == stepperSpeeds.end()) {
        mySpeed = stepperSpeeds.begin();
    }
}

void run_stepper() {
    // stepper_driver.disable();
    if (requestMessage.leftSpeed < 0) {
        stepper_driver.disableInverseMotorDirection();
    } else {
        stepper_driver.enableInverseMotorDirection();
    }

    int run_speed = abs(requestMessage.leftSpeed) * 50;

    if (run_speed >= 1000) {
        stepper_driver.moveAtVelocity(run_speed);
        stepper_driver.enable();
    }  else {
        stepper_driver.disable();
    }
}

void run_stepper_2() {
    // stepper_driver.disable();
    if (armEffortRequest.clawVel < 0) {
        stepper_driver.disableInverseMotorDirection();
    } else {
        stepper_driver.enableInverseMotorDirection();
    }

    int run_speed = abs(armEffortRequest.clawVel) * 50;

    if (run_speed >= 1000) {
        stepper_driver.moveAtVelocity(run_speed);
        stepper_driver.enable();
    }  else {
        stepper_driver.disable();
    }
}