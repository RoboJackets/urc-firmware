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
JOY_MAX = 32767
JOY_MIN = -32768 
STEER_DECREASE = 0.7
DEADBAND = 300

# state variables
server_address = (SERVER_IP, PORT)
left_speed = 0
right_speed = 0
exit_flag = False
debug_enabled = False

min_value = -3000
max_value = 3000


# capture joystick input
def joystick_thread_tank():

    # shared thread variables
    global left_speed, right_speed, exit_flag, debug_enabled

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
                if event.ev_type == 'Absolute' and event.code == 'ABS_Y':
                    left_speed = translate_joystick_input(int(event.state))

                    if debug_enabled:
                        print(f'left_input={event.state}, left_speed={left_speed:.2f}')
                    
                elif event.ev_type == 'Absolute' and event.code == 'ABS_RY':
                    right_speed = translate_joystick_input(int(event.state))
                    
                    if debug_enabled:
                        print(f'right_input={event.state}, right_speed={right_speed:.2f}')

                if debug_enabled:
                    print(f"[event={event.ev_type}, code={event.code}]")
            

        except:
            print("Joystick disconnected! Exiting...")
            exit_flag = True

# capture joystick input
def joystick_thread_steer():

    # shared thread variables
    global left_speed, right_speed, exit_flag, debug_enabled

    # check for gamepad
    if len(devices.gamepads) <= 0:
        print("No joysticks detected! Exiting...")
        exit_flag = True
    else:
        print("Gamepad connected: ", devices.gamepads[0])

    input_speed = 0
    input_steer = 0
    input_detected = False

    while not exit_flag:
        
        try:
            events = get_gamepad()
            for event in events:
                if event.ev_type == 'Absolute' and event.code == 'ABS_Y':
                    input_speed = translate_joystick_input(int(event.state))
                    input_detected = True

                    if debug_enabled:
                        print(f'forward_input={event.state / JOY_MAX}')
                    
                elif event.ev_type == 'Absolute' and event.code == 'ABS_RX':
                    # input_steer = translate_joystick_input(int(event.state))
                    input_steer = int(event.state)
                    input_detected = True
                    
                    if debug_enabled:
                        print(f'steer={event.state / JOY_MAX}')
            
            if input_detected:
                left_speed = int(input_speed if input_steer > DEADBAND else input_speed * (1.0 - ((STEER_DECREASE) * abs(input_steer) / JOY_MAX)))
                right_speed = int(input_speed if input_steer < -1 * DEADBAND else input_speed * (1.0 - ((STEER_DECREASE) * abs(input_steer) / JOY_MAX)))

                if debug_enabled:
                    print(f'left_speed={left_speed}, right_speed={right_speed}')

        except:
            print("Joystick disconnected! Exiting...")
            exit_flag = True

        input_detected = False

# send data to Teensy
def output_thread():

    global left_speed, right_speed, server_address, exit_flag, debug_enabled

    udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    while not exit_flag:
        # message = urc_pb2.RequestMessage()
        # message.requestSpeed = True
        # message.requestDiagnostics = False
        # message.leftSpeed = left_speed
        # message.rightSpeed = right_speed
        # message.timestamp = 0

        message = urc_pb2.DriveEncodersMessage()
        message.leftSpeed = left_speed
        message.rightSpeed = right_speed
        message.timestamp = 0

        payload = message.SerializeToString()

        udp_socket.sendto(payload, server_address)

        if debug_enabled:
            print(f'Send to {server_address}: [left={message.leftSpeed}, right={message.rightSpeed}]')

        sleep(SEND_UPDATE_MS / 1000.0)

# test receiving data on localhost
def input_thread():

    global server_address, debug_enabled, exit_flag

    udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    udp_socket.bind(server_address)
    udp_socket.settimeout(TIMEOUT_MS / 1000.0)

    while not exit_flag: 

        try:
            data, address = udp_socket.recvfrom(1024)

            message = urc_pb2.RequestMessage()
            message.ParseFromString(data)
            print(f'Recv from {address}: [left={message.leftSpeed}, right={message.rightSpeed}]')
        except:
            continue

# translate integer speed (0 to 255) to speed 
def translate_joystick_input(speed):
    ouput_value = np.interp(speed, [JOY_MIN, JOY_MAX], [min_value, max_value])
    
    if abs(ouput_value) <= 300:
        ouput_value = 0
    
    return (int)(ouput_value)

    

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


if __name__ == "__main__":

    # parse arguments
    parser = argparse.ArgumentParser(description="Joystick Control of Solo UNO")

    parser.add_argument("-D", action='store_true', help="Enable debug messages")
    parser.add_argument("-R", type=str, default="[-3000, 3000]", help="Range of output value. Example: [-3000,3000]")
    parser.add_argument("-C", choices=['tank', 'steer'], default='tank', help="Control type")

    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument("-L", action='store_true', help="Test joystick on localhost.")
    group.add_argument("-E", type=str, help="Teensy endpoint. Example: 192.168.1.168:8443")

    args = parser.parse_args()

    # if endpoint provided, validate ip address
    if args.E:
        server_address = validate_ip_address(args.E)
    if args.D:
        debug_enabled = True

    min_value, max_value = validate_range(args.R)

    # start threads
    threading.Thread(target=output_thread).start()

    if args.C == 'steer':
        threading.Thread(target=joystick_thread_steer).start()
    else:
        threading.Thread(target=joystick_thread_tank).start()

    # if testing on localhost, start input_thread
    if args.L:
        threading.Thread(target=input_thread).start()
    
