import SoloPy as solo
import time
import argparse

parser = argparse.ArgumentParser(description="Run SoloUNO encoder calibration")
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
mySolo.sensor_calibration(solo.POSITION_SENSOR_CALIBRATION_ACTION.INCREMENTAL_ENCODER_START_CALIBRATION)
time.sleep(20)
result = mySolo.sensor_calibration(solo.POSITION_SENSOR_CALIBRATION_ACTION.STOP_CALIBRATION)


time.sleep(1)

if result[0]:
    print("CCW Offset:", mySolo.get_encoder_hall_ccw_offset())
    print("CW Offset:", mySolo.get_encoder_hall_cw_offset())
    mySolo.set_control_mode(solo.CONTROL_MODE.SPEED_MODE)
else:
    print("ERROR:", result[1])
    
time.sleep(1)
    
print("Done, disconnecting")
#ensure close the serial
mySolo.serial_close() 
print("Disconnected")