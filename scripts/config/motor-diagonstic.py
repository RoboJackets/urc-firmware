import can
import time
from simple_term_menu import TerminalMenu

SCAN_BUS_CHOICE = "Scan Bus"
TEST_MOTOR_CHOICE = "Test Motor"
QUIT_CHOICE = "Quit"


def main_menu():
    print("Choose an option:")
    options = [SCAN_BUS_CHOICE, TEST_MOTOR_CHOICE, QUIT_CHOICE]
    menu = TerminalMenu(options)
    return options[menu.show()]

def scan_bus(bus, buffer):

    print("Scanning bus...")
    ids = [0x6A1, 0x6A2, 0x6A3, 0x6A4, 0x6A5, 0x6A6]

    for id in ids:
        bus.send(can.Message(arbitration_id=id, data=[0x22, 0x20, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00], is_extended_id=False))

    while True:
        msg = buffer.get_message(timeout=0.1)
        if msg is None:
            break
        if msg.arbitration_id in [id & 0xF2F for id in ids]:
            version_str = ''.join(f'{e:x}' for e in msg.data)
            print(f" Found SOLO UNO [id=0x{msg.arbitration_id:x}, version=0x{version_str}]")
    
def test_motor(bus, buffer):

    print("Testing motor...")

    ids = [0x6A1, 0x6A2, 0x6A3, 0x6A4, 0x6A5, 0x6A6]

    # Speed mode
    for id in ids:
        bus.send(can.Message(arbitration_id=id, data=[0x22, 0x16, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00], is_extended_id=False))

    while True:
        msg = buffer.get_message(timeout=0.1)
        if msg is None:
            break
        if (msg.arbitration_id in [id & 0xF2F for id in ids]):
            data_str = ''.join(f'{e:x}' for e in msg.data)
            print(f" Speed Control Set [id=0x{msg.arbitration_id:x}, data=0x{data_str}]")

    # direction CCW
    for id in ids:
        bus.send(can.Message(arbitration_id=id, data=[0x22, 0x0C, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00], is_extended_id=False))

    while True:
        msg = buffer.get_message(timeout=0.1)
        if msg is None:
            break
        if (msg.arbitration_id in [id & 0xF2F for id in ids]):
            data_str = ''.join(f'{e:x}' for e in msg.data)
            print(f" Rotating CCW [id=0x{msg.arbitration_id:x}, data=0x{data_str}]")

    # speed ref = 2000
    for id in ids:
        bus.send(can.Message(arbitration_id=id, data=[0x22, 0x05, 0x30, 0x00, 0xD0, 0x07, 0x00, 0x00], is_extended_id=False))

    while True:
        msg = buffer.get_message(timeout=0.1)
        if msg is None:
            break
        if (msg.arbitration_id in [id & 0xF2F for id in ids]):
            data_str = ''.join(f'{e:x}' for e in msg.data)
            print(f" Speed Ref = 2000 [id=0x{msg.arbitration_id:x}, data=0x{data_str}]")

    time.sleep(3)

    # direction CW
    for id in ids:
        bus.send(can.Message(arbitration_id=id, data=[0x22, 0x0C, 0x30, 0x00, 0x01, 0x00, 0x00, 0x00], is_extended_id=False))

    while True:
        msg = buffer.get_message(timeout=0.1)
        if msg is None:
            break
        if (msg.arbitration_id in [id & 0xF2F for id in ids]):
            data_str = ''.join(f'{e:x}' for e in msg.data)
            print(f" Rotating CCW [id=0x{msg.arbitration_id:x}, data=0x{data_str}]")

    time.sleep(3)

    # speed ref = 0
    for id in ids:
        bus.send(can.Message(arbitration_id=id, data=[0x22, 0x05, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00], is_extended_id=False))

    while True:
        msg = buffer.get_message(timeout=0.1)
        if msg is None:
            break
        if (msg.arbitration_id in [id & 0xF2F for id in ids]):
            data_str = ''.join(f'{e:x}' for e in msg.data)
            print(f" Speed Ref = 0 [id=0x{msg.arbitration_id:x}, data=0x{data_str}]")

    # Torque mode
    for id in ids:
        bus.send(can.Message(arbitration_id=id, data=[0x22, 0x16, 0x30, 0x00, 0x01, 0x00, 0x00, 0x00], is_extended_id=False))

    while True:
        msg = buffer.get_message(timeout=0.1)
        if msg is None:
            break
        if (msg.arbitration_id in [id & 0xF2F for id in ids]):
            data_str = ''.join(f'{e:x}' for e in msg.data)
            print(f" Torque Control Set [id=0x{msg.arbitration_id:x}, data=0x{data_str}]")
    



if __name__ == "__main__":

    print("Walli motor configuration tool")

    bus = can.interface.Bus(channel='can0', bustype='socketcan', baud=500000)
    buffer = can.BufferedReader()
    notifier = can.Notifier(bus, [buffer])

    try:
        while True:
            choice = main_menu()

            if choice == SCAN_BUS_CHOICE:
                scan_bus(bus, buffer)
            elif choice == TEST_MOTOR_CHOICE:
                test_motor(bus, buffer)
            else:
                break

        
    finally:
        notifier.stop()
        bus.shutdown()
