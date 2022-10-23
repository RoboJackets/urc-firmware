# Docker Installation

PlatformIO is cross-platform, so you shouldn't need to use Docker. However, Docker
is a reliable option if something isn't working.

## 1. Install Docker

[Windows Instructions](https://docs.docker.com/desktop/windows/install/)

[Mac Instructions](https://docs.docker.com/desktop/mac/install/)

[Ubuntu Instructions](https://docs.docker.com/engine/install/ubuntu/)

## 2. Install VSCode (Highly Recommended)

You do not have to use VSCode. However, VSCode has very nice extensions for using Docker containers.

### 2a. Install VSCode

[Download VSCode here](https://code.visualstudio.com/Download)

### 2b. Install VSCode Extensions

Search for and install the following extensions in VSCode

* Docker
* ROS
* Remote - Containers

## 3. Create Docker Image

Next, you need to create the Docker image. To do this, you need to manually create the Docker image from our Dockerfile.

To do the manual installation, you need to download the Dockerfile in the `urc-software` repository. If you have `git` installed, you can just clone the whole repository:
```bash
git clone https://github.com/RoboJackets/urc-firmware.git
cd urc-software
```
If you don't have `git` or don't want to download the whole repository, you can just save [this](https://raw.githubusercontent.com/RoboJackets/urc-firmware/master/Dockerfile) as `Dockerfile` on your computer.

Once you are in the same directory as our Dockerfile, run:
```bash
docker build -t robojackets/urc-firmware-baseimage .
```

## 4. Create Docker Container

To create a Docker container from the newly created image `robojackets/urc-baseimage`, run

```bash
docker container create -i -t --name=urc_firmware_container robojackets/urc-firmware-baseimage
```

## 5. Launch Docker Container

You can launch your newly created Docker container using the command line or VSCode.

### 5a. Launch Docker Conatiner Using VSCode

First, make sure that you have the `Remote - Containers` extension installed.

Once you have it, click on the Remote Explorer tab. You should see the container you made in the "containers" tab. 

To launch the container in VSCode, right click the container and select `Attach to Container`. This will open VSCode in the Docker container.

### 5b. Launch Docker Container Using Command Line

To run a Docker container in the command line, run

```bash
docker start -i urc_firmware_container
```

If you want to open another terminal if the Docker container is still running, run

```bash
docker attach urc_firmware_container
```

## 6. Finish Setup

In its current state, the Docker container is not quite ready for building the `urc-software` codebase. 

### Update the .bashrc

```bash
cd /root
echo 'export PATH=$PATH:~/.platformio/penv/bin' >> /root/.bashrc
source /root/.bashrc
```

### Re-clone `urc-software` in `/colcon_ws/src`

```bash
cd /root
git clone https://github.com/RoboJackets/urc-firmware.git --recursive
```
### Build

```bash
cd /root/urc-firmware
pio run
```

## 7. Closing the Container

Once you are done with the conatiner, be sure to close the Docker container. Otherwise, the 
Docker container will take up a big chunk of memory on your computer.

Important Note: DO NOT delete the Docker container! The container saves its current state, so all of your
work is also preserved inside the container as well! Only delete the container if you somehow mess up your
environment. Then, you can just create a new container from the `robojackets/urc-baseimage`.

### 7a. Close Docker Conatiner Using VSCode

Closing the VSCode window does not stop the Docker container. To stop the Docker container, you can either:
* go to the `Remote Explorer` tab, right click the active Docker container, and hit `Stop Container`.
* go to the `Docker` tab, and toggle the green arrow on the active container.

### 7b. Close Docker Container Using Command Line

Close the Docker container using

```bash
docker stop urc_firmware_container
```

### [Return to Documents](../README.md)