#ifndef MAIN_HPP
#define MAIN_HPP

#include <Arduino.h>
#include <string.h>

#include "EthernetDriver.hpp"
#include "Context.hpp"
#include "Messages.hpp"

struct RoboClawData {
  uint8_t wheelSpeed;
  bool valid;
};

void setupRoboClaw(Context &context);
void setupNetwork(Context &context);
void updateNetwork(Context &context);
void updateRoboClaw(Context &context);

#endif