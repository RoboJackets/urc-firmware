# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: urc.proto
"""Generated protocol buffer code."""
from google.protobuf import descriptor as _descriptor
from google.protobuf import descriptor_pool as _descriptor_pool
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()




DESCRIPTOR = _descriptor_pool.Default().AddSerializedFile(b'\n\turc.proto\"\xae\x01\n\x12\x41rmEncodersMessage\x12\x19\n\x11shoulderLiftTicks\x18\x01 \x02(\x05\x12\x19\n\x11shouldSwivelTicks\x18\x02 \x02(\x05\x12\x16\n\x0e\x65lbowLiftTicks\x18\x03 \x02(\x05\x12\x18\n\x10\x65lbowSwivelTicks\x18\x04 \x02(\x05\x12\x16\n\x0ewristLiftTicks\x18\x05 \x02(\x05\x12\x18\n\x10wristSwivelTicks\x18\x06 \x02(\x05\"P\n\x14\x44riveEncodersMessage\x12\x11\n\tleftSpeed\x18\x01 \x01(\x05\x12\x12\n\nrightSpeed\x18\x02 \x01(\x05\x12\x11\n\ttimestamp\x18\x03 \x02(\x05\"c\n\x11\x44rivetrainRequest\x12\x12\n\nm1Setpoint\x18\x01 \x02(\x05\x12\x12\n\nm2Setpoint\x18\x02 \x02(\x05\x12\x12\n\nm3Setpoint\x18\x03 \x02(\x05\x12\x12\n\nm4Setpoint\x18\x04 \x02(\x05\"\xb4\x02\n\x12\x44rivetrainResponse\x12\x17\n\x0fm1SpeedFeedback\x18\x01 \x02(\x05\x12\x11\n\tm1Current\x18\x02 \x02(\r\x12\x1a\n\x12m1PositionFeedback\x18\x03 \x02(\x05\x12\x17\n\x0fm2SpeedFeedback\x18\x04 \x02(\x05\x12\x11\n\tm2Current\x18\x05 \x02(\r\x12\x1a\n\x12m2PositionFeedback\x18\x06 \x02(\x05\x12\x17\n\x0fm3SpeedFeedback\x18\x07 \x02(\x05\x12\x11\n\tm3Current\x18\x08 \x02(\r\x12\x1a\n\x12m3PositionFeedback\x18\t \x02(\x05\x12\x17\n\x0fm4SpeedFeedback\x18\n \x02(\x05\x12\x11\n\tm4Current\x18\x0b \x02(\r\x12\x1a\n\x12m4PositionFeedback\x18\x0c \x02(\x05\"\x8e\x01\n\x0eRequestMessage\x12\x14\n\x0crequestSpeed\x18\x01 \x02(\x08\x12\x1a\n\x12requestDiagnostics\x18\x02 \x02(\x08\x12\x11\n\tleftSpeed\x18\x03 \x01(\x05\x12\x12\n\nrightSpeed\x18\x04 \x01(\x05\x12\x10\n\x08\x64uration\x18\x05 \x01(\x05\x12\x11\n\ttimestamp\x18\x06 \x02(\x05\"4\n\x12StatusLightCommand\x12\r\n\x05\x63olor\x18\x01 \x01(\x05\x12\x0f\n\x07\x64isplay\x18\x02 \x01(\x05\"\x8f\x01\n\x15NewStatusLightCommand\x12\x12\n\nredEnabled\x18\x01 \x02(\x05\x12\x13\n\x0b\x62lueEnabled\x18\x02 \x02(\x05\x12\x14\n\x0cgreenEnabled\x18\x03 \x02(\x05\x12\x10\n\x08redBlink\x18\x04 \x02(\x05\x12\x11\n\tblueBlink\x18\x05 \x02(\x05\x12\x12\n\ngreenBlink\x18\x06 \x02(\x05\"*\n\x12StatusLightMessage\x12\x14\n\x0clightCommand\x18\x01 \x02(\x05\">\n\x0fSetpointMessage\x12\x14\n\x0cleftSetpoint\x18\x01 \x02(\x05\x12\x15\n\rrightSetpoint\x18\x02 \x02(\x05\"~\n\rTeensyMessage\x12\x31\n\x12statusLightMessage\x18\x01 \x01(\x0b\x32\x13.StatusLightMessageH\x00\x12+\n\x0fsetpointMessage\x18\x02 \x01(\x0b\x32\x10.SetpointMessageH\x00\x42\r\n\x0bmessageType\"!\n\x0e\x41rmClawRequest\x12\x0f\n\x07\x63lawVel\x18\x01 \x01(\x05\"\xc2\x01\n\x10\x41rmEffortRequest\x12\x1a\n\x12shoulderLiftEffort\x18\x01 \x01(\x05\x12\x1c\n\x14shoulderSwivelEffort\x18\x02 \x01(\x05\x12\x17\n\x0f\x65lbowLiftEffort\x18\x03 \x01(\x05\x12\x17\n\x0fwristLiftEffort\x18\x04 \x01(\x05\x12\x19\n\x11wristSwivelEffort\x18\x05 \x01(\x05\x12\x0f\n\x07\x63lawVel\x18\x06 \x01(\x05\x12\x16\n\x0elinearActuator\x18\x07 \x01(\x05\"\x97\x01\n\x13\x41rmPositionFeedback\x12\x19\n\x11shoulderLiftTicks\x18\x01 \x01(\x05\x12\x1b\n\x13shoulderSwivelTicks\x18\x02 \x01(\x05\x12\x16\n\x0e\x65lbowLiftTicks\x18\x03 \x01(\x05\x12\x16\n\x0ewristLiftTicks\x18\x04 \x01(\x05\x12\x18\n\x10wristSwivelTicks\x18\x05 \x01(\x05\"V\n\x13ScienceMotorRequest\x12\x14\n\x0cleadscrewVel\x18\x01 \x01(\x05\x12\x14\n\x0cturntableVel\x18\x02 \x01(\x05\x12\x13\n\x0b\x64rillEffort\x18\x03 \x01(\x05')



