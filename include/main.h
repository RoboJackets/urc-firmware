#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>

#include "pb_encode.h"
#include "pb_decode.h"
#include "urc.pb.h"

void setFlag();

IntervalTimer timer;
volatile bool printFlag;

#endif