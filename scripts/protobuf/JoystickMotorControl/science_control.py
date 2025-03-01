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
scienceMotorRequest = urc_pb2.ScienceMotorRequest()
exit_flag = False
debug_enabled = False

# capture joystick input
def joystick_thread_vel():

    # shared thread variables
    global scienceMotorRequest, exit_flag, debug_enabled

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

                # leadscrew
                if event.code == 'BTN_SOUTH':
                    scienceMotorRequest.leadscrewVel = event.state * 5000
                elif event.code == 'BTN_EAST':
                    scienceMotorRequest.leadscrewVel = event.state * -5000
                # turntable
                elif event.code == 'ABS_Z':
                    val = 1 if event.state > 0 else 0
                    scienceMotorRequest.turntableVel = val * -100
                elif event.code == 'ABS_RZ':
                    val = 1 if event.state > 0 else 0
                    scienceMotorRequest.turntableVel = val * 100
                elif event.code == 'BTN_TL':
                    scienceMotorRequest.turntableVel = event.state * 50
                elif event.code == 'BTN_TR':
                    scienceMotorRequest.turntableVel = event.state * -50
                
                # drillscripts
                elif event.code == 'ABS_RY':
                    val = int(-150 * event.state / JOY_MAX) if abs(event.state) > DEADBAND else 0
                    scienceMotorRequest.drillEffort = val
                elif event.code == 'BTN_NORTH':
                    scienceMotorRequest.drillEffort = event.state * 10
                elif event.code == 'BTN_WEST':
                    scienceMotorRequest.drillEffort = event.state * -10

                # # TESTING
                print(f"[event={event.ev_type}, code={event.code}, state={event.state}]")   

        except:
            print("Joystick disconnected! Exiting...")
            exit_flag = True

# send data to Teensy
def output_thread():

    global scienceMotorRequest

    udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    while not exit_flag:

        payload = scienceMotorRequest.SerializeToString()
        udp_socket.sendto(payload, server_address)

        if debug_enabled:
            print(f'Send to {server_address}: {print_scienceMotorRequest(scienceMotorRequest)}')

        sleep(SEND_UPDATE_MS / 1000.0)


# test receiving data on localhost
def input_thread():


    udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    udp_socket.bind(('0.0.0.0', 8443))
    # udp_socket.settimeout(TIMEOUT_MS / 1000.0)

    print('Input thread starting...')

    while not exit_flag: 

        try:
            data, address = udp_socket.recvfrom(1024)

            message = urc_pb2.ScienceMotorRequest()
            message.ParseFromString(data)
            print(f'Recv from {address}: {print_scienceMotorRequest(message)}')
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

def print_scienceMotorRequest(request):
    s = '['
    s += f'leadscrewVel={request.leadscrewVel}'
    s += f'turntableVel={request.turntableVel}'
    s += f'drillEffort={request.drillEffort}'
    s += ']'
    return s

if __name__ == "__main__":

    # parse arguments
    parser = argparse.ArgumentParser(description="Joystick Control of Science")
    parser.add_argument("-D", action='store_true', help="Enable debug messages")
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
    # threading.Thread(target=input_thread).start()
    
