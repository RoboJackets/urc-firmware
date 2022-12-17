#ifndef MAIN_HPP
#define MAIN_HPP

#include <Arduino.h>

#include "EthernetDriver.hpp"
#include "Context.hpp"
#include "pb_encode.h"
#include "pb_decode.h"
#include "urc.pb.h"


struct RoboClawData {
    uint8_t wheelSpeed;
    bool valid;
};

void setupRoboClaw(Context &context);
void updateNetwork(Context &context);
void updateRoboClaw(Context &context);

#endif