#ifndef ETHERNETDRIVER_HPP
#define ETHERNETDRIVER_HPP

#include <Arduino.h>
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>

#include "pb_encode.h"
#include "pb_decode.h"
#include "urc.pb.h"

namespace ethernet_driver {

const uint8_t DEFAULT_SERVER_IP_ADDR[] = { 192, 168, 8, 167 };
const uint8_t DEFAULT_CLIENT_IP_ADDR[] = { 192, 168, 8, 255 };
const uint8_t DEFAULT_SERVER_MAC_ADDR[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
const uint16_t DEFAULT_UDP_PORT = 8443;
const uint32_t TIMER_DURATION_MS = 100;

class EthernetDriver {
public:
  
  /// @brief Constructor for Ethernet Driver
  EthernetDriver() : _serverIP(DEFAULT_SERVER_IP_ADDR), _clientIP(DEFAULT_CLIENT_IP_ADDR), _udp(), _port(DEFAULT_UDP_PORT) {}

  /// @brief Constructor for Ethernet Driver
  /// @param serverIP IP address of this microcontroller, seperated by '.'
  /// @param clientIP IP address of main computer, seperated by '.'
  /// @param macAddress MAC address of this microcontroller
  /// @param port Port to send messages on
  EthernetDriver(String serverIP, String clientIP, String macAddress, uint16_t port);

  /// @brief Default destructor
  ~EthernetDriver(){};

  /// @brief Send the string "hello" using UDP
  void sendTestMessage();

  /// @brief Determine if there is a request waiting to be processed. Must be called before receiveRequest()
  /// @return true if request is ready, false otherwise
  bool requestReady();

  /// @brief Read an incoming request. Must be called after requestReady()
  /// @param requestMessage protobuf message to read data into
  /// @return true on success, false on failure
  bool receiveRequest(RequestMessage requestMessage);

  /// @brief Send a reponse to the target computer.
  /// @param driveEncodersMessage protobuf message
  /// @return true on success, false on failure
  bool sendEncoderMessages(DriveEncodersMessage driveEncodersMessage);

  /// @brief Set IP address of this microcontroller 
  /// @param ip IP address of this microcontroller, seperated by '.'
  void setServerIP(String ip) { _serverIP.fromString(ip); }

  /// @brief Set IP address of this microcontroller 
  /// @param ip Four element array containing IP address of this microcontroller
  void setServerIP(const char *ip) { _serverIP.fromString(ip); }

  /// @brief Set IP address of target computer 
  /// @param ip IP address of target computer, seperated by '.'
  void setClientIP(String ip) { _clientIP.fromString(ip); }

  /// @brief Set IP address of target computer 
  /// @param ip Four element array containing IP address of target computer
  void setClientIP(const char *ip) { _clientIP.fromString(ip); }

  unsigned long getSendTimer() { return _sendTimer; }
  void setSendTimer(unsigned long time) { _sendTimer = time; }

  /// @brief Determine if TIMER_DURATION_MS milliseconds have elapsed since last response was sent
  /// @return true if TIMER_DURATION_MS milliseconds has elapsed, false otherwise
  bool sendTimeHasElapsed() { return _sendTimer >= TIMER_DURATION_MS; }

  /// @brief Reset the timer tracking how long since last response was sent
  void resetSendTimer() { _sendTimer -= TIMER_DURATION_MS; }

private:
  IPAddress _clientIP;
  IPAddress _serverIP;
  EthernetUDP _udp;
  uint16_t _port;
  static elapsedMillis _sendTimer;
};

}  // namespace ethernet_driver

#endif