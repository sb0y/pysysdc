# pysysdc [![Travis Build Status](https://app.travis-ci.com/sb0y/pysysdc.svg?branch=main)](https://app.travis-ci.com/sb0y/pysysdc) ![Github Actions Status](https://github.com/sb0y/pysysdc/actions/workflows/python-publish.yml/badge.svg)


Python3.5+ sd-bus adapter which allows to manage services, publish and call methods, natively control systemd units.

Tested on Ubuntu 16.04 and Ubuntu 20.04.

## Idea
If you are orchestrating a large number of services on a Linux system, you have to run the command `service` or `systemctl` quite frequently.
If you want to do this programmatically, you have to start the process in most cases with a shell (to parse `systemctl` commands arguments) which sometimes may lead you to arguments symbols escaping.

What will do the `systemctl restart nginx.service` command in the end? This command will send message over SD-Bus to systemd daemon (PID 1) which will restart `nginx` service.
What if it is possible to send the message to systemd through the SD-Bus directly from Python? What if the same mechanism can be used for IPC?

## How to compile
```
sudo apt update
sudo apt install python3-setuptools python3-stdeb python3-all-dev libsystemd-dev dh-python
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
import pysysdc.systemctl as systemctl

systemctl(svc="nginx", action="reload")
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
from pysysdc.sdbus import SDServer

token_data = "Zm9yaDhhOWViZWJjMjYxZmU1MTVlOTlhNjczMWE3NWQ3N2ZiOjoyMDQ3NzY6Omh0dHA6Ly9yZWdpb25zZWZmZWN0aXZlLmNvbS86Ojo6MTUxMDczMjE6OjY2NDcxNDo6MDo6MTo6MTo6OjowOjpkZWZhdWx0Ojow"
def token(arg: str) -> tuple:
	print("Hello from handler token().")
	print("Function runned with argument: '%s'." % arg)
	return True, token_data

def config(mode: str, data: str) -> tuple:
	print("Hello from handler config().")
	print("Mode: '%s'" % mode)
	print("Data: '%s'" % data)
	return True, 'done', 'test', 10

sd = SDServer(
	(
		{"path": "/dc/cloud/auth", 
		"if_name": "dc.cloud.auth", 
		"method_name": "Token", 
		"input_sig": "s", 
		"output_sig": "is", 
		"method_handler": token},

		{"path": "/dc/cloud/auth",
		"if_name": "dc.cloud.auth.config", 
		"method_name": "Config", 
		"input_sig": "ss", 
		"output_sig": "issi", 
		"method_handler": config},
	)

)
sd.listen() # blocking
```

and call it from another program

```python
from pysysdc.sdbus import SDClient

sd = SDClient(
	service_name="dc.cloud.auth",
	path="/dc/cloud/auth",
	if_name="dc.cloud.auth",
	method_name="Token",
	input_sig="s",
	output_sig="is"
)
ret = sd.send(method_name="Token", first_arg="get")
print(ret)
print(repr(ret))
```
