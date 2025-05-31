#include <Arduino.h>
#include <TMC2209.h>
#include <QNEthernet.h>
#include <vector>
#include <RoboClaw.h>
#include "urc.pb.h"
#include "Messages.hpp"

// constants
constexpr int BLINK_RATE_MS = 500;
constexpr int STEPPER_UPDATE_RATE_MS = 100;
constexpr int DRILL_UPDATE_RATE_MS = 1; 
constexpr int PORT = 8445;
constexpr int STEPPER_1_ENABLE_PIN = 2;
constexpr int STEPPER_2_ENABLE_PIN = 9;
constexpr int DRILL_ENABLE_PIN = 29;
constexpr int DRILL_SIGNAL_1_PIN = 25;
constexpr int DRILL_SIGNAL_2_PIN = 24;
constexpr int ROBOCLAW_DRILL_ADDR = 0x80;
constexpr int PWM_PERIOD_MS = 20;  // 50Hz PWM

constexpr int STEPPER_SERIAL_BAUD_RATE = 115200;
const uint8_t RUN_CURRENT_PERCENT = 100;
const uint8_t HOLD_CURRENT_STANDSTILL = 0;
const int32_t RUN_VELOCITY = 40000;
const int32_t STOP_VELOCITY = 0;
const int STOP_DURATION = 1000;

// variables
qindesign::network::EthernetUDP udp;
IPAddress remoteIP;
TMC2209 stepperDrill;
TMC2209 stepperTurntable;
RoboClaw roboclaw(&Serial7, 38400);
ScienceMotorRequest scienceMotorRequest;
std::vector<int> stepperSpeeds;
std::vector<int>::iterator mySpeed;
int pwmOnDurationMs = 10;          // this will change based on speed
int currentSpeed = 50;              // latest speed command (-100 to +100)
bool roboclaw_pwm_high = true;

// timer variables
elapsedMillis blinkTimer;
elapsedMillis stepperUpdateTimer;
elapsedMillis drillUpdateTimer;
elapsedMillis roboclawPwmTimer;

// functions
void setup_test_stepper();
void test_stepper();
void run_stepper(TMC2209 &driver, int speed);
void run_turntable_stepper(int speed);
void run_drill_stepper(int speed);
void run_roboclaw_effort(int address, int effort);
void run_roboclaw_simple_serial(int address, int effort);
void run_roboclaw_simple_serial_pwm(int speedPercent);

