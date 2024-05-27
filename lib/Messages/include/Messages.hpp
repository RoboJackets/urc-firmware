#ifndef MESSAGES_HPP
#define MESSAGES_HPP

#include <Arduino.h>

#include "pb_encode.h"
#include "pb_decode.h"
#include "urc.pb.h"

namespace protobuf {

class Messages {

public:

  // static bool decode(int messageType, )

  static bool decodeStatusLightRequest(uint8_t *buffer, size_t bufferLen, StatusLightCommand &requestMessage) {
    pb_istream_t istream = pb_istream_from_buffer(buffer, bufferLen);
    return pb_decode(&istream, StatusLightCommand_fields, &requestMessage);
  }

  static bool decodeRequest(uint8_t *buffer, size_t bufferLen, RequestMessage &requestMessage) {
    pb_istream_t istream = pb_istream_from_buffer(buffer, bufferLen);
    return pb_decode(&istream, RequestMessage_fields, &requestMessage);
  }

  static bool decodeRequest(uint8_t *buffer, size_t bufferLen, DriveEncodersMessage &requestMessage) {
    pb_istream_t istream = pb_istream_from_buffer(buffer, bufferLen);
    return pb_decode(&istream, DriveEncodersMessage_fields, &requestMessage);
  }

  static bool decodeRequest(uint8_t *buffer, size_t bufferLen, ArmEffortRequest &requestMessage) {
    pb_istream_t istream = pb_istream_from_buffer(buffer, bufferLen);
    return pb_decode(&istream, ArmEffortRequest_fields, &requestMessage);
  }

  static bool decodeRequest(uint8_t *buffer, size_t bufferLen, ScienceMotorRequest &requestMessage) {
    pb_istream_t istream = pb_istream_from_buffer(buffer, bufferLen);
    return pb_decode(&istream, ScienceMotorRequest_fields, &requestMessage);
  }

  static size_t encodeResponse(uint8_t *buffer, size_t bufferLen, DriveEncodersMessage &driveEncodersMessage) {
    pb_ostream_t ostream = pb_ostream_from_buffer(buffer, bufferLen);
    pb_encode(&ostream, DriveEncodersMessage_fields, &driveEncodersMessage);
    return ostream.bytes_written;
  }

  static size_t encodeResponse(uint8_t *buffer, size_t bufferLen, ArmPositionFeedback &message) {
    pb_ostream_t ostream = pb_ostream_from_buffer(buffer, bufferLen);
    pb_encode(&ostream, ArmPositionFeedback_fields, &message);
    return ostream.bytes_written;
  }
};

}  // namespace protobuf

#endif