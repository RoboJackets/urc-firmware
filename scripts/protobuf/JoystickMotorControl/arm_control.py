import socket
import threading

from pynput import keyboard
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
SERVER_IP = "192.168.1.8"
PORT = 8443
JOY_MAX = 1023
JOY_MIN = -1023
STEER_DECREASE = 0.7
DEADBAND = 300

# state variables
server_address = (SERVER_IP, PORT)
# driveEncodersMessage = urc_pb2.DriveEncodersMessage()
arm_lock = threading.Lock()
joint_speed_commands = {
    "shoulderSwivelSpeed": 0,
    "shoulderLiftSpeed": 0,
    "elbowLiftSpeed": 0,
    "wristLeftSpeed": 0,
    "wristRightSpeed": 0,
    "clawVel": 0,
    "linearActuator": 0,
}
exit_flag = False
debug_enabled = False

current_keys = set()
current_keys_lock = threading.Lock()

udp_socket_lock = threading.Lock()
udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)


def on_key_press(key):
    """Handle the key press event"""
    try:
        key_name = key.char  # Try to get single character
    except AttributeError:
        key_name = str(key)  # Handle other keys (e.g., special keys)

    with current_keys_lock:
        current_keys.add(key_name)


def on_key_release(key):
    """Handle the key release event"""
    try:
        key_name = key.char
    except AttributeError:
        key_name = str(key)

    with current_keys_lock:
        current_keys.discard(key_name)

    if key == keyboard.Key.esc:
        # Stop listener
        return False


def keyboard_thread():
    global joint_speed_commands, arm_lock, exit_flag, debug_enabled

    listener = keyboard.Listener(on_press=on_key_press, on_release=on_key_release)
    listener.start()

    while listener.running:
        with current_keys_lock:
            if "Key.left" in current_keys:
                joint_speed_commands["shoulderSwivelSpeed"] = -700
            elif "Key.right" in current_keys:
                joint_speed_commands["shoulderSwivelSpeed"] = 700
            elif "Key.up" in current_keys:
                joint_speed_commands["shoulderLiftSpeed"] = 1500
            elif "Key.down" in current_keys:
                joint_speed_commands["shoulderLiftSpeed"] = -1500
            elif "r" in current_keys or "R" in current_keys:
                joint_speed_commands["elbowLiftSpeed"] = 1000
            elif "f" in current_keys or "F" in current_keys:
                joint_speed_commands["elbowLiftSpeed"] = -1000
            elif "a" in current_keys or "A" in current_keys:
                joint_speed_commands["wristRightSpeed"] = -1000
                joint_speed_commands["wristLeftSpeed"] = 1000
            elif "d" in current_keys or "D" in current_keys:
                joint_speed_commands["wristRightSpeed"] = 1000
                joint_speed_commands["wristLeftSpeed"] = -1000
            elif "w" in current_keys or "W" in current_keys:
                joint_speed_commands["wristRightSpeed"] = 1000
                joint_speed_commands["wristLeftSpeed"] = 1000
            elif "s" in current_keys or "S" in current_keys:
                joint_speed_commands["wristRightSpeed"] = -1000
                joint_speed_commands["wristLeftSpeed"] = -1000
            elif "[" in current_keys:
                joint_speed_commands["linearActuator"] = 1
            elif "]" in current_keys:
                joint_speed_commands["linearActuator"] = -1
            elif "z" in current_keys or "Z" in current_keys:
                joint_speed_commands["clawVel"] = -600
            elif "c" in current_keys or "C" in current_keys:
                joint_speed_commands["clawVel"] = 600
            else:
                for k in joint_speed_commands:
                    joint_speed_commands[k] = 0