int main() {
    // pin setup
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(STEPPER_1_ENABLE_PIN, OUTPUT);
    digitalWrite(STEPPER_1_ENABLE_PIN, LOW);
    pinMode(STEPPER_2_ENABLE_PIN, OUTPUT);
    digitalWrite(STEPPER_2_ENABLE_PIN, LOW);

    pinMode(DRILL_ENABLE_PIN, OUTPUT);
    pinMode(DRILL_SIGNAL_1_PIN, OUTPUT);
    pinMode(DRILL_SIGNAL_2_PIN, OUTPUT);
    // analogWrite(DRILL_ENABLE_PIN, 100);
    // digitalWrite(DRILL_SIGNAL_1_PIN, LOW);
    // digitalWrite(DRILL_SIGNAL_2_PIN, HIGH);

     // Ethernet setup
    // IPAddress ip(192, 168, 1, 180);
    // IPAddress gateway(192, 168, 1, 1);
    // IPAddress subnet(255, 255, 0, 0);
    // IPAddress dns(192, 168, 1, 1);

    constexpr char hostName[]{"Science_Teensy"};
    qindesign::network::Ethernet.setHostname(hostName);
    qindesign::network::Ethernet.begin();
    udp.begin(PORT);
    scienceMotorRequest = ScienceMotorRequest_init_zero;
    uint8_t requestBuffer[256];
    size_t requestLength;

    // Stepper setup
    Serial1.begin(STEPPER_SERIAL_BAUD_RATE);
    stepperDrill.setup(Serial1);
    stepperDrill.setRunCurrent(RUN_CURRENT_PERCENT);
    stepperDrill.setHoldCurrent(HOLD_CURRENT_STANDSTILL);
    stepperDrill.enable();
    stepperDrill.moveAtVelocity(STOP_VELOCITY);
    delay(STOP_DURATION);
    stepperDrill.moveAtVelocity(RUN_VELOCITY);

    Serial2.begin(STEPPER_SERIAL_BAUD_RATE);
    stepperTurntable.setup(Serial2);
    stepperTurntable.setRunCurrent(RUN_CURRENT_PERCENT);
    stepperTurntable.setHoldCurrent(HOLD_CURRENT_STANDSTILL);
    stepperTurntable.enable();
    stepperTurntable.moveAtVelocity(STOP_VELOCITY);
    delay(STOP_DURATION);
    stepperTurntable.moveAtVelocity(RUN_VELOCITY);

    // Roboclaw setup
    roboclaw.begin(38400);

    // test setup
    setup_test_stepper();

    Serial7.begin(38400);

    while (true) {

        // receive UDP packets
        requestLength = udp.parsePacket();
        if (udp.available()) { 

            memset(requestBuffer, 0, 256);
            udp.readBytes(requestBuffer, requestLength);
            remoteIP = udp.remoteIP();
            protobuf::Messages::decodeRequest(requestBuffer, requestLength, scienceMotorRequest);

            Serial.print("Packet received: ");
            // Serial.print("drillEffort ");
            // Serial.print(scienceMotorRequest.drillEffort);
            // Serial.print(" hasDE ");
            // Serial.print(scienceMotorRequest.has_drillEffort);
            // Serial.print(" hasLSV ");
            // Serial.print(scienceMotorRequest.has_leadscrewVel);
            // Serial.print(" hasTTV ");
            // Serial.print(scienceMotorRequest.has_turntableVel);
            // Serial.print(" LSV ");
            // Serial.print(scienceMotorRequest.leadscrewVel);
            // Serial.print(" TTV ");
            // Serial.print(scienceMotorRequest.turntableVel);
            // Serial.println(" ");
            //
            // currentSpeed = scienceMotorRequest.drillEffort;
        }


        // steppers
        if (stepperUpdateTimer >= STEPPER_UPDATE_RATE_MS) {
            stepperUpdateTimer -= STEPPER_UPDATE_RATE_MS;

            // test_stepper();
            // Serial.print("turntableVel = ");
            // Serial.println(scienceMotorRequest.turntableVel);

            run_turntable_stepper(scienceMotorRequest.turntableVel);
            run_drill_stepper(scienceMotorRequest.leadscrewVel);

            // run_turntable_stepper(300);
            
            // if (scienceMotorRequest.drillEffort >= 0) {
            //     digitalWrite(DRILL_SIGNAL_1_PIN, LOW);
            //     digitalWrite(DRILL_SIGNAL_2_PIN, HIGH);
            //     analogWrite(DRILL_ENABLE_PIN, scienceMotorRequest.drillEffort);
            // }  else {
            //     digitalWrite(DRILL_SIGNAL_1_PIN, HIGH);
            //     digitalWrite(DRILL_SIGNAL_2_PIN, LOW);
            //     analogWrite(DRILL_ENABLE_PIN, abs(scienceMotorRequest.drillEffort));
            // }
        }
        
        // run_roboclaw_simple_serial(0, 10);
        // if (roboclawPwmTimer >= PWM_PERIOD_MS) {
        //     roboclawPwmTimer -= PWM_PERIOD_MS;
        //
        //     if (pwmOnDurationMs > 0) {
        //         // Turn motor ON
        //         if (currentSpeed > 0) {
        //             Serial7.write(127); // Forward
        //             digitalWrite(LED_BUILTIN, HIGH);
        //         } else if (currentSpeed < 0) {
        //             Serial7.write(1);   // Reverse
        //             digitalWrite(LED_BUILTIN, HIGH);
        //         } else {
        //             Serial7.write(64);  // Stop
        //             digitalWrite(LED_BUILTIN, LOW);
        //         }
        //     } else {
        //         Serial7.write(64); // Stop if 0 speed
        //         digitalWrite(LED_BUILTIN, LOW);
        //     }
        // } else if (roboclawPwmTimer >= pwmOnDurationMs) {
        //     Serial7.write(64); // Turn motor OFF (stop) for rest of period
        // }
        //

        currentSpeed = scienceMotorRequest.drillEffort;
        if (roboclawPwmTimer >= pwmOnDurationMs) {
            roboclawPwmTimer -= pwmOnDurationMs;

            if (roboclaw_pwm_high) {
                pwmOnDurationMs = PWM_PERIOD_MS - abs(pwmOnDurationMs);
                roboclaw_pwm_high = false;
                Serial7.write(64);
            }
            else {
                run_roboclaw_simple_serial_pwm(currentSpeed);
                roboclaw_pwm_high = true;
                if (currentSpeed > 0)
                    Serial7.write(127);
                else
                    Serial7.write(1);
            }
        }

        // blink
        if (blinkTimer >= BLINK_RATE_MS) {
            blinkTimer -= BLINK_RATE_MS;
            digitalToggle(LED_BUILTIN);
        }
    }
}

