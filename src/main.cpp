#include <Arduino.h>

#include "pb_encode.h"
#include "pb_decode.h"
#include "urc.pb.h"

#include <SoftwareSerial.h>
#include "RoboClaw.h"

#include <SOLOMotorControllersUart.h> 

#define address 0x80

//Velocity PID coefficients.
#define Kp 1.0
#define Ki 0.5
#define Kd 0.25
#define qpps 44000

// SOLOMotorControllers *SOLO_Obj1; 

RoboClaw roboclaw(&Serial2,10000);

void roboclawTest();
void nanopbTest();

void setup() {

  // int SOLOdeviceAddress = 0;  //Device adress of SOLO (default vale in SOLO units is 0)
  // Stream &serialSelected = Serial3; // Serial port that Arduino will use (Arduino 1 have only Serial, but other version have more like Serial1)
  // SOLOMotorControllers::UartBaudrate baudrate = SOLOMotorControllers::UartBaudrate::rate115200;  // Boudrate Arduino will use for the comunication (SOLO neet the same value, is possible to set in the Motion Terminal) 
  // long millisecondsTimeout = 200; //Timout in milliseconds before interrupt the wait
  // int packetFailureTrialAttempts = 5; //How many time will happen a try on the same comunication if an error heppen 
  // SOLO_Obj1 = new SOLOMotorControllersUart(SOLOdeviceAddress, serialSelected, baudrate, millisecondsTimeout, packetFailureTrialAttempts);

  nanopbTest(); 
  roboclawTest();
}

void nanopbTest() {
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

void roboclawTest() {
  roboclaw.begin(38400);
  roboclaw.SetM1VelocityPID(address,Kd,Kp,Ki,qpps);
  roboclaw.SetM2VelocityPID(address,Kd,Kp,Ki,qpps);  
  return;
}

void loop() {
  // put your main code here, to run repeatedly:
}