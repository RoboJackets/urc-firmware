# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: urc.proto
# Protobuf Python Version: 4.25.3
"""Generated protocol buffer code."""
from google.protobuf import descriptor as _descriptor
from google.protobuf import descriptor_pool as _descriptor_pool
from google.protobuf import symbol_database as _symbol_database
from google.protobuf.internal import builder as _builder
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()




DESCRIPTOR = _descriptor_pool.Default().AddSerializedFile(b'\n\turc.proto\"\xae\x01\n\x12\x41rmEncodersMessage\x12\x19\n\x11shoulderLiftTicks\x18\x01 \x02(\x05\x12\x19\n\x11shouldSwivelTicks\x18\x02 \x02(\x05\x12\x16\n\x0e\x65lbowLiftTicks\x18\x03 \x02(\x05\x12\x18\n\x10\x65lbowSwivelTicks\x18\x04 \x02(\x05\x12\x16\n\x0ewristLiftTicks\x18\x05 \x02(\x05\x12\x18\n\x10wristSwivelTicks\x18\x06 \x02(\x05\"P\n\x14\x44riveEncodersMessage\x12\x11\n\tleftSpeed\x18\x01 \x01(\x05\x12\x12\n\nrightSpeed\x18\x02 \x01(\x05\x12\x11\n\ttimestamp\x18\x03 \x02(\x05\"\x8b\x01\n\x11\x44rivetrainRequest\x12\x12\n\nm1Setpoint\x18\x01 \x02(\x05\x12\x12\n\nm2Setpoint\x18\x02 \x02(\x05\x12\x12\n\nm3Setpoint\x18\x03 \x02(\x05\x12\x12\n\nm4Setpoint\x18\x04 \x02(\x05\x12\x12\n\nm5Setpoint\x18\x05 \x02(\x05\x12\x12\n\nm6Setpoint\x18\x06 \x02(\x05\"\xfe\x01\n\x12\x44rivetrainResponse\x12\x12\n\nm1Feedback\x18\x01 \x02(\x05\x12\x11\n\tm1Current\x18\x02 \x02(\r\x12\x12\n\nm2Feedback\x18\x03 \x02(\x05\x12\x11\n\tm2Current\x18\x04 \x02(\r\x12\x12\n\nm3Feedback\x18\x05 \x02(\x05\x12\x11\n\tm3Current\x18\x06 \x02(\r\x12\x12\n\nm4Feedback\x18\x07 \x02(\x05\x12\x11\n\tm4Current\x18\x08 \x02(\r\x12\x12\n\nm5Feedback\x18\t \x02(\x05\x12\x11\n\tm5Current\x18\n \x02(\r\x12\x12\n\nm6Feedback\x18\x0b \x02(\x05\x12\x11\n\tm6Current\x18\x0c \x02(\r\"\x8e\x01\n\x0eRequestMessage\x12\x14\n\x0crequestSpeed\x18\x01 \x02(\x08\x12\x1a\n\x12requestDiagnostics\x18\x02 \x02(\x08\x12\x11\n\tleftSpeed\x18\x03 \x01(\x05\x12\x12\n\nrightSpeed\x18\x04 \x01(\x05\x12\x10\n\x08\x64uration\x18\x05 \x01(\x05\x12\x11\n\ttimestamp\x18\x06 \x02(\x05\"4\n\x12StatusLightCommand\x12\r\n\x05\x63olor\x18\x01 \x01(\x05\x12\x0f\n\x07\x64isplay\x18\x02 \x01(\x05\"\x94\x01\n\rTeensyMessage\x12\x11\n\tmessageID\x18\x01 \x02(\x05\x12\x32\n\x14\x64riveEncodersMessage\x18\x02 \x01(\x0b\x32\x12.DrivetrainRequestH\x00\x12\x31\n\x12statusLightCommand\x18\x03 \x01(\x0b\x32\x13.StatusLightCommandH\x00\x42\t\n\x07payload\"\x8d\x01\n\x15NewStatusLightCommand\x12\x12\n\nredEnabled\x18\x01 \x01(\x05\x12\x13\n\x0b\x62lueEnabled\x18\x02 \x01(\x05\x12\x14\n\x0cgreenEnabled\x18\x03 \x01(\x05\x12\x10\n\x08redBlink\x18\x04 \x01(\x05\x12\x10\n\x08\x62lueBlnk\x18\x05 \x01(\x05\x12\x11\n\tgrenBlink\x18\x06 \x01(\x05\"!\n\x0e\x41rmClawRequest\x12\x0f\n\x07\x63lawVel\x18\x01 \x01(\x05\"\xaa\x01\n\x10\x41rmEffortRequest\x12\x1a\n\x12shoulderLiftEffort\x18\x01 \x01(\x05\x12\x1c\n\x14shoulderSwivelEffort\x18\x02 \x01(\x05\x12\x17\n\x0f\x65lbowLiftEffort\x18\x03 \x01(\x05\x12\x17\n\x0fwristLiftEffort\x18\x04 \x01(\x05\x12\x19\n\x11wristSwivelEffort\x18\x05 \x01(\x05\x12\x0f\n\x07\x63lawVel\x18\x06 \x01(\x05\"\x97\x01\n\x13\x41rmPositionFeedback\x12\x19\n\x11shoulderLiftTicks\x18\x01 \x01(\x05\x12\x1b\n\x13shoulderSwivelTicks\x18\x02 \x01(\x05\x12\x16\n\x0e\x65lbowLiftTicks\x18\x03 \x01(\x05\x12\x16\n\x0ewristLiftTicks\x18\x04 \x01(\x05\x12\x18\n\x10wristSwivelTicks\x18\x05 \x01(\x05\"V\n\x13ScienceMotorRequest\x12\x14\n\x0cleadscrewVel\x18\x01 \x01(\x05\x12\x14\n\x0cturntableVel\x18\x02 \x01(\x05\x12\x13\n\x0b\x64rillEffort\x18\x03 \x01(\x05')

