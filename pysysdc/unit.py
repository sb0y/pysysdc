import time
from typing import Tuple
from pysysdc.sdbus import SDClient
from pysysdc.sdreply import SDReply
#import logging

class Unit(object):
	__bus = None
	__unit_name = ""

	def __init__(self, unit_name: str, unit_type: str="service") -> None:
		self.__bus = SDClient(service_name="org.freedesktop.systemd1",
			path="/org/freedesktop/systemd1",
			if_name="org.freedesktop.systemd1.Manager",
			method_name="StartUnit",
			method_args="ss"
		)
		
		self.__unit_name = "%s.%s" % (unit_name, unit_type)

	def __del__(self) -> None:
		pass

	def wait_status(self, desired_states: Tuple[str], timeout: int=30) -> bool:
		time_start = time.time()
		while True:
			delta = time.time() - time_start
			if delta >= timeout:
				raise TimeoutError("Timeout on wait status. Timeout = %d, desired_states = %s" % (timeout, desired_states))
			status = self.status()
			if status[0]:
				if status[1]["SubState"].strip() in desired_states:
					return True
			
			time.sleep(1)

		return False

	# https://www.freedesktop.org/wiki/Software/systemd/dbus/
	def restart(self) -> SDReply:
		return self.__bus.unit_restart(self.__unit_name)

	def stop(self) -> SDReply:
		return self.__bus.unit_stop(self.__unit_name)
	
	def start(self) -> SDReply:
		return self.__bus.unit_start(self.__unit_name)

	def reload(self) -> SDReply:
		return self.__bus.unit_reload(self.__unit_name)

	def disable(self) -> tuple:
		return self.__bus.unit_disable(self.__unit_name)
	
	def enable(self) -> tuple:
		return self.__bus.unit_enable(self.__unit_name)

	def mask(self) -> tuple:
		return self.__bus.unit_mask(self.__unit_name)

	def unmask(self) -> tuple:
		return self.__bus.unit_unmask(self.__unit_name)

	def status(self) -> tuple:
		return self.__bus.unit_status(self.__unit_name)