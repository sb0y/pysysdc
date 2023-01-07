import logging
import time
import pysysdc

class SDBus(object):

	def __init__(self, service_name: str = "", path: str = "", if_name: str = "", method_name: str = "", method_args: str = "", method_return: str = "", output_sig: str = "s"):
		logging.info("pysysdc: init")
		self.service_name = service_name
		self.path = path
		self.if_name = if_name
		self.method_name = method_name
		self.method_return = method_return
		self.method_args = method_args
		self.output_sig = output_sig
		pysysdc.init()

	def __del__(self):
		pysysdc.deinit()

	def set_func(self, cf):
		pysysdc.set_func(cf)

	def __service_register(self):
		ret = -1
		while ret < 0:
			ret = pysysdc.service_register(self.path, self.if_name, self.method_name, self.method_return, self.method_args)
			if ret < 0:
				logging.error("pysysdc: '%s'", pysysdc.get_last_error(ret))
				pysysdc.deinit()
				time.sleep(3)

		return True

	def listen(self):
		self.__service_register()
		while True:
			ret = pysysdc.listen()
			if ret < 0:
				err = pysysdc.get_last_error(ret)
				logging.error("pysysdc: '%s'", err)
				return False, err

		return True, ''

	def send(self, service_name: str = "", 
					path: str = "", 
					if_name: str = "", 
					method_name: str = "", 
					method_args: str = "", 
					output_sig: str = "s",
					first_arg = "", 
					second_arg = ""):
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
			err = pysysdc.get_last_error(ret)
			logging.error("pysysdc: '%s'", err)

		logging.debug("pysysdc: send '%s'", ret)
		return ret

	def unit_enable(self, unit_name):
		return pysysdc.unit_enable(unit_name)

	def unit_disable(self, unit_name):
		return pysysdc.unit_disable(unit_name)

	def unit_mask(self, unit_name):
		return pysysdc.unit_mask(unit_name)

	def unit_unmask(self, unit_name):
		return pysysdc.unit_unmask(unit_name)

	def unit_status(self, unit_name):
		return pysysdc.unit_status(unit_name)

	@staticmethod
	def set_hostname(method, value):
		if not value:
			return None
		return pysysdc.set_hostname(method, value)

	@staticmethod
	def get_hostname(attr):
		return pysysdc.get_hostname(attr)

	@staticmethod
	def machine_reboot(wall_message=""):
		return pysysdc.machine_reboot(wall_message)

	@staticmethod
	def machine_poweroff(wall_message=""):
		return pysysdc.machine_poweroff(wall_message)

	@staticmethod
	def set_wall_message(wall_message=""):
		return pysysdc.set_wall_message(wall_message)