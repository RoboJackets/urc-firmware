# Protobuf Example

### Setup

##### Install Python Dependencies  

1. Install Python on your system.
2. Create a [Python virtual environment](https://packaging.python.org/en/latest/guides/installing-using-pip-and-virtual-environments/).
3. Install dependencies in `requirements.txt` file:

```bash
pip3 install -r requirements.txt
```

##### Install `protoc` compiler

1. Install [`protoc` protobuf compiler](https://github.com/protocolbuffers/protobuf/releases).
2. Run the following commands

```bash
protoc -I ../../../protos/ --python_out=. urc.proto
```

3. End result: `urc_pb2.py` file

### Running the Script

1. Connect the Teensy 4.1 and your computer to the same LAN. Configure static IPs for
your computer and the Teensy. 
2. Run the `motor_test.py` script. 

Expected behavior: all motors should spin in one direction, stop, spin in the opposite direction, and stop repeatedly.
