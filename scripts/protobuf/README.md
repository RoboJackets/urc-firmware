# Protobuf Example

### Setup

1. Install `protoc` protobuf compiler
2. Run the following commands

```bash
cd urc-firmware/scripts/protobuf
protoc -I ../../protos/ --python_out=. urc.proto
```



### Running the Script

1. Connect the Teensy 4.1 and your computer to the same LAN. Configure static IPs for
your computer and the Teensy. 
2. Run the `motor_test.py` script. 

Expected behavior: all motors should spin in one direction, stop, spin in the opposite direction, and stop repeatedly.
