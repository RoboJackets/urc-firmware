#include "EthernetDriver.h"

#include "pb_encode.h"
#include "pb_decode.h"
#include "urc.pb.h"

// Function Declarations
void send_encoder_estimates(DriveEncodersMessage driveEncodersMessage);

using namespace ethernet_driver;

int main() {

  elapsedMillis elapsedTime;

  Serial.begin(9600);
  Serial.println("Hello World!");

  // Construct UDP instance
  EthernetDriver ethernet_driver;
  ethernet_driver.initDriver();

  while (true) {
    noInterrupts();
    if (ethernet_driver.getPrintFlag()) {
      Serial.println(elapsedTime);
      elapsedTime = 0;

      // g_udp.beginPacket(g_ip, UDP_PORT);
      // g_udp.write("hello");
      // g_udp.endPacket();

      ethernet_driver.resetPrintFlag();
    }
    interrupts();

    delay(5);
  }

  return 0;
}

#if defined(ARDUINO_STYLE_FUNCS)
void setup() {

  Serial.begin(9600);
  Serial.println("Hello World!");

  connect_udp();

  g_encoder_message_timer.begin(update_flag, 100000);
}

void loop() {
#if defined(USING_ENCODER_MESSAGES)
  /* create Nanopb messages */
  DriveEncodersMessage driveEncodersMessage = DriveEncodersMessage_init_zero;

  /* read values from drive motor controllers */
  driveEncodersMessage.frontLeftTicks = 12;
  driveEncodersMessage.frontRightTicks = 15;
  driveEncodersMessage.middleLeftTicks = 22;
  driveEncodersMessage.middleRightTicks = 25;
  driveEncodersMessage.backLeftTicks = 32;
  driveEncodersMessage.backRightTicks = 35;

  sendEncoderEstimates(driveEncodersMessage);
#endif

  /*  Send UDP message
      Disable interrupts to read / write from g_print_flag
  */
  noInterrupts();
  if (g_print_flag) {
    Serial.println(g_elapsedTime);
    g_elapsedTime = 0;

    g_udp.beginPacket(g_ip, UDP_PORT);
    g_udp.write("hello");
    g_udp.endPacket();

    g_print_flag = false;
  }
  interrupts();

  delay(5);
}
#endif

#if defined(USING_ENCODER_MESSAGES)
void send_encoder_estimates(DriveEncodersMessage driveEncodersMessage) {
  uint8_t responsebuffer[256];
  size_t response_length;
  bool ostatus;

  pb_ostream_t ostream = pb_ostream_from_buffer(responsebuffer, sizeof(responsebuffer));
  pb_encode(&ostream, DriveEncodersMessage_fields, &driveEncodersMessage);
  response_length = ostream.bytes_written;
}
#endif