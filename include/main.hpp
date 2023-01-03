#ifndef MAIN_HPP
#define MAIN_HPP

#include <Arduino.h>
#include <string.h>

#include "Context.hpp"
#include "Messages.hpp"
#include "Managers.hpp"

struct RoboClawData {
  uint8_t wheelSpeed;
  bool valid;
};

void setupRoboClaw(Context &context);
void setupNetwork(Context &context);
void setupMotors(Context &context);
void updateNetwork(Context &context);
void updateRoboClaw(Context &context);
void updateMotors(Context &context);

#endif