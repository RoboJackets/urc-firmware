# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# NO CHECKED-IN PROTOBUF GENCODE
# source: urc.proto
# Protobuf Python Version: 5.29.3
"""Generated protocol buffer code."""
from google.protobuf import descriptor as _descriptor
from google.protobuf import descriptor_pool as _descriptor_pool
from google.protobuf import runtime_version as _runtime_version
from google.protobuf import symbol_database as _symbol_database
from google.protobuf.internal import builder as _builder
_runtime_version.ValidateProtobufRuntimeVersion(
    _runtime_version.Domain.PUBLIC,
    5,
    29,
    3,
    '',
    'urc.proto'
)
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()




DESCRIPTOR = _descriptor_pool.Default().AddSerializedFile(b'\n\turc.proto\"\xae\x01\n\x12\x41rmEncodersMessage\x12\x19\n\x11shoulderLiftTicks\x18\x01 \x02(\x05\x12\x19\n\x11shouldSwivelTicks\x18\x02 \x02(\x05\x12\x16\n\x0e\x65lbowLiftTicks\x18\x03 \x02(\x05\x12\x18\n\x10\x65lbowSwivelTicks\x18\x04 \x02(\x05\x12\x16\n\x0ewristLiftTicks\x18\x05 \x02(\x05\x12\x18\n\x10wristSwivelTicks\x18\x06 \x02(\x05\"P\n\x14\x44riveEncodersMessage\x12\x11\n\tleftSpeed\x18\x01 \x01(\x05\x12\x12\n\nrightSpeed\x18\x02 \x01(\x05\x12\x11\n\ttimestamp\x18\x03 \x02(\x05\"c\n\x11\x44rivetrainRequest\x12\x12\n\nm1Setpoint\x18\x01 \x02(\x05\x12\x12\n\nm2Setpoint\x18\x02 \x02(\x05\x12\x12\n\nm3Setpoint\x18\x03 \x02(\x05\x12\x12\n\nm4Setpoint\x18\x04 \x02(\x05\"\xb4\x02\n\x12\x44rivetrainResponse\x12\x17\n\x0fm1SpeedFeedback\x18\x01 \x02(\x05\x12\x11\n\tm1Current\x18\x02 \x02(\r\x12\x1a\n\x12m1PositionFeedback\x18\x03 \x02(\x05\x12\x17\n\x0fm2SpeedFeedback\x18\x04 \x02(\x05\x12\x11\n\tm2Current\x18\x05 \x02(\r\x12\x1a\n\x12m2PositionFeedback\x18\x06 \x02(\x05\x12\x17\n\x0fm3SpeedFeedback\x18\x07 \x02(\x05\x12\x11\n\tm3Current\x18\x08 \x02(\r\x12\x1a\n\x12m3PositionFeedback\x18\t \x02(\x05\x12\x17\n\x0fm4SpeedFeedback\x18\n \x02(\x05\x12\x11\n\tm4Current\x18\x0b \x02(\r\x12\x1a\n\x12m4PositionFeedback\x18\x0c \x02(\x05\"\x8e\x01\n\x0eRequestMessage\x12\x14\n\x0crequestSpeed\x18\x01 \x02(\x08\x12\x1a\n\x12requestDiagnostics\x18\x02 \x02(\x08\x12\x11\n\tleftSpeed\x18\x03 \x01(\x05\x12\x12\n\nrightSpeed\x18\x04 \x01(\x05\x12\x10\n\x08\x64uration\x18\x05 \x01(\x05\x12\x11\n\ttimestamp\x18\x06 \x02(\x05\"4\n\x12StatusLightCommand\x12\r\n\x05\x63olor\x18\x01 \x01(\x05\x12\x0f\n\x07\x64isplay\x18\x02 \x01(\x05\"\x8f\x01\n\x15NewStatusLightCommand\x12\x12\n\nredEnabled\x18\x01 \x02(\x05\x12\x13\n\x0b\x62lueEnabled\x18\x02 \x02(\x05\x12\x14\n\x0cgreenEnabled\x18\x03 \x02(\x05\x12\x10\n\x08redBlink\x18\x04 \x02(\x05\x12\x11\n\tblueBlink\x18\x05 \x02(\x05\x12\x12\n\ngreenBlink\x18\x06 \x02(\x05\"\x8c\x01\n\x12StatusLightMessage\x12\x12\n\nredEnabled\x18\x01 \x02(\x05\x12\x13\n\x0b\x62lueEnabled\x18\x02 \x02(\x05\x12\x14\n\x0cgreenEnabled\x18\x03 \x02(\x05\x12\x10\n\x08redBlink\x18\x04 \x02(\x05\x12\x11\n\tblueBlink\x18\x05 \x02(\x05\x12\x12\n\ngreenBlink\x18\x06 \x02(\x05\">\n\x0fSetpointMessage\x12\x14\n\x0cleftSetpoint\x18\x01 \x02(\x05\x12\x15\n\rrightSetpoint\x18\x02 \x02(\x05\"~\n\rTeensyMessage\x12\x31\n\x12statusLightMessage\x18\x01 \x01(\x0b\x32\x13.StatusLightMessageH\x00\x12+\n\x0fsetpointMessage\x18\x02 \x01(\x0b\x32\x10.SetpointMessageH\x00\x42\r\n\x0bmessageType\"!\n\x0e\x41rmClawRequest\x12\x0f\n\x07\x63lawVel\x18\x01 \x01(\x05\"\xaa\x01\n\x10\x41rmEffortRequest\x12\x1a\n\x12shoulderLiftEffort\x18\x01 \x01(\x05\x12\x1c\n\x14shoulderSwivelEffort\x18\x02 \x01(\x05\x12\x17\n\x0f\x65lbowLiftEffort\x18\x03 \x01(\x05\x12\x17\n\x0fwristLiftEffort\x18\x04 \x01(\x05\x12\x19\n\x11wristSwivelEffort\x18\x05 \x01(\x05\x12\x0f\n\x07\x63lawVel\x18\x06 \x01(\x05\"\x97\x01\n\x13\x41rmPositionFeedback\x12\x19\n\x11shoulderLiftTicks\x18\x01 \x01(\x05\x12\x1b\n\x13shoulderSwivelTicks\x18\x02 \x01(\x05\x12\x16\n\x0e\x65lbowLiftTicks\x18\x03 \x01(\x05\x12\x16\n\x0ewristLiftTicks\x18\x04 \x01(\x05\x12\x18\n\x10wristSwivelTicks\x18\x05 \x01(\x05\"V\n\x13ScienceMotorRequest\x12\x14\n\x0cleadscrewVel\x18\x01 \x01(\x05\x12\x14\n\x0cturntableVel\x18\x02 \x01(\x05\x12\x13\n\x0b\x64rillEffort\x18\x03 \x01(\x05')

