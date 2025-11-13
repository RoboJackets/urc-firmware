#include <RoboClaw.h>
#include "urc.ph.h"

constexpr int DRILL_ENABLE_PIN = 29;
constexpr int DRILL_SIGNAL_1_PIN = 25;
constexpr int DRILL_SIGNAL_2_PIN = 24;
constexpr int ROBOCLAW_DRILL_ADDR = 0x80;

TMC2209 stepperDrill;
RoboClaw roboclaw(&Serial6, 38400);

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

// pin setup
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(STEPPER_1_ENABLE_PIN, OUTPUT);
    digitalWrite(STEPPER_1_ENABLE_PIN, LOW);
    pinMode(STEPPER_2_ENABLE_PIN, OUTPUT);
    digitalWrite(STEPPER_2_ENABLE_PIN, LOW);

    pinMode(DRILL_ENABLE_PIN, OUTPUT);
    pinMode(DRILL_SIGNAL_1_PIN, OUTPUT);
    pinMode(DRILL_SIGNAL_2_PIN, OUTPUT);

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

    while (true) {

        // receive UDP packets
        requestLength = udp.parsePacket();
        if (udp.available()) {

            memset(requestBuffer, 0, 256);
            udp.readBytes(requestBuffer, requestLength);
            remoteIP = udp.remoteIP();
            protobuf::Messages::decodeRequest(requestBuffer, requestLength, scienceMotorRequest);

            Serial.print("Packet received: ");
            Serial.print("drillEffort ");
            Serial.print(scienceMotorRequest.drillEffort);
            Serial.print(" hasDE ");
            Serial.print(scienceMotorRequest.has_drillEffort);
            Serial.print(" hasLSV ");
            Serial.print(scienceMotorRequest.has_leadscrewVel);
            Serial.print(" hasTTV ");
            Serial.print(scienceMotorRequest.has_turntableVel);
            Serial.print(" LSV ");
            Serial.print(scienceMotorRequest.leadscrewVel);
            Serial.print(" TTV ");
            Serial.print(scienceMotorRequest.turntableVel);
            Serial.println(" ");
        }

        // steppers
        if (stepperUpdateTimer >= STEPPER_UPDATE_RATE_MS) {
            stepperUpdateTimer -= STEPPER_UPDATE_RATE_MS;

            run_turntable_stepper(scienceMotorRequest.turntableVel);
            run_drill_stepper(scienceMotorRequest.leadscrewVel);
            run_roboclaw_effort(ROBOCLAW_DRILL_ADDR, scienceMotorRequest.drillEffort);
        }

        // blink
        if (blinkTimer >= BLINK_RATE_MS) {
            blinkTimer -= BLINK_RATE_MS;
            digitalToggle(LED_BUILTIN);
        }
    }