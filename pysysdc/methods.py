from pysysdc.sdbus import SDClient
import logging

class SDMethods(object):
	@staticmethod
	def set_static_hostname(hostname: str) -> bool:
		return SDClient.set_hostname("SetStaticHostname", hostname)
	
	@staticmethod
	def set_pretty_hostname(hostname: str) -> bool:
		return SDClient.set_hostname("SetPrettyHostname", hostname)

	@staticmethod
	def set_transient_hostname(hostname: str) -> bool:
		return SDClient.set_hostname("SetHostname", hostname)

	# https://github.com/systemd/systemd/blob/main/src/hostname/hostnamectl.c#L261
	@staticmethod
	def get_hostname_by_attr(attr) -> tuple:
		return SDClient.get_hostname(attr)

	# https://manpages.ubuntu.com/manpages/impish/man5/org.freedesktop.login1.5.html
	@staticmethod
	def machine_reboot(wall_message: str="") -> bool:
		return SDClient.machine_reboot(wall_message)

	@staticmethod
	def machine_poweroff(wall_message: str="") -> bool:
		return SDClient.machine_poweroff(wall_message)

	@staticmethod
	def set_wall_message(wall_message: str="") -> bool:
		return SDClient.set_wall_message(wall_message)

	@staticmethod
	def set_timezone(timezone: str) -> bool:
		return SDClient.set_timezone(timezone)
	
	@staticmethod
	def daemon_reload() -> bool:
		return SDClient.daemon_reload()

class Methods(SDMethods):
	
	@staticmethod
	def get_token():
		array = [None, None, None]
		token_array = [None, None]
		
		try:
			sd = SDClient(service_name="dc.cloud.auth", path="/dc/cloud/auth", if_name="dc.cloud.auth", method_name="Token", method_args="s", method_return="is", output_sig="is")
			array = sd.send()
			if array[1]:
				token_array[0], token_array[1] = array[1].split(',')
		except Exception as e:
				logging.error("Pysysdc:: Get token exception ocurred")
				logging.exception(e)
		return (array[0], token_array[0], array[2], token_array[1])

	@staticmethod
	def token_fetch():
		sd = SDClient(service_name="dc.cloud.auth", path="/dc/cloud/auth", if_name="dc.cloud.auth", method_name="Token", method_args="s", method_return="is", output_sig="is")
		return sd.send(first_arg="refetch")

	@staticmethod
	def token_false():
		sd = SDClient(service_name="dc.cloud.auth", path="/dc/cloud/auth", if_name="dc.cloud.auth", method_name="Token", method_args="s", method_return="is", output_sig="is")
		return sd.send(first_arg="tokenfalse")

