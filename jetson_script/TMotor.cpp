#include "TMotor.h"

// Global CAN bus and SOLO controller instances
static FlexCAN_T4<CAN3, RX_SIZE_16, TX_SIZE_16> can3;
static solo_can::SoloCan<CAN3, RX_SIZE_16, TX_SIZE_16> solo(can3);

static const long CAN_READ_TIMEOUT_MS = 100;

// SOLO SDO response CAN ID offset (CANopen spec)
static const uint16_t SDO_RESPONSE_BASE = 0x0580;

void init(long baudrate) {
    can3.begin();
    can3.setBaudRate(baudrate);
}

void enableMotor(uint8_t id) {
    solo.SetSpeedReferenceCommand(id, 0, false);
}

void disableMotor(uint8_t id) {
    solo.SetSpeedReferenceCommand(id, 0, false);
    solo.SetTorqueReferenceCommand(id, 0, false);
}

// Helper: send a read request and wait for the SOLO SDO response
static uint32_t readSOLOValue(uint8_t id, uint16_t code) {
    solo_can::CanOpenData req = {
        .id      = (uint16_t)(0x0600 + id),
        .type    = solo_can::SDO_READ_COMMAND,
        .code    = code,
        .payload = 0
    };
    can3.write(solo_can::createMessage(req));

    CAN_message_t msg;
    unsigned long start = millis();
    while (millis() - start < CAN_READ_TIMEOUT_MS) {
        if (can3.read(msg)) {
            auto data = solo_can::parseMessage(msg);
            if (data.id == (uint16_t)(SDO_RESPONSE_BASE + id) && data.code == code) {
                return data.payload;
            }
        }
    }
    return 0;
}

void setSpeed(uint8_t id, int rpm) {
    solo.SetSpeedReferenceCommand(id, rpm, false);
}

long getSpeed(uint8_t id) {
    return (long)readSOLOValue(id, solo_can::SPEED_FEEDBACK_CODE);
}

void setPosition(uint8_t id, long counts) {
    // Position reference SDO (object 0x301B, not in SoloCAN constants)
    solo_can::CanOpenData data = {
        .id      = (uint16_t)(0x0600 + id),
        .type    = solo_can::SDO_WRITE_COMMAND,
        .code    = 0x301B,
        .payload = (uint32_t)counts
    };
    can3.write(solo_can::createMessage(data));
}

long getPosition(uint8_t id) {
    return (long)readSOLOValue(id, solo_can::POSITION_FEEDBACK_CODE);
}

void setEffort(uint8_t id, int effort) {
    solo.SetTorqueReferenceCommand(id, effort, false);
}

float getEffort(uint8_t id) {
    uint32_t raw = readSOLOValue(id, solo_can::QUADRATURE_CURRENT_FEEDBACK_CODE);
    return solo_can::toFloat(raw);
}

void stop(uint8_t id) {
    disableMotor(id);
}
