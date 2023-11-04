# How to configure motor controllers

0. Unplug motor from Solo Uno.
1. Turn on the Solo Uno. Set the following settings:
* Command Mode: Digital
* Control Type: Torque (Maybe its safe?)
* Device Address: 161-166 (i.e. A1 - A6)
* Motor Type: BLDC-PMSM
* PWM Frequency: 20 kHz
* Number of Poles: 14
* Current Limit: 7.5
* Feedback Control Mode: Sensor Less
2. For the following settings, click the lock to edit the values. If these values are out of range / incorrect, the motor controller could explode!
* Current Controller Kp: 0.20
* Current Controller Ki: 0.00165
* Motor Inductance: 0.00022
* Motor Resistance: 0.024
3. Turn off the Solo Uno, plug in the motor.
4. Run "Motor Identification" in Motion Terminal. This will set the correct values for Current Controller Kp, Current Controller Ki, Motor Inductance, and Motor Resistance.
5. Set the following settings:
* Feedback Control Mode: Using Encoders
* Encoder Lines: 2048
6. Run "Encoder Calibration". 
7. Set the following settings:
* Control Type: Speed
* Speed Controller Kp: 0.12
* Speed Controller Ki: 0.005
8. The Solo is now ready for regular use.
