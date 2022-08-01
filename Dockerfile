FROM ubuntu:jammy

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update --fix-missing && apt-get -y install \
        apt-utils \
        git \
        wget \
        python3 \
        python3-venv \
        && apt-get clean

COPY . .

RUN export PATH=$PATH:~/.platformio/penv/bin && \
    wget https://raw.githubusercontent.com/platformio/platformio-core-installer/master/get-platformio.py -O get-platformio.py && \
    python3 get-platformio.py && \
    pio platform install "platformio/teensy" && \
    pio run