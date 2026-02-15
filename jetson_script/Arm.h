#ifndef ARM_H
#define ARM_H

/**************************GENERAL LIBRARIES**************************/
/*******************TO BE UTILIZED FOR ALL MOTORS*******************/

#include <Arduino.h>
#include <FlexCAN_T4.h> // CAN enabling tool
#include "SoloCAN.hpp" // SOLO UNO motor controller's library
#include <QNEthernet.h> // Teensy's ethernet library
#include <map>

#include "urc.ph.h" // Protobuf header file for compiling code
#include "Messages.hpp" // Protobuff decode functions (encodeResponse, decodeResponse)

#include "pb_encode.h" // Protobuf encode functions
#include "pb_decode.h" // Protobuf decode functions

// NEED TO INCLUDE NEW METHODS FOR CONTROLLING ARM MOTORS

#endif