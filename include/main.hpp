#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>

#include "EthernetDriver.hpp"
#include "Context.hpp"
#include "pb_encode.h"
#include "pb_decode.h"
#include "urc.pb.h"

void updateNetwork(Context &context);
void setupMotors(Context &context);
void checkRoboClaws(Context &context);

#endif