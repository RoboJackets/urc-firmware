#ifndef MAIN_HPP
#define MAIN_HPP

#include <Arduino.h>

#include "EthernetDriver.hpp"
#include "Context.hpp"
#include "pb_encode.h"
#include "pb_decode.h"
#include "urc.pb.h"

void updateNetwork(Context &context, uint32_t &myTemp);
void setupMotors(Context &context);
void updateRoboClaw(Context &context, uint32_t &myTemp);

#endif