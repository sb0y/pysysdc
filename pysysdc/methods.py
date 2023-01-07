from pysysdc.sdbus import SDBus

class Methods(object):
	
	@staticmethod
	def get_token():
		sd = SDBus(service_name="dc.cloud.auth", path="/dc/cloud/auth", if_name="dc.cloud.auth", method_name="Token", method_args="s", method_return="s", output_sig="s")
		return sd.send()

	@staticmethod
	def token_fetch():
		sd = SDBus(service_name="dc.cloud.auth", path="/dc/cloud/auth", if_name="dc.cloud.auth", method_name="Token", method_args="s", method_return="s", output_sig="s")
		return sd.send(first_arg="refetch")

	@staticmethod
	def set_static_hostname(hostname):
		return SDBus.set_hostname("SetStaticHostname", hostname)
	
	@staticmethod
	def set_pretty_hostname(hostname):
		return SDBus.set_hostname("SetPrettyHostname", hostname)

	@staticmethod
	def set_transient_hostname(hostname):
		return SDBus.set_hostname("SetHostname", hostname)

	# https://github.com/systemd/systemd/blob/main/src/hostname/hostnamectl.c#L261
	@staticmethod
	def get_hostname_by_attr(attr):
		return SDBus.get_hostname(attr)

	# https://manpages.ubuntu.com/manpages/impish/man5/org.freedesktop.login1.5.html
	@staticmethod
	def machine_reboot(wall_message=""):
		return SDBus.machine_reboot(wall_message)

	@staticmethod
	def machine_poweroff(wall_message=""):
		return SDBus.machine_poweroff(wall_message)

	@staticmethod
	def set_wall_message(wall_message=""):
		return SDBus.set_wall_message(wall_message)