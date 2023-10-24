import socket
import threading
import urc_pb2
from inputs import get_key, devices

# constants
STATE_MACHINE_UPDATE_MS = 5000
MOTOR_SPEED = 3000
SERVER_IP = '192.168.1.168'
PORT = 8443

# state variables
server_address = (SERVER_IP, PORT)
client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# capture joystick input
def joystick_thread():
    # List all connected input devices
    for device in devices:
        print(device)

    # Find the joystick in the list (you may need to look at the output to identify it)
    joystick = devices.joysticks[0]  # Change the index if you have multiple joysticks

    if len(devices.joysticks) <= 0:
        print("No joysticks detected! Exiting...")
        exit()

    # Process events
    while True:
        events = get_key()
        for event in events:
            if event.ev_type == "Absolute" and event.ev_type == "Sync" and event.ev_type == "Misc":
                if event.ev_type == "Absolute":
                    if event.ev_type == "ABS_X":
                        print(f"Axis X: {event.ev_value}")
                    elif event.ev_type == "ABS_Y":
                        print(f"Axis Y: {event.ev_value}")
                    # Add more cases for other axes if needed

                elif event.ev_type == "Sync":
                    pass  # Ignore Sync events

                elif event.ev_type == "Misc":
                    if event.ev_type == "BTN_A":
                        print("Button A pressed")
                    elif event.ev_type == "BTN_B":
                        print("Button B pressed")
                    # Add more cases for other buttons if needed

# send data to Teensy
def output_thread():

    global client_socket

    # new_state = determine_next_state(curr, prev)

    # speed = determine_speed(curr)

    message = urc_pb2.RequestMessage()
    message.requestSpeed = True
    message.requestDiagnostics = False
    # message.leftSpeed = speed
    # message.rightSpeed = speed
    message.timestamp = 0

    payload = message.SerializeToString()

    client_socket.sendto(payload, server_address)

    


if __name__ == "__main__":

    threading.Timer(interval=STATE_MACHINE_UPDATE_MS / 1000.0, function=output_thread).start()
    threading.Thread(target=joystick_thread).start()


