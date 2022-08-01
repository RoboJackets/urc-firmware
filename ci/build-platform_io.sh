#!/bin/bash

# Exit immediately if a command exits with a non-zero status.
set -e 
#Enable the globstar shell option
shopt -s globstar
# Make sure we are inside the github workspace
cd $GITHUB_WORKSPACE

export PATH=$PATH:~/.platformio/penv/bin
wget https://raw.githubusercontent.com/platformio/platformio-core-installer/master/get-platformio.py -O get-platformio.py
python3 get-platformio.py
pio platform install "platformio/teensy"
pio run