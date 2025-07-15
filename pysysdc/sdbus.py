import os
from abc import ABC
from typing import Callable, Union
import select

import warnings

import logging

import pysysdc
from pysysdc.sdreply import SDReply

warnings.simplefilter("default")

class SDBus(ABC):
	def __init__(self, *args: Union[str, tuple], **kwargs: Union[str, tuple]) -> None:
		# check args validity
		if args:
			self.validate(args)
		if kwargs:
			self.validate(kwargs)

	def enable_backward_compability(self, modify: Union[dict, tuple]) -> None:
		if isinstance(modify, tuple):
			for i, _ in enumerate(modify):
				self.enable_backward_compability(modify[i])
		elif isinstance(modify, dict):
			for old_arg, new_arg in self.backward_args.items():
				if old_arg in modify and old_arg not in self.backward_drop_args:
					from copy import deepcopy
					modify[new_arg] = deepcopy(modify[old_arg])
					modify.pop(old_arg)
					warnings.warn(
						"The `%s` parameter is depercated. Please use `%s` instead." % (old_arg, new_arg), 
						DeprecationWarning, 
						stacklevel=2
					)
				if old_arg in self.backward_drop_args and old_arg in modify:
					warnings.warn(
						"Argument `%s` is deparcated. Please delete from constructor." % old_arg,
						DeprecationWarning, 
						stacklevel=2
					)
					if old_arg in modify:
						modify.pop(old_arg)

	def validate(self, args: "tuple[str, tuple, dict]") -> None:
		self.validate_required(args)
		self.validate_extra(args)

	def merge_args(self, args: "tuple[str, tuple]", kwargs: "dict[str, tuple]") -> None:
		def get_arg(args: list, index: int) -> str:
			if len(args) >= index:
				return args[index]
			return ""
		kwargs.update(
			service_name=get_arg(args, 0),
			path=get_arg(args, 1),
			if_name=get_arg(args, 2),
			method_name=get_arg(args, 3),
			method_args=get_arg(args, 4),
			input_sig=get_arg(args, 5),
			output_sig=get_arg(args, 6),
			first_arg=get_arg(args, 7),
			second_arg=get_arg(args, 8)
		)

	def validate_required(self, args: Union[tuple, dict]) -> None:
		if isinstance(args, tuple):
			for arg_tuple in args:
				self.validate_required(arg_tuple)
		elif isinstance(args, dict):
			for r_arg in self.required_args:
				if r_arg not in args and r_arg not in self.backward_args and r_arg != "method_handler":
					raise ValueError("Dict validation failed. Parameter '%s' is required" % r_arg)
				if r_arg == "method_handler" and r_arg not in args:
					warnings.warn(
						"Seems like you trying to use library in backward compatibility mode. Don't forget to call set_func()!", 
						DeprecationWarning, 
						stacklevel=2
					)

	def validate_extra(self, args: Union[str, tuple, dict], value: Union[str, int] = None) -> None:
		if isinstance(args, tuple):
			for arg in args:
				self.validate_extra(arg)
		elif isinstance(args, dict):
			for k, v in args.items():
				self.validate_extra(k, v)
		elif isinstance(args, str):
			if args not in self.required_args and args not in self.backward_drop_args:
				raise ValueError("Unknown argument '%s'" % args)

