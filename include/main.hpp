#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>

#include "Context.hpp"
#include "pb_encode.h"
#include "pb_decode.h"
#include "urc.pb.h"

void setFlag();
void updateNetwork(Context &context);

IntervalTimer timer;
volatile bool printFlag;

#endif