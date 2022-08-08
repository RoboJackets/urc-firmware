#include <Arduino.h>
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>

#include "pb_encode.h"
#include "pb_decode.h"
#include "urc.pb.h"

/* Networking Global Variables */
static IPAddress g_ip(192, 168, 8, 255);
static EthernetUDP g_udp;
static volatile bool g_print_flag = false;
static const byte g_mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
static const uint16_t UDP_PORT = 8888;

/* Encoder Global Variables */
static IntervalTimer g_encoder_message_timer;
static elapsedMillis g_elapsedTime = elapsedMillis();

/* Function Declarations */
void send_encoder_estimates(DriveEncodersMessage driveEncodersMessage);
void update_flag(void);
void connect_udp(void);

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

void connect_udp(void) {
  /* Start Ethernet */
  Ethernet.begin(g_mac, g_ip);

  /* Open serial communications and wait for port to open: */
  Serial.begin(9600);
  while (!Serial) {
    delay(10); /* wait for serial port to connect. Needed for native USB port only */
  }

  /* Check for Ethernet hardware present */
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found. Sorry, can't run without hardware. :(");
    while (true) {
      delay(10); /* do nothing, no point running without Ethernet hardware */
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  /* start UDP  */
  g_udp.begin(UDP_PORT);
}

void update_flag(void) {
  g_print_flag = true;
}

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