_ARMENCODERSMESSAGE = DESCRIPTOR.message_types_by_name['ArmEncodersMessage']
_DRIVEENCODERSMESSAGE = DESCRIPTOR.message_types_by_name['DriveEncodersMessage']
_DRIVETRAINREQUEST = DESCRIPTOR.message_types_by_name['DrivetrainRequest']
_DRIVETRAINRESPONSE = DESCRIPTOR.message_types_by_name['DrivetrainResponse']
_REQUESTMESSAGE = DESCRIPTOR.message_types_by_name['RequestMessage']
_STATUSLIGHTCOMMAND = DESCRIPTOR.message_types_by_name['StatusLightCommand']
_NEWSTATUSLIGHTCOMMAND = DESCRIPTOR.message_types_by_name['NewStatusLightCommand']
_TEENSYMESSAGE = DESCRIPTOR.message_types_by_name['TeensyMessage']
_ARMCLAWREQUEST = DESCRIPTOR.message_types_by_name['ArmClawRequest']
_ARMEFFORTREQUEST = DESCRIPTOR.message_types_by_name['ArmEffortRequest']
_ARMPOSITIONFEEDBACK = DESCRIPTOR.message_types_by_name['ArmPositionFeedback']
_SCIENCEMOTORREQUEST = DESCRIPTOR.message_types_by_name['ScienceMotorRequest']
ArmEncodersMessage = _reflection.GeneratedProtocolMessageType('ArmEncodersMessage', (_message.Message,), {
  'DESCRIPTOR' : _ARMENCODERSMESSAGE,
  '__module__' : 'urc_pb2'
  # @@protoc_insertion_point(class_scope:ArmEncodersMessage)
  })
_sym_db.RegisterMessage(ArmEncodersMessage)

DriveEncodersMessage = _reflection.GeneratedProtocolMessageType('DriveEncodersMessage', (_message.Message,), {
  'DESCRIPTOR' : _DRIVEENCODERSMESSAGE,
  '__module__' : 'urc_pb2'
  # @@protoc_insertion_point(class_scope:DriveEncodersMessage)
  })
_sym_db.RegisterMessage(DriveEncodersMessage)

DrivetrainRequest = _reflection.GeneratedProtocolMessageType('DrivetrainRequest', (_message.Message,), {
  'DESCRIPTOR' : _DRIVETRAINREQUEST,
  '__module__' : 'urc_pb2'
  # @@protoc_insertion_point(class_scope:DrivetrainRequest)
  })
_sym_db.RegisterMessage(DrivetrainRequest)

DrivetrainResponse = _reflection.GeneratedProtocolMessageType('DrivetrainResponse', (_message.Message,), {
  'DESCRIPTOR' : _DRIVETRAINRESPONSE,
  '__module__' : 'urc_pb2'
  # @@protoc_insertion_point(class_scope:DrivetrainResponse)
  })
_sym_db.RegisterMessage(DrivetrainResponse)

RequestMessage = _reflection.GeneratedProtocolMessageType('RequestMessage', (_message.Message,), {
  'DESCRIPTOR' : _REQUESTMESSAGE,
  '__module__' : 'urc_pb2'
  # @@protoc_insertion_point(class_scope:RequestMessage)
  })
