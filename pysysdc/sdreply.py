from typing import Union
import logging
import pysysdc

class SDReply(object):
	sd_code = -1
	py_code = -1
	py_string = None
	py_int = None
	py_bool = None
	strerr = ''
	def __init__(self, init_list: list) -> None:
		if init_list[0] < 0:
			err = pysysdc.get_last_error(init_list[0])
			logging.error("SDReply: '%s'", err)
		self.sd_code = init_list[0]
		self.py_code = init_list[1]
		if len(init_list) > 2 and init_list[2] is not None:
			self.py_string = init_list[2]

	def __bool__(self) -> bool:
		if self.sd_code >= 0 and self.py_code >= 0:
			return True

		return False

	def __repr__(self) -> str:
		return '{"sd_code": %s, "py_code": "%s", "py_string": "%s"}' % (str(self.sd_code), str(self.py_code), str(self.py_string))

	def __getitem__(self, index: int) -> Union[str, int, bool]:
		# this order is for backwards compatibility
		lst = [self.sd_code, self.py_string, self.py_code]
		return lst[index]

	def __str__(self) -> str:
		if self.py_string is not None:
			return self.py_string
		elif self.py_int is not None:
			return str(self.py_int)
		elif self.py_bool is not None:
			return "true" if self.py_bool else "false"

		return None

	def __int__(self) -> int:
		if self.py_int is not None:
			return self.py_int
		elif self.py_bool is not None:
			return 1 if self.py_bool else 0

		return None

	def __bool__(self) -> bool:
		if self.py_bool is not None:
			return self.py_bool
		elif self.py_int is not None:
			return True if self.py_int else False

		return None

	def __del__(self) -> None:
		pass

	def to_string(self) -> str:
		return self.__str__()

	def to_int(self) -> int:
		return self.__int__()

	def to_bool(self) -> bool:
		return self.__bool__()