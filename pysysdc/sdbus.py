from typing import Callable

import logging
import time

import pysysdc
from pysysdc.sdreply import SDReply

class SDBus(object):
	def __init__(self, service_name: str = "", path: str = "", if_name: str = "", method_name: str = "", method_args: str = "", method_return: str = "", output_sig: str = "is") -> None:
		logging.info("pysysdc: init")
		self.service_name = service_name
		self.path = path
		self.if_name = if_name
		self.method_name = method_name
		self.method_return = method_return
		self.method_args = method_args
		self.output_sig = output_sig
		pysysdc.init()

class SDServer(SDBus):
	def __del__(self) -> None:
		logging.debug("server deinit")
		pysysdc.deinit_server()
	
	def set_func(self, cf: Callable) -> None:
		pysysdc.set_func(cf)

	def __service_register(self) -> bool:
		ret = -1
		while ret < 0:
			ret = pysysdc.service_register(self.path, self.if_name, self.method_name, self.method_return, self.method_args)
			if ret < 0:
				logging.error("pysysdc __service_register: '%s'", pysysdc.get_last_error(ret))
				pysysdc.deinit()
				time.sleep(3)

		return True

	def listen(self) -> tuple:
		self.__service_register()
		while True:
			ret = pysysdc.listen()
			if ret < 0:
				err = pysysdc.get_last_error(ret)
				logging.error("pysysdc listen: '%s'", err)
				return False, err

		return True, ''

class SDClient(SDBus):
	def __del__(self) -> None:
		logging.debug("client deinit")
		pysysdc.deinit_client()

	def send(self, service_name: str = "", 
					path: str = "", 
					if_name: str = "", 
					method_name: str = "", 
					method_args: str = "", 
					output_sig: str = "is",
					first_arg = "", 
					second_arg = "") -> SDReply:
		ret = pysysdc.send(
			service_name if service_name else self.service_name,
			path if path else self.path, 
			if_name if if_name else self.if_name, 
			method_name if method_name else self.method_name, 
			method_args if method_args else self.method_args,
			output_sig if output_sig else self.output_sig,
			first_arg,
			second_arg)

		if ret[0] < 0:
			err = pysysdc.get_last_error(ret[0])
			logging.error("pysysdc send: '%s'", err)

		logging.debug("pysysdc send: '%s'", ret)

		return SDReply(ret)
		#return {"sd_code": ret[0], "py_string": ret[2], "py_code": ret[1]}

	def unit_restart(self, unit_name: str) -> SDReply:
		return SDReply(pysysdc.unit_control(unit_name, "RestartUnit"))
	
	def unit_start(self, unit_name: str) -> SDReply:
		return SDReply(pysysdc.unit_control(unit_name, "StartUnit"))

	def unit_stop(self, unit_name: str) -> SDReply:
		return SDReply(pysysdc.unit_control(unit_name, "StopUnit"))
	
	def unit_reload(self, unit_name: str) -> SDReply:
		return SDReply(pysysdc.unit_control(unit_name, "RelaodUnit"))

	def unit_enable(self, unit_name: str) -> tuple:
		return pysysdc.unit_enable(unit_name)

	def unit_disable(self, unit_name: str) -> tuple:
		return pysysdc.unit_disable(unit_name)

	def unit_mask(self, unit_name: str) -> tuple:
		return pysysdc.unit_mask(unit_name)

	def unit_unmask(self, unit_name: str) -> tuple:
		return pysysdc.unit_unmask(unit_name)

	def unit_status(self, unit_name: str) -> tuple:
		return pysysdc.unit_status(unit_name)

	@staticmethod
	def set_hostname(method, value: str) -> bool:
		return pysysdc.set_hostname(method, value)

	@staticmethod
	def get_hostname(attr: str) -> tuple:
		return pysysdc.get_hostname(attr)

	@staticmethod
	def machine_reboot(wall_message: str="") -> bool:
		return pysysdc.machine_reboot(wall_message)

	@staticmethod
	def machine_poweroff(wall_message: str="") -> bool:
		return pysysdc.machine_poweroff(wall_message)

	@staticmethod
	def set_wall_message(wall_message: str="") -> bool:
		return pysysdc.set_wall_message(wall_message)

	@staticmethod
	def set_timezone(timezone: str) -> bool:
		return pysysdc.set_timezone(timezone)

	@staticmethod
	def daemon_reload() -> bool:
		return pysysdc.daemon_reload()