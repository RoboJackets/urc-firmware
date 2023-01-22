#ifndef MAIN_HPP
#define MAIN_HPP

#include <Arduino.h>
#include <string.h>
#include "Context.hpp"
#include "Messages.hpp"
#include "Managers/EthernetManager.hpp"
#include "Managers/MotorManager.hpp"

// #include "EthernetDriver.hpp"
// #include "pb_encode.h"
// #include "pb_decode.h"
// #include "urc.pb.h"

// void updateNetwork(Context &context);
// void setupMotors(Context &context);
// void checkSoloUnos(Context &context);
void testUpdate();
// void checkRoboClaws(Context &context);

#endif