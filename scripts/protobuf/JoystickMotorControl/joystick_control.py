import socket
import threading
import urc_pb2
import argparse
import ipaddress
import re
import os
import time
import numpy as np
from inputs import get_key, devices, get_gamepad
from copy import deepcopy
from threading import Lock
from pynput import keyboard
from rich.console import Console
from rich.live import Live
from rich.panel import Panel
from rich.table import Table
from datetime import datetime
import random
import pygame

# constants
SEND_UPDATE_MS = 200

TIMEOUT_MS = 1000
SERVER_IP = '127.0.0.1'
PORT = 8443
JOY_MAX = 32767
JOY_MIN = -32768 
STEER_DECREASE = 0.7
DEADBAND = 300

class SoloDataStruct:

    def __init__(self, speed, current, position):
        self.speedFeedback = speed
        self.quadratureCurrent = current
        self.positionFeedback = position

    speedFeedback = 0
    quadratureCurrent = 0
    positionFeedback = 0

# state variables
server_address = (SERVER_IP, PORT)
left_speed = 0
right_speed = 0
# left_feedback = 0
# right_feedback = 0
feedback_data = dict()
exit_flag = False
debug_enabled = False

input_data_lock = Lock()
teensy_data_lock = Lock()

# keyboard thread
current_keys = set()
current_keys_lock = Lock()

udp_socket_lock = Lock()
udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

min_value = -3000
max_value = 3000



def sfxtToFloat(input):

    if input <=  0x7FFE0000:
        return input / 131072.0
    else:
        invert = 0xFFFFFFFF - input + 1
        return invert / -131072.0

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
                    with input_data_lock:
                        left_speed = translate_joystick_input(int(event.state))

                    if debug_enabled:
                        print(f'left_input={event.state}, left_speed={left_speed:.2f}')
                    
                elif event.ev_type == 'Absolute' and event.code == 'ABS_RY':
                    with input_data_lock:
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

def joystick_pygame():

    global left_speed, right_speed, exit_flag, debug_enabled
    LEFT_Y_AXIS = -1
    RIGHT_Y_AXIS = -1

    pygame.init()
    pygame.joystick.init()

    # check for gamepad
    if pygame.joystick.get_count() <= 0:
        print("No joysticks detected! Exiting...")
        pygame.quit()
        exit_flag = True
        return

    joystick = pygame.joystick.Joystick(0)
    joystick.init()
    print("Gamepad connected: ", joystick.get_name())

    if joystick.get_numaxes() == 4:
        LEFT_Y_AXIS = 1
        RIGHT_Y_AXIS = 3
    elif joystick.get_numaxes() == 6:
        LEFT_Y_AXIS = 1
        RIGHT_Y_AXIS = 4
    else:
        print(f"Abnormal number of axes: {joystick.get_numaxes()}")
        pygame.quit()
        exit_flag = True
        return

    while not exit_flag:
        
            events = pygame.event.get()

            for event in events:
                if event.type == pygame.JOYAXISMOTION and event.axis == LEFT_Y_AXIS:
                    with input_data_lock:
                        left_speed= translate_joystick_input_pygame(event.value)
                if event.type == pygame.JOYAXISMOTION and event.axis == RIGHT_Y_AXIS:
                    with input_data_lock:
                        right_speed = translate_joystick_input_pygame(event.value)

                if pygame.joystick.get_count() <= 0:
                    print("Joystick disconnected! Exiting...")
                    exit_flag = True

            # for event in pygame.event.get():
            #     if event.type == pygame.JOYAXISMOTION:
            #         print(f'Axis {event.axis} value: {event.value:.3f}')
            #     elif event.type == pygame.JOYBUTTONDOWN:
            #         print(f'Button {event.button} pressed')
            #     elif event.type == pygame.JOYBUTTONUP:
            #         print(f'Button {event.button} released')

           


def on_key_press(key):
    """ Handle the key press event """
    try:
        key_name = key.char  # Try to get single character
    except AttributeError:
        key_name = str(key)  # Handle other keys (e.g., special keys)
    
    with current_keys_lock:
        current_keys.add(key_name)

def on_key_release(key):
    """ Handle the key release event """
    try:
        key_name = key.char
    except AttributeError:
        key_name = str(key)
    
    with current_keys_lock:
        current_keys.discard(key_name)
    
    if key == keyboard.Key.esc:
        # Stop listener
        return False

def clear_screen():
    """ Clear the terminal screen """
    os.system('cls' if os.name == 'nt' else 'clear')

