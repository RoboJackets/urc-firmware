# Firmware Installation Instructions

These instructions should work for all platforms. They involve downloading the proper
Arduino IDE version and installing Teensyduino.

## 1. Install the Arduino IDE version 1.8.19

[Install here](https://www.arduino.cc/en/software)

Be sure to move the Arduino IDE out of your Downloads folder and into a more permanent place, like your home directory!

## 2. Install Teensyduino

[Follow the instructions here](https://www.pjrc.com/teensy/td_download.html)

Remember to right click the udev rules and use save-as to easily download them!

The Teensyduino Installer requires you to manually select the location of your Arduino IDE installation.

## 2. Clone this repository

```bash
git clone https://github.com/RoboJackets/urc-firmware.git
```

```bash
cd urc-firmware
```

```bash
git submodule update --init
```

## 3. Compiling a .ino file

Before hitting `Verify` in the Arduino IDE, make sure that `Teensy 4.1` is selected 
in `Tools > Board`.

If the compiler gives you a `Nanopb`-related error, you need to install the nanopb library.
Follow steps 2 and 3 [here](../helpers/compiling_nanopb_with_arduino.md) for help with nanopb installation