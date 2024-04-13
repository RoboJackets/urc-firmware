import can
import time
from simple_term_menu import TerminalMenu

CAN_BAUD_1000KBPS = 1000000
CAN_BAUD_500KBPS = 500000
CAN_BAUD_250KBPS = 250000
CAN_BAUD_125KBPS = 125000
CAN_BAUD_100KBPS = 100000

FACTORY_BAUD_RATE = CAN_BAUD_1000KBPS
DEFAULT_BAUD_RATE = CAN_BAUD_500KBPS

MOTOR_ODRIVE_270KV = "ODRIVE 270KV"
MOTOR_NEO_470KV = "NEO 470KV"

SCAN_BUS_CHOICE = "Scan Bus"
TEST_MOTOR_CHOICE = "Test Motor"
FACTORY_RESET_CHOICE = "Factory Reset"
CALIBRATE_MOTOR = "Calibrate Motor"
OK_CHOICE = "OK"
QUIT_CHOICE = "Quit"
BACK_CHOICE = "Back"

def sfxtToFloat(input):

    if input >=  0x7FFE0000:
        return input / 131072.0
    else:
        invert = 0xFFFFFFFF - input + 1
        return invert / -131072.0


def main_menu():
    print("Choose an option:")
    options = [SCAN_BUS_CHOICE, TEST_MOTOR_CHOICE, CALIBRATE_MOTOR, QUIT_CHOICE]
    menu = TerminalMenu(options)
    return options[menu.show()]


def sendSoloCommand(bus, bufferedReader, idList, payload, message = "Got Message", debug = False, data_func = None, delay_after_send = 0):

    bus.flush_tx_buffer()

    sendIDs = [(0x600 + id) for id in idList]
    responseIDs = [(0x580 + id) for id in idList]

    for id in sendIDs:
        bus.send(can.Message(arbitration_id=id, data=payload, is_extended_id=False))
        time.sleep(0.01)

    time.sleep(delay_after_send)

    data_dict = {}

    while True:
        msg = bufferedReader.get_message(timeout=0.1)
        if msg is None:
            break

        if msg.arbitration_id in responseIDs:

            id = msg.arbitration_id - 0x580
            data_dict[id] = [msg.data[4], msg.data[5], msg.data[6], msg.data[7]]

            if debug:
                if data_func is None:
                    data_str = ''.join(f'{e:x}' for e in msg.data)
                    print(f" {message} [id=0x{id:x}, data=0x{data_str}]")
                else:
                    print(f" {message} [id=0x{id:x}, {data_func(msg.data)}]")

    return data_dict
    

def scan_bus(bus, buffer):
    
    print("Scanning bus...")
    
    ids = [i for i in range(0, 255)]
    payload = [0x40, 0x3A, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00]
    version_func = lambda arr: f'version={arr[7]:02x}{arr[6]:02x}{arr[5]:02x}{arr[4]:02x}'
    data = sendSoloCommand(bus, buffer, ids, payload, "Found SOLO UNO", True, version_func, 0)
    return list(data.keys())

    
def test_motor(bus, buffer):

    print("Testing motor...")

    ids = [0x0A1, 0x0A2, 0x0A3, 0x0A4, 0x0A5, 0x0A6]
    
    payload = [0x22, 0x16, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00]
    sendSoloCommand(bus, buffer, ids, payload, "Speed Control Set")

    payload = [0x22, 0x0C, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00]
    sendSoloCommand(bus, buffer, ids, payload, "Rotating CCW", True)

    payload = [0x22, 0x05, 0x30, 0x00, 0xD0, 0x07, 0x00, 0x00]
    sendSoloCommand(bus, buffer, ids, payload, "Speed Ref = 2000")

    time.sleep(3)

    payload = [0x22, 0x0C, 0x30, 0x00, 0x01, 0x00, 0x00, 0x00]
    sendSoloCommand(bus, buffer, ids, payload, "Rotating CW", True)

    time.sleep(3)

    payload = [0x22, 0x05, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00]
    sendSoloCommand(bus, buffer, ids, payload, "Speed Ref = 0")

    payload = [0x22, 0x16, 0x30, 0x00, 0x01, 0x00, 0x00, 0x00]
    sendSoloCommand(bus, buffer, ids, payload, "Torque Control Set")
    

def factory_reset(bus, buffer):

    solos = scan_bus(bus, buffer)
    print("Select which motor controller you want to factory reset:")

    options = [f'0x{id:x}' for id in solos]
    options.append(BACK_CHOICE)
    menu = TerminalMenu(options)
    choice_idx = menu.show()

    if options[choice_idx] == BACK_CHOICE:
        print("No SOLO UNO reset.")
        return
    
    id = [solos[choice_idx]]
    payload = [0x22, 0x14, 0x30, 0x00, 0x01, 0x00, 0x00, 0x00]
    sendSoloCommand(bus, buffer, id, payload, "Factory Reset SOLO UNO", True)

    time.sleep(3)

    payload = [0x22, 0x2C, 0x30, 0x00, 0x01, 0x00, 0x00, 0x00]
    sendSoloCommand(bus, buffer, id, payload, "Set CAN baud to 500kbps", True)

    payload = [0x22, 0x01, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00]
    payload[5] = 0x00FF & id[0]
    payload[6] = 0xFF00 & id[0]
    sendSoloCommand(bus, buffer, id, payload, f"Set Device Address to 0x{id[0]:x}", True)

    print("Please power cycle SOLO UNO for changes to take effect.")