def keyboard_thread():

    # shared thread variables
    global left_speed, right_speed, exit_flag, debug_enabled

    listener = keyboard.Listener(on_press=on_key_press, on_release=on_key_release)
    listener.start()  # Start the listener

    try:
        while listener.running:
            with current_keys_lock:
                # Clear screen and print the current list of pressed keys
                # clear_screen()
                # print("Press 'esc' to exit...")
                # print("Currently pressed keys:", ', '.join(current_keys))
                if "Key.up" in current_keys and "Key.down" not in current_keys:
                    with input_data_lock:
                        left_speed = -3000
                        right_speed = -3000
                elif "Key.up" not in current_keys and "Key.down" in current_keys:
                    with input_data_lock:
                        left_speed = 3000
                        right_speed = 3000
                elif "Key.right" in current_keys and "Key.left" not in current_keys:
                    with input_data_lock:
                        left_speed = -3000
                        right_speed = 3000
                elif "Key.right" not in current_keys and "Key.left" in current_keys:
                    with input_data_lock:
                        left_speed = 3000
                        right_speed = -3000
                else:
                    with input_data_lock:
                        left_speed = 0
                        right_speed = 0
            time.sleep(0.1)  # Update every 100 ms
    finally:
        print("Exiting...")
        listener.stop()
        exit_flag = True


# send data to Teensy
def output_thread():

    global left_speed, right_speed, server_address, exit_flag, debug_enabled, udp_socket

    # udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    while not exit_flag:
        
        # # old message format, working
        # message = urc_pb2.DrivetrainRequest()
        # message.m1Setpoint = left_speed
        # message.m2Setpoint = left_speed
        # message.m3Setpoint = left_speed
        # message.m4Setpoint = right_speed
        # message.m5Setpoint = right_speed
        # message.m6Setpoint = right_speed
        # payload = b'\x11' + message.SerializeToString()

        # # new message format, working
        # message = urc_pb2.TeensyMessage()
        # message.messageID = 0
        # message.driveEncodersMessage.m1Setpoint = left_speed
        # message.driveEncodersMessage.m2Setpoint = left_speed
        # message.driveEncodersMessage.m3Setpoint = left_speed
        # message.driveEncodersMessage.m4Setpoint = right_speed
        # message.driveEncodersMessage.m5Setpoint = right_speed
        # message.driveEncodersMessage.m6Setpoint = right_speed
        # payload = message.SerializeToString()

        # software message fix
        message = urc_pb2.TeensyMessage()
        message.messageID = 0
        message.m1Setpoint = left_speed
        message.m2Setpoint = left_speed
        message.m3Setpoint = right_speed
        message.m4Setpoint = right_speed
        message.redEnabled = 0
        message.blueEnabled = 0
        message.greenEnabled = 0
        message.redBlink = 0
        message.blueBlink = 0
        message.greenBlink = 0
        payload = message.SerializeToString()

        with udp_socket_lock:
            udp_socket.sendto(payload, server_address)

        if debug_enabled:
            print(f'Send to {server_address}: [left={message.leftSpeed}, right={message.rightSpeed}]')

        time.sleep(SEND_UPDATE_MS / 1000.0)

# test receiving data on localhost
def input_thread():

    global server_address, debug_enabled, exit_flag, udp_socket, feedback_data
    
    # udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    # udp_socket.bind(server_address)
    # udp_socket.settimeout(TIMEOUT_MS / 1000.0)

    while not exit_flag: 

        with udp_socket_lock:
            data = -1

            try:
                # first packet
                data, addr = udp_socket.recvfrom(1024)
                # while True:
                #     _, _ = udp_socket.recvfrom(1024)

                # # last packet 
                # while True:
                    # data, addr = udp_socket.recvfrom(1024)

            except BlockingIOError as e:
                pass
                
            if data != -1:
                try: 
                    message = urc_pb2.DrivetrainResponse()
                    message.ParseFromString(data)

                    with teensy_data_lock:
                        feedback_data[1] = SoloDataStruct(message.m1SpeedFeedback, sfxtToFloat(message.m1Current), message.m1PositionFeedback)
                        feedback_data[2] = SoloDataStruct(message.m2SpeedFeedback, sfxtToFloat(message.m2Current), message.m2PositionFeedback)
                        feedback_data[3] = SoloDataStruct(message.m3SpeedFeedback, sfxtToFloat(message.m3Current), message.m3PositionFeedback)
                        feedback_data[4] = SoloDataStruct(message.m4SpeedFeedback, sfxtToFloat(message.m4Current), message.m4PositionFeedback)                
                except:
                    pass

        time.sleep(0.05)

# translate joystick input, pygame version
def translate_joystick_input_pygame(speed):
    ouput_value = np.interp(speed, [-1, 1], [min_value, max_value])

    if abs(ouput_value) <= 300:
        ouput_value = 0

    return (int)(ouput_value)

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


##########