_globals = globals()
_builder.BuildMessageAndEnumDescriptors(DESCRIPTOR, _globals)
_builder.BuildTopDescriptorsAndMessages(DESCRIPTOR, 'urc_pb2', _globals)
if not _descriptor._USE_C_DESCRIPTORS:
  DESCRIPTOR._loaded_options = None
  _globals['_ARMENCODERSMESSAGE']._serialized_start=14
  _globals['_ARMENCODERSMESSAGE']._serialized_end=188
  _globals['_DRIVEENCODERSMESSAGE']._serialized_start=190
  _globals['_DRIVEENCODERSMESSAGE']._serialized_end=270
  _globals['_DRIVETRAINREQUEST']._serialized_start=272
  _globals['_DRIVETRAINREQUEST']._serialized_end=371
  _globals['_DRIVETRAINRESPONSE']._serialized_start=374
  _globals['_DRIVETRAINRESPONSE']._serialized_end=682
  _globals['_REQUESTMESSAGE']._serialized_start=685
  _globals['_REQUESTMESSAGE']._serialized_end=827
  _globals['_STATUSLIGHTCOMMAND']._serialized_start=829
  _globals['_STATUSLIGHTCOMMAND']._serialized_end=881
  _globals['_NEWSTATUSLIGHTCOMMAND']._serialized_start=884
  _globals['_NEWSTATUSLIGHTCOMMAND']._serialized_end=1027
  _globals['_STATUSLIGHTMESSAGE']._serialized_start=1030
  _globals['_STATUSLIGHTMESSAGE']._serialized_end=1170
  _globals['_SETPOINTMESSAGE']._serialized_start=1172
  _globals['_SETPOINTMESSAGE']._serialized_end=1234
  _globals['_TEENSYMESSAGE']._serialized_start=1236
  _globals['_TEENSYMESSAGE']._serialized_end=1362
  _globals['_ARMCLAWREQUEST']._serialized_start=1364
  _globals['_ARMCLAWREQUEST']._serialized_end=1397
  _globals['_ARMEFFORTREQUEST']._serialized_start=1400
  _globals['_ARMEFFORTREQUEST']._serialized_end=1570
  _globals['_ARMPOSITIONFEEDBACK']._serialized_start=1573
  _globals['_ARMPOSITIONFEEDBACK']._serialized_end=1724
  _globals['_SCIENCEMOTORREQUEST']._serialized_start=1726
  _globals['_SCIENCEMOTORREQUEST']._serialized_end=1812
# @@protoc_insertion_point(module_scope)