class SDServer(SDBus):
	required_args = ("path", "if_name", "method_name", "input_sig", "output_sig", "method_handler")
	backward_args = {"method_return": "output_sig", "method_args": "input_sig", "service_name": "service_name"}
	backward_drop_args = ("service_name",)
	svc_methods = ()

	def __init__(self, args: "Union[tuple[dict], str]" = None, **kwargs: "Union[dict, str]") -> None:
		_args = ()
		if args:
			_args = args
		elif kwargs:
			_args = (kwargs,)

		if isinstance(_args, dict):
			_args = (_args,)

		self.enable_backward_compability(_args)
		super().__init__(_args)
		self.svc_methods = _args
		pysysdc.init(len(_args))

	def __del__(self) -> None:
		self.shutdown()

	def shutdown(self) -> None:
		pysysdc.deinit_server()
	
	def set_func(self, cf: Callable) -> None:
		warnings.warn("This method is deprecated. Please use new styled SDServer constructor.", DeprecationWarning, stacklevel=2)
		warnings.warn("The method `set_func` is changing ONLY first method handler!", DeprecationWarning, stacklevel=2)
		self.svc_methods[0]["method_handler"] = cf

	def __services_register(self) -> bool:
		for index, method in enumerate(self.svc_methods):
			if not callable(method["method_handler"]):
				logging.error("The `method_handler` for method '%s' (path: '%s') is not callable!", method["method_name"], method["path"])
				return False
			ret = pysysdc.service_register(
				method["path"],
				method["if_name"],
				method["method_name"],
				method["output_sig"],
				method["input_sig"],
				method["method_handler"],
				index
			)

			if ret < 0:
				logging.error("pysysdc__services_register: '%s'", pysysdc.get_last_error(ret))
				return False

		return True

	def listen(self) -> tuple:
		ret = False
		if self.__services_register():
			ret = pysysdc.listen()
			if ret < 0:
				err = pysysdc.get_last_error(ret)
				logging.error("pysysdc listen: '%s'", err)
				return False, err

		return ret, ''
	
	'''
	def listen_new(self) -> tuple:
		ret = False
		if self.__services_register():
			sd_fd = pysysdc.get_sd_bus_fd()
			if sd_fd:
				py_sd_fd = os.fdopen(sd_fd, "r+", closefd=False)
				epoll = select.epoll()
				epoll.register(py_sd_fd, select.EPOLLIN)
				while True:
					events = epoll.poll(timeout=1000)
	'''
class SDClient(SDBus):
	required_args = ("service_name", "path", "if_name", "method_name", "input_sig", "output_sig")
	backward_args = {"method_return": "output_sig", "method_args": "input_sig"}
	backward_drop_args = ()
	method_info = {}

	def __init__(self, *args: Union[str, int], **kwargs: Union[str, int]) -> None:
		if args:
			def set_if_empty_kwargs(kwargs: Union[str, int], arg_val: Union[str, int], key: str) -> None:
				if key not in kwargs:
					kwargs[key] = arg_val
			for index, key_name in {0: "service_name", 1: "path", 2: "if_name", 3: "method_name", 4: "method_return",\
								5: "method_args", 6: "input_sig", 7: "output_sig", 8: "first_arg",\
								9: "second_arg"}.items():
				if len(args) >= index:
					set_if_empty_kwargs(kwargs, args[index], key_name)

		self.enable_backward_compability(kwargs)
		#print(kwargs)
		super().__init__(**kwargs)
		self.method_info = kwargs
		pysysdc.init(0)

	def __del__(self) -> None:
		pysysdc.deinit_client()

	def send(self, *args: str, **kwargs: str) -> SDReply:
		if args:
			self.merge_args(args, kwargs)
		if kwargs:
			self.method_info.update(kwargs)

		#print(self.method_info)
		ret = pysysdc.send(
			self.method_info.get("service_name", ""),
			self.method_info.get("path", ""),
			self.method_info.get("if_name", ""),
			self.method_info.get("method_name", ""),
			self.method_info.get("input_sig", ""),
			self.method_info.get("output_sig", ""),
			self.method_info.get("first_arg", ""),
			self.method_info.get("second_arg", "")
		)

		if ret[0] < 0:
			err = pysysdc.get_last_error(ret[0])
			logging.error("pysysdc send: '%s'", err)

		logging.debug("pysysdc send: '%s'", ret)

		return SDReply(ret)

	def unit_restart(self, unit_name: str) -> SDReply:
		return SDReply(pysysdc.unit_control(unit_name, "RestartUnit"))
	
	def unit_start(self, unit_name: str) -> SDReply:
		return SDReply(pysysdc.unit_control(unit_name, "StartUnit"))

	def unit_stop(self, unit_name: str) -> SDReply:
		return SDReply(pysysdc.unit_control(unit_name, "StopUnit"))
	
	def unit_reload(self, unit_name: str) -> SDReply:
		return SDReply(pysysdc.unit_control(unit_name, "ReloadUnit"))

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