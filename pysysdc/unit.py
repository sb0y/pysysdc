from pysysdc.sdbus import SDBus
import logging

class Unit(object):
	__bus = None
	__unit_name = ""

	def __init__(self, unit_name):
		self.__bus = SDBus(service_name="org.freedesktop.systemd1",
			path="/org/freedesktop/systemd1",
			if_name="org.freedesktop.systemd1.Manager",
			method_name="StartUnit",
			method_args="ss"
		)
		
		self.__unit_name = "%s.service" % unit_name

	def __del__(self):
		pass

	def __manage(self, action):
		ret = self.__bus.send(service_name="org.freedesktop.systemd1",
			method_name=action,
			first_arg=self.__unit_name,
			second_arg="replace",
			output_sig="o"
		)

		return ret

	# https://www.freedesktop.org/wiki/Software/systemd/dbus/
	def restart(self):
		action = "%sUnit" % self.restart.__name__.title()
		return self.__manage(action)

	def stop(self):
		action = "%sUnit" % self.stop.__name__.title()
		return self.__manage(action)
	
	def start(self):
		action = "%sUnit" % self.start.__name__.title()
		return self.__manage(action)

	def reload(self):
		action = "%sUnit" % self.reload.__name__.title()
		return self.__manage(action)

	def disable(self):
		return self.__bus.unit_disable(self.__unit_name)
	
	def enable(self):
		return self.__bus.unit_enable(self.__unit_name)

	def mask(self):
		return self.__bus.unit_mask(self.__unit_name)

	def unmask(self):
		return self.__bus.unit_unmask(self.__unit_name)

	def status(self):
		return self.__bus.unit_status(self.__unit_name)