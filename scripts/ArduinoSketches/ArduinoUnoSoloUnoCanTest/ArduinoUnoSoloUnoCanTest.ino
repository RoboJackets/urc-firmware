// EXAMPLE of how read the SOLO board temperature, 
// every second we print the value of the temperature


//Importing SOLO Arduino library
#include "SOLOMotorControllersCanopen.h" 

// Solo Object
SOLOMotorControllers *SOLO_Obj1; 

// Variables
float Temperature=0; 
int fwVersion;
int error;

// definitions
const int SOLOdeviceAddress = 0;  // set CAN ID using Motion Studio
const int chipSelectPin = 10;     // inland CAN shield uses CS pin 10 

void setup() {
  Serial.begin(115200);
  SOLO_Obj1 = new SOLOMotorControllersCanopen(SOLOdeviceAddress, chipSelectPin);  
}
  
void loop() {
  //Reading
  Temperature = SOLO_Obj1->GetBoardTemperature(error);
  fwVersion = SOLO_Obj1->GetDeviceFirmwareVersion(error);

  //Print
  Serial.println("Read from SOLO");
  Serial.println(Temperature,7);
  Serial.println(fwVersion, HEX);
  Serial.println("Error");
  Serial.println(error);

  delay(1000);
}
