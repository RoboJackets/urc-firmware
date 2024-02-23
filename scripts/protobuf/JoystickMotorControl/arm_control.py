import socket
import threading
import urc_pb2
import argparse
import ipaddress
import re
import numpy as np
from inputs import get_key, devices, get_gamepad
from time import sleep

# constants
SEND_UPDATE_MS = 200
TIMEOUT_MS = 1000
SERVER_IP = '127.0.0.1'
PORT = 8443
JOY_MAX = 32767.0
JOY_MIN = -32768.0
STEER_DECREASE = 0.7
DEADBAND = 300

# state variables
server_address = (SERVER_IP, PORT)
# driveEncodersMessage = urc_pb2.DriveEncodersMessage()
arm_lock = threading.Lock()
armClawRequest = urc_pb2.ArmClawRequest()
effortRequest = urc_pb2.ArmEffortRequest()
exit_flag = False
debug_enabled = False

# capture joystick input
def joystick_thread_vel():

    # shared thread variables
    global armClawRequest, effortRequest, arm_lock, exit_flag, debug_enabled

    # check for gamepad
    if len(devices.gamepads) <= 0:
        print("No joysticks detected! Exiting...")
        exit_flag = True
    else:
        print("Gamepad connected: ", devices.gamepads[0])

    while not exit_flag:
        
        try:
            events = get_gamepad()
            for event in events:

                if event.code == 'BTN_SOUTH':
                    effortRequest.clawVel = event.state * 50
                elif event.code == 'BTN_EAST':
                    effortRequest.clawVel = event.state * -50
                elif event.code == 'BTN_NORTH':
                    effortRequest.wristSwivelEffort = event.state * 50
                elif event.code == 'BTN_WEST':
                    effortRequest.wristSwivelEffort = event.state * -50
                elif event.code == 'BTN_TL':
                    effortRequest.shoulderSwivelEffort = event.state * -50
                elif event.code == 'BTN_TR':
                    effortRequest.shoulderSwivelEffort = event.state * 50
                elif event.code == 'ABS_RY':
                    if event.state > 200:
                        effortRequest.wristLiftEffort = 50
                    elif event.state < -200:
                        effortRequest.wristLiftEffort = -50
                    else:
                        effortRequest.wristLiftEffort = 0
                elif event.code == 'ABS_Y':
                    if event.state > 200:
                        effortRequest.elbowLiftEffort = 50
                    elif event.state < -200:
                        effortRequest.elbowLiftEffort = -50
                    else:
                        effortRequest.elbowLiftEffort = 0
                elif event.code == 'ABS_HAT0Y':
                    effortRequest.shoulderLiftEffort = event.state * -50


                # # TESTING
                # print(f"[event={event.ev_type}, code={event.code}, state={event.state}]")   

        except:
            print("Joystick disconnected! Exiting...")
            exit_flag = True

# send data to Teensy
def output_thread():

    global armClawRequest, effortRequest

    udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    while not exit_flag:

        # driveEncodersMessage.timestamp = 0
        # payload = driveEncodersMessage.SerializeToString()
        with arm_lock:
            # payload = armClawRequest.SerializeToString()
            payload = effortRequest.SerializeToString()
        udp_socket.sendto(payload, server_address)

        if debug_enabled:
            # print(f'Send to {server_address}: {print_armClawRequest(armClawRequest)}')
            print(f'Send to {server_address}: {print_effortRequest(effortRequest)}')

        sleep(SEND_UPDATE_MS / 1000.0)


# test receiving data on localhost
def input_thread():


    udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    udp_socket.bind(server_address)
    udp_socket.settimeout(TIMEOUT_MS / 1000.0)

    while not exit_flag: 

        try:
            data, address = udp_socket.recvfrom(1024)

            message = urc_pb2.RequestMessage()
            message.ParseFromString(data)
            # print(f'Recv from {address}: [left={message.leftSpeed}, right={message.rightSpeed}]')
        except:
            continue

# input ip address as 'ip_addr:port', output tuple (ip (str), port (int))
def validate_ip_address(ip):
    split_string = ip.split(':')

    # validate ip address
    try:
        ipaddress.ip_address(split_string[0])
    except:
        print(f"Invalid ip address: {split_string[0]}")
        exit()

    # convert port to string
    try:
        port = int(split_string[1])
    except:
        print(f"Port can't be converted to string: {split_string[1]}")
        exit()

    # validate port
    if port < 1 or port > 65535:
        print(f"Invalid port: {port}")
        exit()

    return (split_string[0], port)

def validate_range(range):
    range_values = range.strip("[]").replace(" ", "").split(",")

    if len(range_values) != 2:
        raise ValueError("Invalid number of values in the range string")
        
    # Convert values to integers
    int_values = [int(value) for value in range_values]
    
    return int_values


def print_driveEncodersMessage(request):
    return f'[left={request.leftSpeed}, right={request.rightSpeed}]'

def print_armClawRequest(request):
    return f'[clawVel={request.clawVel}]'

def print_effortRequest(request):
    s = '['
    s += f'sLift={request.shoulderLiftEffort},'
    s += f'sSwivel={request.shoulderSwivelEffort},'
    s += f'eLift={request.elbowLiftEffort},'
    s += f'wLift={request.wristLiftEffort},'
    s += f'wSwivel={request.wristSwivelEffort},'
    s += f'clawVel={request.clawVel}'
    s += ']'
    return s

def reset_print_armClawRequest(request):
    request.clawVel = 0

if __name__ == "__main__":

    # parse arguments
    parser = argparse.ArgumentParser(description="Joystick Control of Arm")

    parser.add_argument("-D", action='store_true', help="Enable debug messages")
    # parser.add_argument("-R", type=str, default="[-3000, 3000]", help="Range of output value. Example: [-3000,3000]")

    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument("-L", action='store_true', help="Test joystick on localhost.")
    group.add_argument("-E", type=str, help="Teensy endpoint. Example: 192.168.1.168:8443")

    args = parser.parse_args()

    # if endpoint provided, validate ip address
    if args.E:
        server_address = validate_ip_address(args.E)
    if args.D:
        debug_enabled = True

    # start threads
    threading.Thread(target=output_thread).start()
    threading.Thread(target=joystick_thread_vel).start()

    # if testing on localhost, start input_thread
    if args.L:
        threading.Thread(target=input_thread).start()
    
