import sys

from pysysdc.unit import Unit

__all__ = ("systemctl",)

def systemctl(action: str, svc: str, type: str = "service") -> tuple:
	supported_actions = ("restart", "start", "stop", "reload", "mask", "unmask", "enable", "disable", "status", "extended_status")
	if action not in supported_actions:
		raise ValueError("Specified action '%s' is not supported" % action)
	
	_action = action
	if _action == "extended_status":
		_action = "status"

	unit = Unit(unit_name=svc, unit_type=type)
	ret = getattr(unit, _action)()

	if action == "status":
		return ret[0], ret[1].get("ActiveState", None)

	return ret[0], ret[1]

sys.modules[__name__] = systemctl