void run_turntable_stepper(int speed) {
    Serial.print("speed = ");
    Serial.println(speed);
    if (speed < 0) {
        stepperTurntable.disableInverseMotorDirection();
    } else {
        stepperTurntable.enableInverseMotorDirection();
    }
    
    int run_speed = abs(speed) * 60;

    if (run_speed >= 1000) {
        stepperTurntable.moveAtVelocity(run_speed);
        stepperTurntable.enable();
    }  else {
        stepperTurntable.disable();
    }
}

void run_drill_stepper(int speed) {
    if (speed < 0) {
        stepperDrill.disableInverseMotorDirection();
    } else {
        stepperDrill.enableInverseMotorDirection();
    }
    
    int run_speed = abs(speed) * 50;

    if (run_speed >= 1000) {
        stepperDrill.moveAtVelocity(run_speed);
        stepperDrill.enable();
    }  else {
        stepperDrill.disable();
    }
}

void setup_test_stepper() {
    stepperSpeeds.push_back(0);
    stepperSpeeds.push_back(-1000);
    stepperSpeeds.push_back(0);
    stepperSpeeds.push_back(-4000);
    stepperSpeeds.push_back(0);
    stepperSpeeds.push_back(-8000);
    stepperSpeeds.push_back(1000);
    stepperSpeeds.push_back(0);
    stepperSpeeds.push_back(4000);
    stepperSpeeds.push_back(8000);

    mySpeed = stepperSpeeds.begin();
}

void test_stepper() {
    if (*mySpeed < 0) {
        stepperDrill.disableInverseMotorDirection();
        stepperTurntable.disableInverseMotorDirection();
    } else {
        stepperDrill.enableInverseMotorDirection();
        stepperTurntable.enableInverseMotorDirection();
    }
    
    int run_speed = abs(*mySpeed) * 10;
    stepperDrill.moveAtVelocity(run_speed);
    stepperDrill.enable();
    stepperTurntable.moveAtVelocity(run_speed / 5);
    stepperTurntable.enable();

    mySpeed++;
    if (mySpeed == stepperSpeeds.end()) {
        mySpeed = stepperSpeeds.begin();
    }
}


void run_roboclaw_effort(int address, int effort) {
    

    // int hash = address * 10 + channel;
    // if (lastCommand.count(hash) > 0 && lastCommand[hash] == effort) return;
    // lastCommand[hash] = effort;
    
    uint8_t addr = address;
    bool isReversed = (effort < 0); 
    uint8_t requestedSpeed = abs(effort);

    if (isReversed) {
        roboclaw.BackwardM1(addr, requestedSpeed);
    } else {
        roboclaw.ForwardM1(addr, requestedSpeed);
    }
    
}

void run_roboclaw_simple_serial(int address, int speed) {
    uint8_t addr = address;
    bool isReversed = (speed < 0); 

    if (speed == 0) {
        Serial7.write(64);
    }
    if (isReversed) {
        Serial7.write(1);
    } else {
        Serial7.write(127);
    }

}


void run_roboclaw_simple_serial_pwm(int speedPercent) {
    currentSpeed = constrain(speedPercent, -100, 100);

    // convert speed percent to PWM duty cycle
    pwmOnDurationMs = map(abs(currentSpeed), 0, 100, 0, PWM_PERIOD_MS);
}
