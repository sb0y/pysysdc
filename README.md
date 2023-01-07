# pysysdc [![Build Status](https://app.travis-ci.com/sb0y/pysysdc.svg?branch=main)](https://app.travis-ci.com/sb0y/pysysdc)

Python3.5+ sd-bus adapter which allows to manage services, publish and call methods.

Tested on Ubuntu 16.04 and Ubuntu 20.04.

## Idea
If you are orchestrating a large number of services on a Linux system, you have to run the command `service` or `systemctl` quite frequently.
If you want to do this programmatically, you have to start the process in most cases with a shell (to parse `systemctl` commands arguments) which sometimes may lead you to arguments symbols escaping.

What will do the `systemctl restart nginx.service` command in the end? This command will send message over SD-Bus to systemd daemon (PID 1) which will restart `nginx` service.
What if it is possible to send the message to systemd through the SD-Bus directly from Python? What if the same mechanism can be used for IPC?

## How to compile
```
sudo apt update
sudo apt install python3-setuptools python3-stdeb python3-all-dev libsystemd-dev
python3 ./setup.py build
```

## How to build DEB package
Just run `build.sh` and make sure you installed `fakeroot` package.
```
sudo apt update
sudo apt install fakeroot python3-setuptools python3-stdeb python3-all-dev libsystemd-dev
./build.sh
```

## How to use
For example you can reload `nginx` service in the following way:
```python
from pysysdc.unit import Unit

Unit("nginx").reload()
```

All supported unit methods can be found [here](https://github.com/sb0y/pysysdc/blob/main/pysysdc/unit.py#L32).

It's also possible to set your hostname through this library without any subprocess spawn, configuration editing and etc:
```python3
from pysysdc.methods import Methods

Methods.set_pretty_hostname("myhostname")
```
or reboot your machine
```python3
from pysysdc.methods import Methods

Methods.machine_reboot()
```
see other implemented systemd methods [here](https://github.com/sb0y/pysysdc/blob/ffa3b75bb782efdb167980a3a52b4ae4ec96ca0d/pysysdc/methods.py#L16).

Also you can publish method in you service.
```python
from pysysdc.sdbus import SDBus

data = "example data string"
def handler():
	return True, data

sd = SDBus(path="/dc/cloud/auth", if_name="dc.cloud.auth", method_name="Token", method_args="s", output_sig="s", method_return="s")
sd.set_func(handler)
sd.listen()
```

and call it from another program

```python
from pysysdc.sdbus import SDBus

sd = SDBus(
	path="/dc/cloud/auth", 
	if_name="dc.cloud.auth", 
	method_name="Token", 
	method_args="s", 
	method_return="s", 
	output_sig="s")
ret = sd.send(method_name="Token")
print(ret)
```
