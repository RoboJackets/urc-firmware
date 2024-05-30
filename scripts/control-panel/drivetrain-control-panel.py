from tabulate import tabulate
from simple_term_menu import TerminalMenu
import argparse
import ipaddress
import socket
import urc_pb2


STATUS_LIGHT_CHOICE = "Status Light"



QUIT_CHOICE = "Quit"
EDIT_CHOICE = "Edit"
CONFIRM_CHOICE = "Confirm"
BACK_CHOICE = "Back"

SERVER_IP = '127.0.0.1'
PORT = 8443

server_address = (SERVER_IP, PORT)
udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

def status_light_config():

    header = ["Light", "Enabled", "Blink"]
    table = [
        ["Red", 0, 0],
        ["Blue", 0, 0],
        ["Green", 0, 0]
    ]

    while True:

        print("Send the following command?")
        print()
        print(tabulate(table, headers=header))
        print()

        options = [EDIT_CHOICE, CONFIRM_CHOICE, BACK_CHOICE]
        menu = TerminalMenu(options)
        choice_idx = menu.show()

        if options[choice_idx] == EDIT_CHOICE:
            print("Select the row you want to edit")
            options = [row[0] for row in table]
            menu = TerminalMenu(options)
            row_idx = menu.show()

            print("Select the column you want to edit")
            options = [header[i] for i in range(1, len(header))]
            menu = TerminalMenu(options)
            col_idx = menu.show()
            col_idx = col_idx + 1

            print("Select the new value")
            options = ["0", "1"]
            menu = TerminalMenu(options)
            val_idx = menu.show()

            table[row_idx][col_idx] = int(options[val_idx])
        elif options[choice_idx] == CONFIRM_CHOICE:
            
            message = urc_pb2.TeensyMessage()
            message.messageID = 1
            message.statusLightCommand.redEnabled = table[0][1]
            message.statusLightCommand.blueEnabled = table[1][1]
            message.statusLightCommand.greenEnabled = table[2][1]
            message.statusLightCommand.redBlink = table[0][2]
            message.statusLightCommand.blueBlink = table[1][2]
            message.statusLightCommand.greenBlink = table[2][2]

            payload = message.SerializeToString()
            udp_socket.sendto(payload, server_address)


            # # OLD PROTOBUF MESSAGE
            # message = urc_pb2.NewStatusLightCommand()
            # message.redEnabled = table[0][1]
            # message.blueEnabled = table[1][1]
            # message.greenEnabled = table[2][1]

            # payload = b'\x10' + message.SerializeToString()
            # print(payload)
            # udp_socket.sendto(payload, server_address)
        else:
            return


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


if __name__ == "__main__":

    parser = argparse.ArgumentParser(description="Drivetrain Control Panel")
    parser.add_argument("-E", type=str, help="Teensy endpoint. Example: 192.168.1.168:8443")

    args = parser.parse_args()

    if not args.E:
        print("No endpoint given!")
        exit()

    server_address = validate_ip_address(args.E)

    print("Drivetrain Control Panel")
    
    while True:
        options = [STATUS_LIGHT_CHOICE, QUIT_CHOICE]
        menu = TerminalMenu(options)
        choice_idx = menu.show()

        if options[choice_idx] == STATUS_LIGHT_CHOICE:
            status_light_config()
        else:
            break
