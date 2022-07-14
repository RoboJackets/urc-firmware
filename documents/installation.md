# Installation Instructions

These instructions should work for all platforms. It involves downloading the proper
Arduino IDE and installing Teensyduino.

## 1. Install the Arduino IDE version 1.8.19 and Teensyduino

Follow the instructions [here](https://www.pjrc.com/teensy/td_download.html).

NOTE 1: You must use the Arduino IDE version 1.8.19.

NOTE 2: Be sure to move the Arduino IDE out of your Downloads folder and into a more permanent place, like your home directory. The Teensyduino Installer requires you to manually select the location of your Arduino IDE installation.

## 2. Clone this repository

Once you install the Arduino IDE version 1.8.19 and Teensyduino, clone this repository:
```
git clone https://github.com/RoboJackets/urc-firmware.git
```
```
cd urc-firmware
```

```
git submodule init && git submodule update
```

## 3. Compiling a .ino file

Before hitting `Verify` in the Arduino IDE, make sure that `Teensy 4.1` is selected 
in `Tools > Board`.

If the compiler gives you a `Nanopb`-related error, you need to install the `Nanopb` library. 
Follow steps 2 and 3 [here](compiling_nanopb_with_arduino.md) to install the library.