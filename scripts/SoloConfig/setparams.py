import SoloPy as solo
import time
import argparse

# 0: Unplug motor from Solo Uno.
# 1: Set the following settings:
#  a. Command Mode: Digital
#  b. Control Type: Speed
#  c. Device Address: 161-166 (i.e. A1 - A6)
#  d. Motor Type: BLDC-PMSM
#  e. PWM Frequency: 20 kHz
#  f. Number of Poles: 14
#  g. Current Limit: 7.5
# 2: For the following settings, click the lock to edit the values
#  a. Current Controller Kp: 0.20
#  b. Current Controller Ki: 0.00165
#  c. Motor Inductance: 
# 2: Turn off the Solo Uno, plug in the motor.
# 2: Set can ID in motion terminal (IDs=160-166).
# 3: Run this script to set important values.
# 4: Do "Motor Identification" in Motion Terminal.
# 5: 

parser = argparse.ArgumentParser(description="Set SoloUNO constants")
parser.add_argument('port', type=str, help="USB port")
args = parser.parse_args()

mySolo = solo.SoloMotorControllerUart(args.port, 0, solo.UART_BAUD_RATE.RATE_937500)
print("Setting Constants")

while True:
    time.sleep(1)

    commandMode, error = mySolo.get_command_mode()  
    if error == solo.ERROR.NO_ERROR_DETECTED:
        print("Configuring")
    else:
        print("ERROR: " + str(error))

    if error == solo.ERROR.NO_ERROR_DETECTED:
        success_list = []

        # configurations
        success_list.append(mySolo.set_command_mode(solo.COMMAND_MODE.DIGITAL))
        success_list.append(mySolo.set_output_pwm_frequency_khz(20))
        success_list.append(mySolo.set_motor_type(solo.MOTOR_TYPE.BLDC_PMSM))
        success_list.append(mySolo.set_current_limit(7.5))
        success_list.append(mySolo.set_motor_poles_counts(14))

        # controls
        success_list.append(mySolo.set_feedback_control_mode(solo.FEEDBACK_CONTROL_MODE.ENCODERS))
        success_list.append(mySolo.set_control_mode(solo.CONTROL_MODE.SPEED_MODE))
        success_list.append(mySolo.set_incremental_encoder_lines(2048))
        
        # motor identification
        success_list.append(mySolo.set_current_controller_kp(0.20))
        success_list.append(mySolo.set_current_controller_ki(0.0015))
        success_list.append(mySolo.set_motor_inductance(0.00022))
        success_list.append(mySolo.set_motor_resistance(0.024))
        
        if all([e[0] for e in success_list]):
            break
        else:
            print("\n".join([error for bool,error in success_list]))
            print("\n\n\n\n\n")

print("Disconnecting")
mySolo.serial_close()
time.sleep(3)

