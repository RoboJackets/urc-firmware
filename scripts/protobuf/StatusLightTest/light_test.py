import socket
import threading
import urc_pb2
# import numpy as np
from time import sleep

# constants
SEND_UPDATE_MS = 200
TIMEOUT_MS = 1000
SERVER_IP = '192.168.1.206'
PORT = 8443
JOY_MAX = 32767.0
JOY_MIN = -32768.0
STEER_DECREASE = 0.7
DEADBAND = 300

# state variables
server_address = (SERVER_IP, PORT)
arm_lock = threading.Lock()
armClawRequest = urc_pb2.ArmClawRequest()
effortRequest = urc_pb2.ArmEffortRequest()
exit_flag = False
debug_enabled = False
udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

def use_lights():
    blueMessage = urc_pb2.StatusLightMessage()
    redMessage = urc_pb2.NewStatusLightCommand()
    redMessage.redEnabled = 1
    blueMessage.blueEnabled = 1
    blueData = blueMessage.SerializeToString()
    redData = redMessage.SerializeToString()
    for i in range(40):
        if i % 2 == 0:
            udp_socket.sendto(blueData, server_address)
        else:
            udp_socket.sendto(redData, server_address)
        sleep(1)


if __name__ == "__main__":
    use_lights()
        