def set_baud_rate():
    print("What is the CAN baud rate? (Note: upon factory reset, baud rate is 1000kbps)")

    bauds = [CAN_BAUD_1000KBPS, CAN_BAUD_500KBPS, CAN_BAUD_250KBPS, CAN_BAUD_125KBPS, CAN_BAUD_100KBPS]
    options = [f'{int(b/1000)}kbps' for b in bauds]
    menu = TerminalMenu(options)
    choice_idx = menu.show()
    
    return bauds[choice_idx]


def calibrate_motor(bus, buffer):

    solos = scan_bus(bus, buffer)
    print("Select which motor controller you want to calibrate:")

    options = [f'0x{id:x}' for id in solos]
    options.append(BACK_CHOICE)
    menu = TerminalMenu(options)
    choice_idx = menu.show()

    if options[choice_idx] == BACK_CHOICE:
        print("Calibration canceled.")
        return
    
    id = [solos[choice_idx]]

    print(f"What kind of motor is connected to 0x{id[0]:x}?")
    options = [MOTOR_ODRIVE_270KV, MOTOR_NEO_470KV]
    menu = TerminalMenu(options)
    choice_idx = menu.show()
    motor = options[choice_idx]

    payload = [0x22, 0x02, 0x30, 0x00, 0x01, 0x00, 0x00, 0x00]
    sendSoloCommand(bus, buffer, id, payload, "Command Mode Digital")

    payload = [0x22, 0x3F, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00]
    sendSoloCommand(bus, buffer, id, payload, "Motion Profile: Step/Ramp Response")

    payload = [0x22, 0x16, 0x30, 0x00, 0x01, 0x00, 0x00, 0x00]
    sendSoloCommand(bus, buffer, id, payload, "Torque Control Set")

    payload = [0x22, 0x15, 0x30, 0x00, 0x01, 0x00, 0x00, 0x00]
    sendSoloCommand(bus, buffer, id, payload, "Motor Type: BLDC-PMSM")

    payload = [0x22, 0x09, 0x30, 0x00, 0x14, 0x00, 0x00, 0x00]
    sendSoloCommand(bus, buffer, id, payload, "PWM Frequency: 20kHz")

    payload = [0x22, 0x0F, 0x30, 0x00, 0x0E, 0x00, 0x00, 0x00]
    sendSoloCommand(bus, buffer, id, payload, "Number of Poles: 14")

    payload = [0x22, 0x03, 0x30, 0x00, 0x00, 0x00, 0x0F, 0x00]
    sendSoloCommand(bus, buffer, id, payload, "Current Limit: 7.5")

    print("Motor Identification Procedure")
    print("WARNING: motor will vibrate!")
    print("Select 'OK' to proceed.")

    options = [OK_CHOICE, QUIT_CHOICE]
    menu = TerminalMenu(options)
    choice_idx = menu.show()

    if options[choice_idx] == QUIT_CHOICE:
        print("Calibration canceled.")
        return

    payload = [0x22, 0x07, 0x30, 0x00, 0x01, 0x00, 0x00, 0x00]
    sendSoloCommand(bus, buffer, id, payload, "Running Motor Parameters Identification")

    time.sleep(1.5)

    payload = [0x40, 0x3A, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00]
    extract_func = lambda arr: arr[4] | (arr[5] << 8) | (arr[6] << 16) | (arr[7] << 24) 
    format_func = lambda arr: f'{sfxtToFloat(extract_func(arr))}'

    payload = [0x40, 0x17, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00]
    data = sendSoloCommand(bus, buffer, id, payload, "Current Controller Kp", format_func, 0)
    my_extract_func = lambda arr: arr[0] | (arr[1] << 8) | (arr[2] << 16) | (arr[3] << 24) 
    print(f'data=0x{my_extract_func(data[id[0]]):08x}')




if __name__ == "__main__":

    print("Walli motor configuration tool")

    # baud = set_baud_rate()

    bus = can.interface.Bus(channel='can0', bustype='socketcan', baud=DEFAULT_BAUD_RATE)
    buffer = can.BufferedReader()
    notifier = can.Notifier(bus, [buffer])

    bus.flush_tx_buffer()

    try:
        while True:
            choice = main_menu()

            if choice == SCAN_BUS_CHOICE:
                scan_bus(bus, buffer)
            elif choice == TEST_MOTOR_CHOICE:
                test_motor(bus, buffer)
            elif choice == CALIBRATE_MOTOR:
                calibrate_motor(bus, buffer)
            else:
                break

        
    finally:
        notifier.stop()
        bus.shutdown()
