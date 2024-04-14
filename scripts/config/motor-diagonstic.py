import can
import time
from tabulate import tabulate
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
CALIBRATE_MOTOR_CHOICE = "Calibrate Motor"
READ_DATA_CHOICE = "Read Data"
OK_CHOICE = "OK"
CONTINUE_CHOICE = "Continue"
SKIP_CHOICE = "Skip"
STOP_CHOICE = "Stop"
ADD_MORE_CHOICE = "Add More"
QUIT_CHOICE = "Quit"
BACK_CHOICE = "Back"

FORMAT_SFXT = "SFXT"
FORMAT_INTEGER = "INTEGER"
FORMAT_HEX = "HEX"

def sfxtToFloat(input):

    if input <=  0x7FFE0000:
        return input / 131072.0
    else:
        invert = 0xFFFFFFFF - input + 1
        return invert / -131072.0
    
def extractDataFromPayload(payload):
    return payload[0] | (payload[1] << 8) | (payload[2] << 16) | (payload[3] << 24)

def main_menu():
    print("Choose an option:")
    options = [SCAN_BUS_CHOICE, TEST_MOTOR_CHOICE, CALIBRATE_MOTOR_CHOICE, READ_DATA_CHOICE, QUIT_CHOICE]
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

    if len(solos) == 0: 
        print("No SOLO UNOs detected!")
        return

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

    options = [OK_CHOICE, SKIP_CHOICE, QUIT_CHOICE]
    menu = TerminalMenu(options)
    choice_idx = menu.show()

    if options[choice_idx] == QUIT_CHOICE:
        print("Calibration canceled.")
        return

    if options[choice_idx] == OK_CHOICE:
        payload = [0x22, 0x07, 0x30, 0x00, 0x01, 0x00, 0x00, 0x00]
        sendSoloCommand(bus, buffer, id, payload, "Running Motor Parameters Identification")

        time.sleep(1.5)

        motor_id_list = []
        extract_func = lambda arr: arr[0] | (arr[1] << 8) | (arr[2] << 16) | (arr[3] << 24) 

        payload = [0x40, 0x17, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00]
        data = sendSoloCommand(bus, buffer, id, payload, "Current Controller Kp")
        motor_id_list.append(extract_func(data[id[0]]))

        payload = [0x40, 0x18, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00]
        data = sendSoloCommand(bus, buffer, id, payload, "Current Controller Ki")
        motor_id_list.append(extract_func(data[id[0]]))

        payload = [0x40, 0x0E, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00]
        data = sendSoloCommand(bus, buffer, id, payload, "Motor Inductance")
        motor_id_list.append(extract_func(data[id[0]]))

        payload = [0x40, 0x0D, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00]
        data = sendSoloCommand(bus, buffer, id, payload, "Motor Resistance")
        motor_id_list.append(extract_func(data[id[0]]))

        print(f"Kp={sfxtToFloat(motor_id_list[0])}")
        print(f"Ki={sfxtToFloat(motor_id_list[1]) / 20000.0}")
        print(f"Ind={sfxtToFloat(motor_id_list[2])}")
        print(f"Res={sfxtToFloat(motor_id_list[3])}")

    if motor == MOTOR_ODRIVE_270KV:
        payload = [0x22, 0x13, 0x30, 0x00, 0x01, 0x00, 0x00, 0x00]
        sendSoloCommand(bus, buffer, id, payload, "Sensor: Incremental Encoder")

        payload = [0x22, 0x10, 0x30, 0x00, 0x00, 0x08, 0x00, 0x00]
        sendSoloCommand(bus, buffer, id, payload, "Encoder Lines: 2048")

        print("Incremental Encoder Calibration")
        print("WARNING: motor will spin slowly")
        print("Select 'OK' to proceed.")

        options = [OK_CHOICE, SKIP_CHOICE, QUIT_CHOICE]
        menu = TerminalMenu(options)
        choice_idx = menu.show()

        if options[choice_idx] == QUIT_CHOICE:
            print("Calibration canceled.")
            return

        if options[choice_idx] == OK_CHOICE:
            payload = [0x22, 0x27, 0x30, 0x00, 0x01, 0x00, 0x00, 0x00]
            sendSoloCommand(bus, buffer, id, payload, "Incremental Encoder Calibration Start")

            time.sleep(20)

            payload = [0x22, 0x27, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00]
            sendSoloCommand(bus, buffer, id, payload, "Incremental Encoder Calibration Stop")

    elif motor == MOTOR_NEO_470KV:
        payload = [0x22, 0x13, 0x30, 0x00, 0x02, 0x00, 0x00, 0x00]
        sendSoloCommand(bus, buffer, id, payload, "Sensor: Hall Effect")

        print("Hall Sensor Calibration")
        print("WARNING: motor will spin slowly")
        print("Select 'OK' to proceed.")

        options = [OK_CHOICE, SKIP_CHOICE, QUIT_CHOICE]
        menu = TerminalMenu(options)
        choice_idx = menu.show()

        if options[choice_idx] == QUIT_CHOICE:
            print("Calibration canceled.")
            return
        
        if options[choice_idx] == OK_CHOICE:

            payload = [0x22, 0x27, 0x30, 0x00, 0x02, 0x00, 0x00, 0x00]
            sendSoloCommand(bus, buffer, id, payload, "Hall Sensor Calibration Start")

            time.sleep(10)

            payload = [0x22, 0x27, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00]
            sendSoloCommand(bus, buffer, id, payload, "Hall Sensor Calibration Stop")
    
    print()
    print("Initial calibration steps complete.")
    print("You may skip Motor Identification and Encoder Calibration when re-running calibration for this motor.")
    print()
    print("Phase Wire Ordering")
    print("Now, you must determine the correct ordering of the three phase wires. To do this, we will try to spin the motor in torque mode.")
    print("- There are 2 valid configurations.")
    print("- Choose the configuration where the motor spins the fastest")
    print("You will have to re-run this calibration script multiple times. Keep note of which phase combinations you have tried.")
    print("WARNING: motor MIGHT spin quickly. It also might do nothing.")
    print("Select 'OK' to activate the motor.")

    options = [OK_CHOICE, QUIT_CHOICE]
    menu = TerminalMenu(options)
    choice_idx = menu.show()

    if options[choice_idx] == QUIT_CHOICE:
        print("Calibration canceled.")
        return
    
    payload = [0x22, 0x04, 0x30, 0x00, 0x00, 0x00, 0x03, 0x00]
    sendSoloCommand(bus, buffer, id, payload, "Torque Reference 1.5")

    print("Press 'Stop' to stop the motor.")
    options = [STOP_CHOICE]
    menu = TerminalMenu(options)
    choice_idx = menu.show()

    payload = [0x22, 0x04, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00]
    sendSoloCommand(bus, buffer, id, payload, "Torque Reference 0")

    print()
    print("IF THE MOTOR SPUN")
    print("- Phase wires in valid order. You must press 'Continue'.")
    print()
    print("IF NOTHING HAPPENED")
    print("- Do the following:")
    print("  * Quit this calibration script.")
    print("  * Turn off power to the SOLO UNO.")
    print("  * Swap phase wires.")
    print("  * Power on SOLO UNO.")
    print("  * Restart this calibration script. You may skip Motor Identification and Encoder Calibration.")
    print("- DO NOT press 'Continue'")

    options = [QUIT_CHOICE, CONTINUE_CHOICE]
    menu = TerminalMenu(options)
    choice_idx = menu.show()

    if options[choice_idx] == QUIT_CHOICE:
        print("Calibration canceled.")
        return

    


