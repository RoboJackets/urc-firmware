import SoloPy as solo
import time

# instanciate a SOLO object:
mySolo = solo.SoloMotorControllerUart("COM3", 0, solo.UART_BAUD_RATE.RATE_937500)
print("Start")

while True:
    time.sleep(1)
    
    commandMode, error = mySolo.get_command_mode()   
    if error == solo.ERROR.NO_ERROR_DETECTED:
        break
    else:
        print("ERROR: " + str(error))
        
print("Setting kp, ki")
mySolo.set_speed_controller_kp(0.12)
mySolo.set_speed_controller_ki(0.005)   

print("Sending CW speed command")
mySolo.set_motor_direction(solo.DIRECTION.CLOCKWISE)
mySolo.set_speed_reference(1000)

time.sleep(5)
mySolo.set_speed_reference(0)

time.sleep(1)
print("Sending CCW speed command")
mySolo.set_motor_direction(solo.DIRECTION.COUNTERCLOCKWISE)
mySolo.set_speed_reference(1000)
    
time.sleep(5)
mySolo.set_speed_reference(0)

print("Done, disconnecting")
#ensure close the serial
mySolo.serial_close() 
print("Disconnected")