_sym_db.RegisterMessage(RequestMessage)

StatusLightCommand = _reflection.GeneratedProtocolMessageType('StatusLightCommand', (_message.Message,), {
  'DESCRIPTOR' : _STATUSLIGHTCOMMAND,
  '__module__' : 'urc_pb2'
  # @@protoc_insertion_point(class_scope:StatusLightCommand)
  })
_sym_db.RegisterMessage(StatusLightCommand)

NewStatusLightCommand = _reflection.GeneratedProtocolMessageType('NewStatusLightCommand', (_message.Message,), {
  'DESCRIPTOR' : _NEWSTATUSLIGHTCOMMAND,
  '__module__' : 'urc_pb2'
  # @@protoc_insertion_point(class_scope:NewStatusLightCommand)
  })
_sym_db.RegisterMessage(NewStatusLightCommand)

TeensyMessage = _reflection.GeneratedProtocolMessageType('TeensyMessage', (_message.Message,), {
  'DESCRIPTOR' : _TEENSYMESSAGE,
  '__module__' : 'urc_pb2'
  # @@protoc_insertion_point(class_scope:TeensyMessage)
  })
_sym_db.RegisterMessage(TeensyMessage)

ArmClawRequest = _reflection.GeneratedProtocolMessageType('ArmClawRequest', (_message.Message,), {
  'DESCRIPTOR' : _ARMCLAWREQUEST,
  '__module__' : 'urc_pb2'
  # @@protoc_insertion_point(class_scope:ArmClawRequest)
  })
_sym_db.RegisterMessage(ArmClawRequest)

ArmEffortRequest = _reflection.GeneratedProtocolMessageType('ArmEffortRequest', (_message.Message,), {
  'DESCRIPTOR' : _ARMEFFORTREQUEST,
  '__module__' : 'urc_pb2'
  # @@protoc_insertion_point(class_scope:ArmEffortRequest)
  })
_sym_db.RegisterMessage(ArmEffortRequest)

ArmPositionFeedback = _reflection.GeneratedProtocolMessageType('ArmPositionFeedback', (_message.Message,), {
  'DESCRIPTOR' : _ARMPOSITIONFEEDBACK,
  '__module__' : 'urc_pb2'
  # @@protoc_insertion_point(class_scope:ArmPositionFeedback)
  })
_sym_db.RegisterMessage(ArmPositionFeedback)

ScienceMotorRequest = _reflection.GeneratedProtocolMessageType('ScienceMotorRequest', (_message.Message,), {
  'DESCRIPTOR' : _SCIENCEMOTORREQUEST,
  '__module__' : 'urc_pb2'
  # @@protoc_insertion_point(class_scope:ScienceMotorRequest)
  })
_sym_db.RegisterMessage(ScienceMotorRequest)

if _descriptor._USE_C_DESCRIPTORS == False:

  DESCRIPTOR._options = None
  _ARMENCODERSMESSAGE._serialized_start=14
  _ARMENCODERSMESSAGE._serialized_end=188
  _DRIVEENCODERSMESSAGE._serialized_start=190
  _DRIVEENCODERSMESSAGE._serialized_end=270
  _DRIVETRAINREQUEST._serialized_start=273
  _DRIVETRAINREQUEST._serialized_end=412
  _DRIVETRAINRESPONSE._serialized_start=415
  _DRIVETRAINRESPONSE._serialized_end=669
  _REQUESTMESSAGE._serialized_start=672
  _REQUESTMESSAGE._serialized_end=814
  _STATUSLIGHTCOMMAND._serialized_start=816
  _STATUSLIGHTCOMMAND._serialized_end=868
  _NEWSTATUSLIGHTCOMMAND._serialized_start=871
  _NEWSTATUSLIGHTCOMMAND._serialized_end=1014
  _TEENSYMESSAGE._serialized_start=1017
  _TEENSYMESSAGE._serialized_end=1291
  _ARMCLAWREQUEST._serialized_start=1293
  _ARMCLAWREQUEST._serialized_end=1326
  _ARMEFFORTREQUEST._serialized_start=1329
  _ARMEFFORTREQUEST._serialized_end=1499
  _ARMPOSITIONFEEDBACK._serialized_start=1502
  _ARMPOSITIONFEEDBACK._serialized_end=1653
  _SCIENCEMOTORREQUEST._serialized_start=1655
  _SCIENCEMOTORREQUEST._serialized_end=1741
# @@protoc_insertion_point(module_scope)
