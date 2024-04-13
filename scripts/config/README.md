# Motor Diagnostic CAN tool

Tested with the Innomaker USB2CAN dongle on Linux using python-can.

### Linux Installation

Tested on Ubuntu 22.04 LTS.

```bash
sudo apt update
sudo apt install can-utils
```

```bash
sudo cp 80-can.network /etc/systemd/network/
sudo systemctl restart systemd-networkd
sudo systemctl enable systemd-networkd

```


### WSL Installation

https://gist.github.com/yonatanh20/664f07d62eb028db18aa98b00afae5a6