_globals = globals()
_builder.BuildMessageAndEnumDescriptors(DESCRIPTOR, _globals)
_builder.BuildTopDescriptorsAndMessages(DESCRIPTOR, 'urc_pb2', _globals)
if _descriptor._USE_C_DESCRIPTORS == False:
  DESCRIPTOR._options = None
  _globals['_ARMENCODERSMESSAGE']._serialized_start=14
  _globals['_ARMENCODERSMESSAGE']._serialized_end=188
  _globals['_DRIVEENCODERSMESSAGE']._serialized_start=190
  _globals['_DRIVEENCODERSMESSAGE']._serialized_end=270
  _globals['_DRIVETRAINREQUEST']._serialized_start=273
  _globals['_DRIVETRAINREQUEST']._serialized_end=412
  _globals['_DRIVETRAINRESPONSE']._serialized_start=415
  _globals['_DRIVETRAINRESPONSE']._serialized_end=669
  _globals['_REQUESTMESSAGE']._serialized_start=672
  _globals['_REQUESTMESSAGE']._serialized_end=814
  _globals['_STATUSLIGHTCOMMAND']._serialized_start=816
  _globals['_STATUSLIGHTCOMMAND']._serialized_end=868
  _globals['_TEENSYMESSAGE']._serialized_start=871
  _globals['_TEENSYMESSAGE']._serialized_end=1019
  _globals['_NEWSTATUSLIGHTCOMMAND']._serialized_start=1022
  _globals['_NEWSTATUSLIGHTCOMMAND']._serialized_end=1163
  _globals['_ARMCLAWREQUEST']._serialized_start=1165
  _globals['_ARMCLAWREQUEST']._serialized_end=1198
  _globals['_ARMEFFORTREQUEST']._serialized_start=1201
  _globals['_ARMEFFORTREQUEST']._serialized_end=1371
  _globals['_ARMPOSITIONFEEDBACK']._serialized_start=1374
  _globals['_ARMPOSITIONFEEDBACK']._serialized_end=1525
  _globals['_SCIENCEMOTORREQUEST']._serialized_start=1527
  _globals['_SCIENCEMOTORREQUEST']._serialized_end=1613
# @@protoc_insertion_point(module_scope)
