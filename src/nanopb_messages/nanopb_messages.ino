#include <pb_decode.h>
#include <pb.h>
#include <pb_encode.h>
#include <pb_common.h>

#include "urc.pb.h"

// 1: Create Nanopb library
// 

// python3 ../../../external/nanopb/generator/nanopb_generator.py -L '#include "%s"' urc.proto
// cp ../../../external/nanopb/pb* .

// Nanopb website: https://jpa.kapsi.fi/nanopb/

void setup() {
  Serial.begin(9600);
  Serial.println("Hello World!");

  // Testing that we can fill out message fields

  ResponseMessage message = ResponseMessage_init_zero;

  /* This is the buffer where we will store the response message. */
  uint8_t responsebuffer[256];
  size_t response_length;
  bool ostatus;
    
    /* Create a stream that will write to our buffer. */
  pb_ostream_t ostream = pb_ostream_from_buffer(responsebuffer, sizeof(responsebuffer));
  
  message.has_voltage = true;
  message.has_speed_l = true;
  
  message.voltage = 12.0;
  message.speed_l = 3.0;

  pb_encode(&ostream, ResponseMessage_fields, &message);
  response_length = ostream.bytes_written;

  Serial.printf("Message length: %d\n", response_length);
}

void loop() {
  

  
}