def read_data(bus, buffer):
    solos = scan_bus(bus, buffer)

    if len(solos) == 0: 
        print("No SOLO UNOs detected!")
        return

    table = [[id] for id in solos]
    headers = ["ID"]
    command_code = 0

    while True:

        while True:
            user_input = input("Enter the Command Code corresponding to the data you want to read in hexadecimal: ")

            try:
                command_code = int(user_input, 16)

                if (command_code >= 0x3001) and (command_code <= 0x3044): 
                    print(f"Received: 0x{command_code:04x}")
                    break
                else:
                    print(f"Input '0x{command_code:04x}' is not a valid command code.")
            except ValueError:
                print("Invalid hexadecimal number.")

        print("What is the desired data format?")
        options = [FORMAT_SFXT, FORMAT_INTEGER, FORMAT_HEX]
        menu = TerminalMenu(options)
        choice_idx = menu.show()
        format = options[choice_idx]

        payload = [0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]
        payload[1] = 0x00FF & command_code
        payload[2] = (0xFF00 & command_code) >> 8
        data = sendSoloCommand(bus, buffer, solos, payload)


        formatted_header = f"0x{command_code:04x}"

        if formatted_header in headers:
            
            idx = headers.index(formatted_header)

            for i in range(len(table)):
                id = table[i][0]

                if id in data:

                    value = "BBBB"

                    if format is FORMAT_SFXT:
                        sfxt = extractDataFromPayload(data[id])
                        value = f"{sfxtToFloat(sfxt):.5f}"
                    elif format is FORMAT_INTEGER:
                        value = f"{extractDataFromPayload(data[id])}"
                    elif format is FORMAT_HEX:
                        value = f"0x{extractDataFromPayload(data[id]):08x}"

                    table[i][idx] = value
                else:
                    table[i][idx] = "X"
        else:
            headers.append(formatted_header)

            for i in range(len(table)):
                id = table[i][0]

                if id in data:
                    value = "BBBB"

                    if format is FORMAT_SFXT:
                        sfxt = extractDataFromPayload(data[id])
                        value = f"{sfxtToFloat(sfxt):.5f}"
                    elif format is FORMAT_INTEGER:
                        value = f"{extractDataFromPayload(data[id])}"
                    elif format is FORMAT_HEX:
                        value = f"0x{extractDataFromPayload(data[id]):08x}"

                    table[i].append(value)
                else:
                    table[i].append("X")
        

        print(tabulate(table, headers=headers))
        print()

        print("Read more data?")
        options = [ADD_MORE_CHOICE, QUIT_CHOICE]
        menu = TerminalMenu(options)
        choice_idx = menu.show()

        if options[choice_idx] == QUIT_CHOICE:
            return
    


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
            elif choice == CALIBRATE_MOTOR_CHOICE:
                calibrate_motor(bus, buffer)
            elif choice == READ_DATA_CHOICE:
                read_data(bus, buffer)
            else:
                break

        
    finally:
        notifier.stop()
        bus.shutdown()
