import SoloPy as solo
import time

# instanciate a SOLO object:
mySolo = solo.SoloMotorControllerUart("COM3", 0, solo.UART_BAUD_RATE.RATE_937500)
print("Start")

while True:
    time.sleep(1)

    commandMode, error = mySolo.get_command_mode()   
    if error == solo.ERROR.NO_ERROR_DETECTED:
        print("Configuring")
    else:
        print("ERROR: " + str(error))

    if error == solo.ERROR.NO_ERROR_DETECTED:
        success_list = []
        success_list.append(mySolo.set_command_mode(solo.COMMAND_MODE.DIGITAL))
        success_list.append(mySolo.set_feedback_control_mode(solo.FEEDBACK_CONTROL_MODE.ENCODERS))
        success_list.append(mySolo.set_control_mode(solo.CONTROL_MODE.SPEED_MODE))
        success_list.append(mySolo.set_output_pwm_frequency_khz(20))
        success_list.append(mySolo.set_incremental_encoder_lines(2048))
        success_list.append(mySolo.set_current_limit(7.5))
        success_list.append(mySolo.set_motor_poles_counts(14))
        success_list.append(mySolo.set_motor_type(solo.MOTOR_TYPE.BLDC_PMSM))
        if all([e[0] for e in success_list]):
            break
        else:
            print("\n".join([error for bool,error in success_list]))
            print("\n\n\n\n\n")

print("Done, disconnecting")
#ensure close the serial
mySolo.serial_close() 
print("Disconnected")