def example_table():
    """Generate a table with random data."""
    table = Table(title="Dynamic Data Table")

    # Add columns
    table.add_column("ID", style="dim", width=12)
    table.add_column("Value", width=12)
    table.add_column("Status", justify="right")

    # Add rows with random data
    for i in range(1, 5):
        value = random.randint(1, 100)
        status = "Active" if value > 50 else "Inactive"
        table.add_row(f"Item {i}", str(value), status)

    return table

def display_example_table():
    global exit_flag
    console = Console()
    with Live(example_table(), console=console, refresh_per_second=1) as live:
        while not exit_flag:
            live.update(example_table())  # Update the table with new data
            time.sleep(1)  # Refresh the table every second

def display_time():
    global exit_flag
    console = Console()
    with Live(console=console, refresh_per_second=10):
        while not exit_flag:
            # Get current time
            current_time = datetime.now().strftime("%H:%M:%S")
            # Create a panel with the current time
            panel = Panel(f"[bold magenta]{current_time}[/bold magenta]", title="Current Time", subtitle="Updated every second")
            # Update the Live display with the new panel
            console.clear()
            console.print(panel)
            time.sleep(0.5)  # Update every second

##########

# class SoloDataStruct:

#     def __init__(self, speed, current):
#         self.speedFeedback = speed
#         self.quadratureCurrent = current 

#     speedFeedback = 0
#     quadratureCurrent = 0


def data_table(left_setpoint, right_setpoint, data_dict):
    table = Table(title="Drivetrain Feedback")
    table.add_column("Motor", style="dim", width=12)
    table.add_column("Setpoint", width=12)
    table.add_column("Feedback", width=12)
    table.add_column("Current", width=12)
    table.add_column("Position", width=12)

    table.add_row("1", str(left_setpoint), str(data_dict[1].speedFeedback), str(data_dict[1].quadratureCurrent), str(data_dict[1].positionFeedback))
    table.add_row("2", str(left_setpoint), str(data_dict[2].speedFeedback), str(data_dict[2].quadratureCurrent), str(data_dict[2].positionFeedback))
    table.add_row("3", str(right_setpoint), str(data_dict[3].speedFeedback), str(data_dict[3].quadratureCurrent), str(data_dict[3].positionFeedback))
    table.add_row("4", str(right_setpoint), str(data_dict[4].speedFeedback), str(data_dict[4].quadratureCurrent), str(data_dict[4].positionFeedback))
    return table


def display_data_table():
    global exit_flag, left_speed, right_speed,feedback_data
    console = Console()
    l_speed = 0
    r_speed = 0
    data_dict = dict()

    data_dict[1] = SoloDataStruct(0,0,0)
    data_dict[2] = SoloDataStruct(0,0,0)
    data_dict[3] = SoloDataStruct(0,0,0)
    data_dict[4] = SoloDataStruct(0,0,0)

    with Live(data_table(l_speed,r_speed,data_dict), console=console, refresh_per_second=10) as live:
        while not exit_flag:
            with input_data_lock:
                l_speed = left_speed
                r_speed = right_speed
            with teensy_data_lock:
                data_dict = deepcopy(feedback_data)

            for i in range(1,7):
                if i not in data_dict:
                    data_dict[i] = SoloDataStruct("X","X","X")
            
            live.update(data_table(l_speed,r_speed,data_dict))
            time.sleep(0.1)


if __name__ == "__main__":

    # parse arguments
    parser = argparse.ArgumentParser(description="Joystick Control of Solo UNO")

    parser.add_argument("-D", action='store_true', help="Enable debug messages")
    parser.add_argument("-R", type=str, default="[-3000, 3000]", help="Range of output value. Example: [-3000,3000]")
    # parser.add_argument("-C", choices=['tank', 'steer'], default='tank', help="Control type")
    parser.add_argument("-I", choices=['joystick', 'keyboard'], default='keyboard', help="Input type")

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

    udp_socket.bind(('0.0.0.0', server_address[1]))
    udp_socket.settimeout(TIMEOUT_MS / 1000.0)
    udp_socket.setblocking(False)

    # start threads
    threading.Thread(target=output_thread).start()
    threading.Thread(target=input_thread).start()
    threading.Thread(target=display_data_table).start()

    if args.I == 'keyboard': 
        threading.Thread(target=keyboard_thread).start()
    elif args.I == 'joystick':
        threading.Thread(target=joystick_pygame).start()
        # if args.C == 'steer':
        #     threading.Thread(target=joystick_thread_steer).start()
        # else:
        #     threading.Thread(target=joystick_thread_tank).start()

    # if testing on localhost, start input_thread
    # if args.L:
    #     threading.Thread(target=input_thread).start()
    