# capture joystick input
def joystick_thread_vel():

    # shared thread variables
    global arm_lock, exit_flag, debug_enabled

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
                if event.code == "BTN_WEST":  # linear actuator forward
                    joint_speed_commands["linearActuator"] = event.state
                elif event.code == "BTN_SOUTH":  # linear actuator backward
                    joint_speed_commands["linearActuator"] = -1 * event.state
                elif event.code == "BTN_NORTH":  # claw close
                    joint_speed_commands["clawVel"] = event.state * 600
                elif event.code == "BTN_EAST":  # claw open
                    joint_speed_commands["clawVel"] = event.state * -600

                if event.code == "BTN_TL":  # swivel left
                    # elif event.code == 'BTN_TOP2':
                    joint_speed_commands["shoulderSwivelSpeed"] = event.state * 50
                elif event.code == "BTN_TR":  # swivel right
                    # elif event.code == 'BTN_PINKIE':
                    joint_speed_commands["shoulderSwivelSpeed"] = event.state * -50
                # elif event.code == 'ABS_RY':
                elif event.code == "ABS_Z":  # elbow 1 down
                    # Ensure input is in valid range from (0-255) - (0-25)
                    # speedRequest.shoulderLiftSpeed = 1 * round((event.state / 255) * 5)
                    joint_speed_commands["shoulderLiftSpeed"] = 2000
                elif event.code == "ABS_RZ":  # elbow 1 up
                    # Ensure input is in valid range from (0-255) - (0-25)
                    joint_speed_commands["shoulderLiftSpeed"] = -2000
                elif event.code == "ABS_Y":  # wrist 1
                    # Ensure input is in valid range from (-32000-32000) - (0-25)
                    # value = event.state
                    # if value < -32000:
                    #     value = -32000
                    # elif value > 32000:
                    #     value = 32000
                    # Map the value using linear interpolation
                    # speedRequest.wristLiftEffort = -1*round((value / 32000) * 25)
                    joint_speed_commands["wristLiftSpeed"] = -1000
                    joint_speed_commands["wristLiftSpeed"] = -1000
                elif event.code == "ABS_X":  # wrist 1
                    # Ensure input is in valid range from (-32000-32000) - (0-25)
                    # value = event.state
                    # if value < -32000:
                    #     value = -32000
                    # elif value > 32000:
                    #     value = 32000
                    # Map the value using linear interpolation
                    # speedRequest.wristLiftEffort = -1*round((value / 32000) * 25)
                    joint_speed_commands["wristLiftSpeed"] = -1000
                    joint_speed_commands["wristLiftSpeed"] = 1000
                elif event.code == "ABS_RY":  # wrist 2
                    # Ensure input is in valid range from (-32000-32000) - (0-25)
                    # value = event.state
                    # if value < -32000:
                    #     value = -32000
                    # elif value > 32000:
                    #     value = 32000
                    # Map the value using linear interpolation
                    # speedRequest.wristSwivelEffort = round((value / 32000) * 75)
                    joint_speed_commands["wristLiftSpeed"] = 1000
                    joint_speed_commands["wristLiftSpeed"] = 1000
                elif event.code == "ABS_A":  # wrist 2
                    # Ensure input is in valid range from (-32000-32000) - (0-25)
                    # value = event.state
                    # if value < -32000:
                    #     value = -32000
                    # elif value > 32000:
                    #     value = 32000
                    # Map the value using linear interpolation
                    # speedRequest.wristSwivelEffort = round((value / 32000) * 75)
                    joint_speed_commands["wristLiftSpeed"] = 1000
                    joint_speed_commands["wristLiftSpeed"] = -1000
                elif event.code == "ABS_HAT0Y":  # elbow 2
                    joint_speed_commands["elbowLiftSpeed"] = -1000
                # # TESTING
                # print(f"[event={event.ev_type}, code={event.code}, state={event.state}]")

        except:
            print("Joystick disconnected! Exiting...")
            exit_flag = True


