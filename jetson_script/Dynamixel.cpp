#include "Dynamixel.h"

// Global SDK instances used by all helper functions
static dynamixel::PortHandler   *portHandler  = nullptr;
static dynamixel::PacketHandler *packetHandler = nullptr;
static int     dxl_comm_result = COMM_TX_FAIL;
static uint8_t dxl_error       = 0;


void init(const char *deviceName, int baudrate) {
    portHandler  = dynamixel::PortHandler::getPortHandler(deviceName);
    packetHandler = dynamixel::PacketHandler::getPacketHandler(PROTOCOL_VERSION);
    portHandler->openPort();
    portHandler->setBaudRate(baudrate);
}

void enableTorque(uint8_t id) {
    dxl_comm_result = packetHandler->write1ByteTxRx(portHandler, id, ADDR_TORQUE_ENABLE, TORQUE_ENABLE, &dxl_error);
    printResult(dxl_comm_result);
}

void disableTorque(uint8_t id) {
    dxl_comm_result = packetHandler->write1ByteTxRx(portHandler, id, ADDR_TORQUE_ENABLE, TORQUE_DISABLE, &dxl_error);
    printResult(dxl_comm_result);
}

void setSpeed(uint8_t id, int32_t velocity) {
    dxl_comm_result = packetHandler->write4ByteTxRx(portHandler, id, ADDR_GOAL_VELOCITY, velocity, &dxl_error);
    printResult(dxl_comm_result);
}

int32_t getSpeed(uint8_t id) {
    int32_t velocity = 0;
    dxl_comm_result = packetHandler->read4ByteTxRx(portHandler, id, ADDR_PRESENT_VELOCITY, (uint32_t*)&velocity, &dxl_error);
    printResult(dxl_comm_result);
}

void setPosition(uint8_t id, int32_t position) {
    dxl_comm_result = packetHandler->write4ByteTxRx(portHandler, id, ADDR_GOAL_POSITION, position, &dxl_error);
    printResult(dxl_comm_result);
}

int32_t getPosition(uint8_t id) {
    int32_t position = 0;
    dxl_comm_result = packetHandler->read4ByteTxRx(portHandler, id, ADDR_PRESENT_POSITION, (uint32_t*)&position, &dxl_error);
    printResult(dxl_comm_result);
    return position;
}

void setEffort(uint8_t id, int16_t current) {
    dxl_comm_result = packetHandler->write2ByteTxRx(portHandler, id, ADDR_GOAL_CURRENT, current, &dxl_error);
    printResult(dxl_comm_result);
}

int16_t getEffort(uint8_t id) {
    int16_t current = 0;
    dxl_comm_result = packetHandler->read2ByteTxRx(portHandler, id, ADDR_PRESENT_CURRENT, (uint16_t*)&current, &dxl_error);
    printResult(dxl_comm_result);
    return current;
}

void printResult(int dxl_comm_result) {
    if (DEBUG == 0) 
        return;
    if (dxl_comm_result != COMM_SUCCESS)
        printf("%s\n", packetHandler->getTxRxResult(dxl_comm_result));
    else if (dxl_error != 0)
        printf("%s\n", packetHandler->getRxPacketError(dxl_error));
}

void stop(uint8_t id) {
    disableTorque(id);
}
