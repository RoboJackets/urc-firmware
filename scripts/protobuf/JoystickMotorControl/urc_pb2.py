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




DESCRIPTOR = _descriptor_pool.Default().AddSerializedFile(b'\n\turc.proto\"\xae\x01\n\x12\x41rmEncodersMessage\x12\x19\n\x11shoulderLiftTicks\x18\x01 \x02(\x05\x12\x19\n\x11shouldSwivelTicks\x18\x02 \x02(\x05\x12\x16\n\x0e\x65lbowLiftTicks\x18\x03 \x02(\x05\x12\x18\n\x10\x65lbowSwivelTicks\x18\x04 \x02(\x05\x12\x16\n\x0ewristLiftTicks\x18\x05 \x02(\x05\x12\x18\n\x10wristSwivelTicks\x18\x06 \x02(\x05\"P\n\x14\x44riveEncodersMessage\x12\x11\n\tleftSpeed\x18\x01 \x01(\x05\x12\x12\n\nrightSpeed\x18\x02 \x01(\x05\x12\x11\n\ttimestamp\x18\x03 \x02(\x05\"\x8e\x01\n\x0eRequestMessage\x12\x14\n\x0crequestSpeed\x18\x01 \x02(\x08\x12\x1a\n\x12requestDiagnostics\x18\x02 \x02(\x08\x12\x11\n\tleftSpeed\x18\x03 \x01(\x05\x12\x12\n\nrightSpeed\x18\x04 \x01(\x05\x12\x10\n\x08\x64uration\x18\x05 \x01(\x05\x12\x11\n\ttimestamp\x18\x06 \x02(\x05\"4\n\x12StatusLightCommand\x12\r\n\x05\x63olor\x18\x01 \x01(\x05\x12\x0f\n\x07\x64isplay\x18\x02 \x01(\x05\"!\n\x0e\x41rmClawRequest\x12\x0f\n\x07\x63lawVel\x18\x01 \x01(\x05\"\xaa\x01\n\x10\x41rmEffortRequest\x12\x1a\n\x12shoulderLiftEffort\x18\x01 \x01(\x05\x12\x1c\n\x14shoulderSwivelEffort\x18\x02 \x01(\x05\x12\x17\n\x0f\x65lbowLiftEffort\x18\x03 \x01(\x05\x12\x17\n\x0fwristLiftEffort\x18\x04 \x01(\x05\x12\x19\n\x11wristSwivelEffort\x18\x05 \x01(\x05\x12\x0f\n\x07\x63lawVel\x18\x06 \x01(\x05')

_globals = globals()
_builder.BuildMessageAndEnumDescriptors(DESCRIPTOR, _globals)
_builder.BuildTopDescriptorsAndMessages(DESCRIPTOR, 'urc_pb2', _globals)
if _descriptor._USE_C_DESCRIPTORS == False:
  DESCRIPTOR._options = None
  _globals['_ARMENCODERSMESSAGE']._serialized_start=14
  _globals['_ARMENCODERSMESSAGE']._serialized_end=188
  _globals['_DRIVEENCODERSMESSAGE']._serialized_start=190
  _globals['_DRIVEENCODERSMESSAGE']._serialized_end=270
  _globals['_REQUESTMESSAGE']._serialized_start=273
  _globals['_REQUESTMESSAGE']._serialized_end=415
  _globals['_STATUSLIGHTCOMMAND']._serialized_start=417
  _globals['_STATUSLIGHTCOMMAND']._serialized_end=469
  _globals['_ARMCLAWREQUEST']._serialized_start=471
  _globals['_ARMCLAWREQUEST']._serialized_end=504
  _globals['_ARMEFFORTREQUEST']._serialized_start=507
  _globals['_ARMEFFORTREQUEST']._serialized_end=677
# @@protoc_insertion_point(module_scope)