# send data to Teensy
def output_thread():
    global joint_speed_commands, udp_socket

    while not exit_flag:

        speedRequest = urc_pb2.ArmSpeedRequest()
        speedRequest.shoulderLiftSpeed = joint_speed_commands["shoulderLiftSpeed"]
        speedRequest.shoulderSwivelSpeed = joint_speed_commands["shoulderSwivelSpeed"]
        speedRequest.elbowLiftSpeed = joint_speed_commands["elbowLiftSpeed"]
        speedRequest.wristLeftSpeed = joint_speed_commands["wristLeftSpeed"]
        speedRequest.wristRightSpeed = joint_speed_commands["wristRightSpeed"]
        speedRequest.clawVel = joint_speed_commands["clawVel"]
        speedRequest.linearActuator = joint_speed_commands["linearActuator"]

        # driveEncodersMessage.timestamp = 0
        # payload = driveEncodersMessage.SerializeToString()
        with arm_lock:
            payload = speedRequest.SerializeToString()
        with udp_socket_lock:
            udp_socket.sendto(payload, server_address)

        if debug_enabled:
            print(f"Send to {server_address}: {print_speedRequest(speedRequest)}")

        sleep(SEND_UPDATE_MS / 1000.0)


# test receiving data on localhost
def input_thread():

    udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    udp_socket.bind(("0.0.0.0", 8443))
    # udp_socket.settimeout(TIMEOUT_MS / 1000.0)

    print("Input thread starting...")

    while not exit_flag:

        try:
            data, address = udp_socket.recvfrom(1024)

            # message = urc_pb2.RequestMessage()
            # message.ParseFromString(data)
            # print(f'Recv from {address}: [left={message.leftSpeed}, right={message.rightSpeed}]')

            message = urc_pb2.ArmPositionFeedback()
            message.ParseFromString(data)
            # print(f'Recv from {address}: {print_armPositionFeedback(message)}')
        except:
            continue


# input ip address as 'ip_addr:port', output tuple (ip (str), port (int))
def validate_ip_address(ip):
    split_string = ip.split(":")

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
    return f"[left={request.leftSpeed}, right={request.rightSpeed}]"


def print_speedRequest(request):
    s = "["
    s += f"sLift={request.shoulderLiftSpeed},"
    s += f"sSwivel={request.shoulderSwivelSpeed},"
    s += f"eLift={request.elbowLiftSpeed},"
    s += f"wLift={request.wristLiftSpeed},"
    s += f"wSwivel={request.wristSwivelSpeed},"
    s += f"clawVel={request.clawVel}"
    s += f"linActu={request.linearActuator}"
    s += "]"
    return s


def print_armPositionFeedback(request):
    s = "["
    s += f"sLiftTicks={request.shoulderLiftTicks},"
    s += f"sSwivelTicks={request.shoulderSwivelTicks},"
    s += f"eLiftTicks={request.elbowLiftTicks},"
    s += f"wLiftTicks={request.wristLiftTicks},"
    s += f"wSwivelTicks={request.wristSwivelTicks},"
    s += "]"
    return s


if __name__ == "__main__":

    # parse arguments
    parser = argparse.ArgumentParser(description="Joystick Control of Arm")

    parser.add_argument("-D", action="store_true", help="Enable debug messages")
    # parser.add_argument("-R", type=str, default="[-3000, 3000]", help="Range of output value. Example: [-3000,3000]")

    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument("-L", action="store_true", help="Test joystick on localhost.")
    group.add_argument(
        "-E", type=str, help="Teensy endpoint. Example: 192.168.1.168:8443"
    )

    parser.add_argument("-I", type=str, help="Input mode, keyboard or joystick")

    args = parser.parse_args()

    # if endpoint provided, validate ip address
    if args.E:
        server_address = validate_ip_address(args.E)
    if args.D:
        debug_enabled = True

    udp_socket.bind(("0.0.0.0", server_address[1]))
    udp_socket.settimeout(TIMEOUT_MS / 1000.0)
    udp_socket.setblocking(False)

    # start threads
    threading.Thread(target=output_thread).start()
    if args.I == "joystick":
        # threading.Thread(target=joystick_thread_vel).start()
        pass
    elif args.I == "keyboard":
        threading.Thread(target=keyboard_thread).start()
    else:
        raise ValueError
    # threading.Thread(target=input_thread).start()

    # # if testing on localhost, start input_thread
    # if args.L:
    #     threading.Thread(target=input_thread).start()
