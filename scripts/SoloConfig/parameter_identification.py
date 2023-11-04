import SoloPy as solo
import time
import argparse

parser = argparse.ArgumentParser(description="Run SoloUNO parameter identification routine")
parser.add_argument('port', type=str, help="USB port")
args = parser.parse_args()

# instanciate a SOLO object:
mySolo = solo.SoloMotorControllerUart(args.port, 0, solo.UART_BAUD_RATE.RATE_937500)
print("Start")

while True:
    time.sleep(1)
    
    commandMode, error = mySolo.get_command_mode()   
    if error == solo.ERROR.NO_ERROR_DETECTED:
        break
    else:
        print("ERROR: " + str(error))
        
print("Identifying")
result = mySolo.motor_parameters_identification(solo.ACTION.START)

time.sleep(1)

if result[0]:
    print("KI:", mySolo.get_current_controller_ki())
    print("KP:", mySolo.get_current_controller_kp())
    print("Inductance:", mySolo.get_motor_inductance())
    print("Resistance:", mySolo.get_motor_resistance())
else:
    print("ERROR:", result[1])
    
time.sleep(1)
    
print("Done, disconnecting")
#ensure close the serial
mySolo.serial_close() 
print("Disconnected")