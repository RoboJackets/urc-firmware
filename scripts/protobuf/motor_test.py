from enum import Enum
import urc_pb2
import socket
import threading
import time
import signal
import sys

class MotorState(Enum):
    MOTOR_STATE_FORWARD = 1
    MOTOR_STATE_BACKWARD = 2
    MOTOR_STATE_STOPPED = 3

# constants
STATE_MACHINE_UPDATE_MS = 5000
MOTOR_SPEED = 3000
SERVER_IP = '192.168.1.168'
PORT = 8443

# state variables
curr = MotorState.MOTOR_STATE_STOPPED
prev = MotorState.MOTOR_STATE_STOPPED
server_address = (SERVER_IP, PORT)
client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

def determine_next_state(curr, prev):
    if curr == MotorState.MOTOR_STATE_STOPPED:
        if prev == MotorState.MOTOR_STATE_FORWARD:
            return MotorState.MOTOR_STATE_BACKWARD
        else:
            return MotorState.MOTOR_STATE_FORWARD
    else:
        return MotorState.MOTOR_STATE_STOPPED

def determine_speed(curr):
    if curr == MotorState.MOTOR_STATE_FORWARD:
        return MOTOR_SPEED
    elif curr == MotorState.MOTOR_STATE_BACKWARD:
        return -1 * MOTOR_SPEED
    else:
        return 0

# state machine
def state_machine():

    global curr
    global prev
    global client_socket

    new_state = determine_next_state(curr, prev)

    prev = curr
    curr = new_state
    speed = determine_speed(curr)

    print(f"State: [{prev} -> {curr}, speed={speed}]")

    message = urc_pb2.RequestMessage()
    message.requestSpeed = True
    message.requestDiagnostics = False
    message.leftSpeed = speed
    message.rightSpeed = speed
    message.timestamp = 0

    payload = message.SerializeToString()

    client_socket.sendto(payload, server_address)



def run_state_machine():
    threading.Timer(STATE_MACHINE_UPDATE_MS / 1000.0, run_state_machine).start()
    state_machine()


if __name__ == "__main__":

    run_state_machine()


