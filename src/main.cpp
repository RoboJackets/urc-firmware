#include <Arduino.h>
#include "pb_encode.h"
#include "pb_decode.h"

#include "urc.pb.h"

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
  // put your main code here, to run repeatedly:
}