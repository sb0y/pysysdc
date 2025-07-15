#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <systemd/sd-bus.h>

#include "systemd_methods.h"
#include "sdbus.h"
#include "dc_error_handling.h"
#include "helpers.h"
#include "connect.h"

int unit_enable(const char *unit_name, char *return_data[])
{
	int r = 0;
	const char *type = NULL;
	const char *path = NULL;
	const char *source = NULL;
	sd_bus_error error = SD_BUS_ERROR_NULL;
	sd_bus_message *m = NULL;
	int carries_install_info = -1;

	sd_bus *bus = acquire_client_bus();
	if (!bus)
	{
		goto finish;
	}

	/* Issue the method call and store the respons message in m */
	r = sd_bus_call_method
	(
		bus,
		"org.freedesktop.systemd1",           /* service to contact */
		"/org/freedesktop/systemd1",          /* object path */
		"org.freedesktop.systemd1.Manager",   /* interface name */
		"EnableUnitFiles",          /* method name */
		&error,          /* object to return error in */
		&m,              /* return message on success */
		"asbb",       /* input signature */
		1,       /* first argument */
		unit_name,
		0,
		0     /* second argument */
	);

	if (r < 0) 
	{
		//fprintf(stderr, "Failed to issue method call: %s\n", error.message);
		set_dc_error_msg(FAILED_TO_ISSUE_METHOD_CALL, error.message);
		goto finish;
	}

	r = sd_bus_message_read(m, "b", &carries_install_info);

	r = sd_bus_message_enter_container(m, SD_BUS_TYPE_ARRAY, "(sss)");

	/* Parse the response message */
	r = sd_bus_message_read(m, "(sss)", &type, &path, &source);
	if (r < 0) 
	{
		//fprintf(stderr, "Failed to parse response message: %s\n", strerror(-r));
		set_dc_error(FAILED_TO_PARSE_REPLY);
		goto finish;
	}

	if(type)
		return_data[0] = strdup(type);
	if(path)
		return_data[1] = strdup(path);
	if(source)
		return_data[2] = strdup(source);

	r = sd_bus_message_exit_container(m);

	finish:
		if(error.message)
			sd_bus_error_free(&error);
		if(m)
			sd_bus_message_unref(m);

	return r < 0 ? 0 : 1;
}

int unit_disable(const char *unit_name, char *return_data[])
{
	int r = 0;
	const char *type = NULL;
	const char *path = NULL;
	const char *source = NULL;
	sd_bus_error error = SD_BUS_ERROR_NULL;
	sd_bus_message *m = NULL;

	sd_bus *bus = acquire_client_bus();
	if (!bus)
	{
		goto finish;
	}

	/* Issue the method call and store the respons message in m */
	r = sd_bus_call_method
	(
		bus,
		"org.freedesktop.systemd1",          /* service to contact */
		"/org/freedesktop/systemd1",          /* object path */
		"org.freedesktop.systemd1.Manager",          /* interface name */
		"DisableUnitFiles",          /* method name */
		&error,          /* object to return error in */
		&m,              /* return message on success */
		"asb",       /* input signature */
		1,       /* first argument */
		unit_name,      /* second argument */
		0
	);

	if (r < 0) 
	{
		//fprintf(stderr, "Failed to issue method call: %s\n", error.message);
		set_dc_error_msg(FAILED_TO_ISSUE_METHOD_CALL, error.message);
		goto finish;
	}

	r = sd_bus_message_enter_container(m, SD_BUS_TYPE_ARRAY, "(sss)");

	/* Parse the response message */
	r = sd_bus_message_read(m, "(sss)", &type, &path, &source);
	if (r < 0) 
	{
		//fprintf(stderr, "Failed to parse response message: %s\n", strerror(-r));
		set_dc_error(FAILED_TO_PARSE_REPLY);
		goto finish;
	}

	if(type)
		return_data[0] = strdup(type);
	if(path)
		return_data[1] = strdup(path);
	if(source)
		return_data[2] = strdup(source);

	//fprintf(stderr, "test2 '%s' '%s' '%s'\n", return_data[0], return_data[1], return_data[2]);

	r = sd_bus_message_exit_container(m);

	finish:
		if(error.message)
			sd_bus_error_free(&error);
		if(m)
			sd_bus_message_unref(m);
		
	return r < 0 ? 0 : 1;
}

int unit_mask(const char *unit_name, char *return_data[])
{
	int r = 0;
	const char *type = NULL;
	const char *path = NULL;
	const char *source = NULL;
	sd_bus_error error = SD_BUS_ERROR_NULL;
	sd_bus_message *m = NULL;

	sd_bus *bus = acquire_client_bus();
	if (!bus)
	{
		goto finish;
	}

	/* Issue the method call and store the respons message in m */
	r = sd_bus_call_method(bus,
							"org.freedesktop.systemd1",          /* service to contact */
							"/org/freedesktop/systemd1",          /* object path */
							"org.freedesktop.systemd1.Manager",          /* interface name */
							"MaskUnitFiles",          /* method name */
							&error,          /* object to return error in */
							&m,              /* return message on success */
							"asbb",       /* input signature */
							1,       /* first argument */
							unit_name,
							0,
							0);     /* second argument */

	if (r < 0) 
	{
		//fprintf(stderr, "Failed to issue method call: %s\n", error.message);
		set_dc_error_msg(FAILED_TO_ISSUE_METHOD_CALL, error.message);
		goto finish;
	}

	r = sd_bus_message_enter_container(m, SD_BUS_TYPE_ARRAY, "(sss)");

	/* Parse the response message */
	r = sd_bus_message_read(m, "(sss)", &type, &path, &source);
	if (r < 0) 
	{
		//fprintf(stderr, "Failed to parse response message: %s\n", strerror(-r));
		set_dc_error(FAILED_TO_PARSE_REPLY);
		goto finish;
	}

	if(type)
		return_data[0] = strdup(type);
	if(path)
		return_data[1] = strdup(path);
	if(source)
		return_data[2] = strdup(source);

	//fprintf(stderr, "test2 '%s' '%s' '%s'\n", return_data[0], return_data[1], return_data[2]);

	r = sd_bus_message_exit_container(m);

	finish:
		if(error.message)
			sd_bus_error_free(&error);
		if(m)
			sd_bus_message_unref(m);
		
	return r < 0 ? 0 : 1;
}

int unit_unmask(const char *unit_name, char *return_data[])
{
	int r = 0;
	const char *type = NULL;
	const char *path = NULL;
	const char *source = NULL;
	sd_bus_error error = SD_BUS_ERROR_NULL;
	sd_bus_message *m = NULL;

	sd_bus *bus = acquire_client_bus();
	if (!bus) 
	{
		goto finish;
	}

	/* Issue the method call and store the respons message in m */
	r = sd_bus_call_method(bus,
							"org.freedesktop.systemd1",          /* service to contact */
							"/org/freedesktop/systemd1",          /* object path */
							"org.freedesktop.systemd1.Manager",          /* interface name */
							"UnmaskUnitFiles",          /* method name */
							&error,          /* object to return error in */
							&m,              /* return message on success */
							"asb",       /* input signature */
							1,       /* first argument */
							unit_name,
							0);     /* second argument */

	if (r < 0) 
	{
		//fprintf(stderr, "Failed to issue method call: %s\n", error.message);
		set_dc_error_msg(FAILED_TO_ISSUE_METHOD_CALL, error.message);
		goto finish;
	}

	r = sd_bus_message_enter_container(m, SD_BUS_TYPE_ARRAY, "(sss)");

	/* Parse the response message */
	r = sd_bus_message_read(m, "(sss)", &type, &path, &source);
	if (r < 0) 
	{
		//fprintf(stderr, "Failed to parse response message: %s\n", strerror(-r));
		set_dc_error(FAILED_TO_PARSE_REPLY);
		goto finish;
	}

	if(type)
		return_data[0] = strdup(type);
	if(path)
		return_data[1] = strdup(path);
	if(source)
		return_data[2] = strdup(source);

	//fprintf(stderr, "test2 '%s' '%s' '%s'\n", return_data[0], return_data[1], return_data[2]);

	r = sd_bus_message_exit_container(m);

	finish:
		if(error.message)
			sd_bus_error_free(&error);
		if(m)
			sd_bus_message_unref(m);
		
	return r < 0 ? 0 : 1;
}

int set_hostname(const char *method, const char *value)
{
	sd_bus_error error = SD_BUS_ERROR_NULL;
	int r = 0;

	sd_bus *bus = acquire_client_bus();
	if (!bus)
	{
		goto finish;
	}

	r = sd_bus_call_method
	(
		bus,
		"org.freedesktop.hostname1",
		"/org/freedesktop/hostname1",
		"org.freedesktop.hostname1",
		method,
		&error, NULL,
		"sb",
		value,
		0
	);

	if (r < 0)
	{
		set_dc_error_msg(FAILED_TO_CALL_METHOD, error.message);
		goto finish;
	}

	finish:
		if(error.message)
			sd_bus_error_free(&error);

	return r < 0 ? 0 : 1;
}

int get_hostname(const char *attr, char **ret)
{
	sd_bus_error error = SD_BUS_ERROR_NULL;
	sd_bus_message *reply = NULL;
	int r = 0;
	char *r_buf = NULL;

	sd_bus *bus = acquire_client_bus();
	if (!bus) {
		goto finish;
	}

	r = sd_bus_get_property(
		bus,
		"org.freedesktop.hostname1",
		"/org/freedesktop/hostname1",
		"org.freedesktop.hostname1",
		attr,
		&error, &reply, "s");

	if (r < 0)
	{
		set_dc_error(FAILED_TO_GET_PROPERTY);
		goto finish;
	}

	r = sd_bus_message_read(reply, "s", &r_buf);
	if (r < 0) {
		set_dc_error(FAILED_TO_PARSE_REPLY);
		goto finish;
	}

	if (r_buf)
		*ret = strdup(r_buf);

	finish:
		if(error.message)
			sd_bus_error_free(&error);
		
		if(reply)
			sd_bus_message_unref(reply);

	return r < 0 ? 0 : 1;
}

char hexchar(int x)
{
	static const char table[16] = "0123456789abcdef";
	return table[x & 15];
}

char *bus_label_escape(const char *s)
{
	char *r, *t;
	const char *f;

	/* Escapes all chars that D-Bus' object path cannot deal
		* with. Can be reversed with bus_path_unescape(). We special
	* case the empty string. */

	if (*s == 0)
		return strdup("_");

	r = (char*)malloc(sizeof(char)*strlen(s)*3+1);
	if (!r)
		return NULL;

	for (f = s, t = r; *f; f++) 
	{

		/* Escape everything that is not a-zA-Z0-9. We also
		* escape 0-9 if it's the first character */

		if (!(*f >= 'A' && *f <= 'Z') &&
			!(*f >= 'a' && *f <= 'z') &&
			!(f > s && *f >= '0' && *f <= '9'))
		{
			
			*(t++) = '_';
			*(t++) = hexchar(*f >> 4);
			*(t++) = hexchar(*f);
		} else
			*(t++) = *f;
	}

	*t = 0;

	return r;
}

int unit_status(const char *unit_name, char *return_data[])
{
	int r = 0;
	sd_bus_error error = SD_BUS_ERROR_NULL;
	sd_bus_message *reply = NULL;
	char *escaped_unit_name = NULL;
	char *path = NULL;

	escaped_unit_name = bus_label_escape(unit_name);
	char *opath = "/org/freedesktop/systemd1/unit/";
	size_t plen = (sizeof(char)*(strlen(opath)+strlen(escaped_unit_name)))+1;
	path = (char*)malloc(plen);
	strcpy(path, opath);
	strcat(path, escaped_unit_name);
	path[plen] = 0;

	sd_bus *bus = acquire_client_bus();
	if (!bus) 
	{
		goto finish;
	}

	/* Issue the method call and store the respons message in m */
	r = sd_bus_call_method(bus,
							"org.freedesktop.systemd1",          /* service to contact */
							path,          /* object path */
							"org.freedesktop.DBus.Properties",          /* interface name */
							"GetAll",          /* method name */
							&error,          /* object to return error in */
							&reply,              /* return message on success */
							"s",       /* input signature */
							"");

	if (r < 0) 
	{
		//fprintf(stderr, "Failed to issue method call: %s\n", error.message);
		set_dc_error_msg(FAILED_TO_ISSUE_METHOD_CALL, error.message);
		goto finish;
	}

	r = sd_bus_message_enter_container(reply, SD_BUS_TYPE_ARRAY, "{sv}");
	if (r < 0) {
		set_dc_error(FAILED_TO_PARSE_REPLY);
		goto finish;
	}

	while ((r = sd_bus_message_enter_container(reply, SD_BUS_TYPE_DICT_ENTRY, "sv")) > 0) {
		const char *name, *contents;

		r = sd_bus_message_read(reply, "s", &name);
		/*if (r < 0) {
			set_dc_error(FAILED_TO_PARSE_REPLY);
			goto finish;
		}*/

		r = sd_bus_message_peek_type(reply, NULL, &contents);
		/*if (r < 0) {
			set_dc_error(FAILED_TO_PARSE_REPLY);
			goto finish;
		}*/

		r = sd_bus_message_enter_container(reply, SD_BUS_TYPE_VARIANT, contents);
		/*if (r < 0) {
			set_dc_error(FAILED_TO_PARSE_REPLY);
			goto finish;
		}*/

		//r = status_property(name, reply, &info, contents);

		switch(contents[0]) {
			case SD_BUS_TYPE_STRING: {
				const char *s = NULL;
				r = sd_bus_message_read(reply, "s", &s);
				//if (r < 0) {
				//	set_dc_error("parse error");
				//	goto finish;
				//}
				if(s) {
					//fprintf(stderr, "HERE44 '%s' \n", name);
					if(strcmp(name,"LoadState") == 0) {
						return_data[0] = strdup(s);
					} else if(strcmp(name,"ActiveState") == 0) {
						return_data[1] = strdup(s);
					} else if(strcmp(name,"SubState") == 0) {
						return_data[2] = strdup(s);
					}
				}
				break;
			}

			/*case SD_BUS_TYPE_ARRAY: {
				r = sd_bus_message_enter_container(reply, SD_BUS_TYPE_ARRAY, "(sasbttttuii)");
				if (r < 0) {
					set_dc_error("parse error");
					goto finish;
				}
				////
				r = sd_bus_message_exit_container(reply);
				if (r < 0) {
					set_dc_error("parse error");
					goto finish;
			}*/

			default:
				r = sd_bus_message_skip(reply, contents);
		}

		r = sd_bus_message_exit_container(reply);
		//if (r < 0) {
		//	set_dc_error("parse error");
		//	goto finish;
		//}

		r = sd_bus_message_exit_container(reply);
		//if (r < 0) {
		//	set_dc_error("parse error");
		//	goto finish;
		//}
	}

	//if (r < 0) {
	//	set_dc_error("parse error");
	//	goto finish;
	//}

	r = sd_bus_message_exit_container(reply);
	//if (r < 0) {
	//	set_dc_error("parse error");
	//	goto finish;
	//}

	finish:
		if(error.message)
			sd_bus_error_free(&error);
		if(reply)
			sd_bus_message_unref(reply);
		if(escaped_unit_name)
			free(escaped_unit_name);
		if(path)
			free(path);
	
	return r < 0 ? 0 : 1;
}

int set_wall_message(const char *wall_message)
{
	sd_bus_error error = SD_BUS_ERROR_NULL;
	sd_bus_message *reply = NULL;
	int r = 0;

	sd_bus *bus = acquire_client_bus();
	if (!bus)
	{
		goto finish;
	}

	r = sd_bus_call_method
	(
		bus,
		"org.freedesktop.login1",
		"/org/freedesktop/login1",
		"org.freedesktop.login1.Manager",
		"SetWallMessage",
		&error, 
		NULL,
		"sb", 
		wall_message, 1
	);

	if (r < 0)
	{
		set_dc_error_msg(FAILED_TO_CALL_METHOD, error.message);
		goto finish;
	}

	finish:
		if(error.message)
			sd_bus_error_free(&error);
		
		if(reply)
			sd_bus_message_unref(reply);

	return r < 0 ? 0 : 1;
}

int logind_method(const char *method, const char *wall_message)
{
	sd_bus_error error = SD_BUS_ERROR_NULL;
	sd_bus_message *reply = NULL;
	int r = 0;

	sd_bus *bus = acquire_client_bus();
	if (!bus) 
	{
		goto finish;
	}

	if(wall_message) 
	{
		set_wall_message(wall_message);
	}

	r = sd_bus_call_method(
						bus,
						"org.freedesktop.login1",
						"/org/freedesktop/login1",
						"org.freedesktop.login1.Manager",
						method,
						&error,
						NULL,
						"b", 0);

	if (r < 0) 
	{
		set_dc_error_msg(FAILED_TO_CALL_METHOD, error.message);
		goto finish;
	}

	finish:
		if(error.message)
			sd_bus_error_free(&error);
		
		if(reply)
			sd_bus_message_unref(reply);

	return r < 0 ? 0 : 1;
}

// https://www.freedesktop.org/software/systemd/man/latest/org.freedesktop.timedate1.html#
int set_timezone(const char *timezone)
{
	sd_bus_error error = SD_BUS_ERROR_NULL;
	int r = 0;

	sd_bus *bus = acquire_client_bus();
	if(!bus)
	{
		goto finish;
	}

	if(r < 0)
	{
		set_dc_error_msg(FAILED_TO_CALL_METHOD, error.message);
		goto finish;
	}

	r = sd_bus_call_method(
					bus,
					"org.freedesktop.timedate1",
					"/org/freedesktop/timedate1",
					"org.freedesktop.timedate1",
					"SetTimezone",
					&error,
					NULL,
					"sb", timezone, 0);

	if(r < 0)
	{
		set_dc_error_msg(FAILED_TO_CALL_METHOD, error.message);
		goto finish;
	}

	finish:
		if(error.message)
			sd_bus_error_free(&error);

	return r < 0 ? 0 : 1;
}

int daemon_reload(void)
{
	sd_bus_error error = SD_BUS_ERROR_NULL;
	sd_bus_message *reply = NULL;
	int r = 0;

	sd_bus *bus = acquire_client_bus();
	if (!bus) {
		goto finish;
	}

	r = sd_bus_call_method(
						bus,
						"org.freedesktop.systemd1",
						"/org/freedesktop/systemd1",
						"org.freedesktop.systemd1.Manager",
						"Reload",
						&error,
						NULL,
						NULL);

	if (r < 0) 
	{
		set_dc_error_msg(FAILED_TO_CALL_METHOD, error.message);
		goto finish;
	}

	finish:
		if(error.message)
			sd_bus_error_free(&error);
		
		if(reply)
			sd_bus_message_unref(reply);

	return r < 0 ? 0 : 1;
}

int unit_control(const char *unit_name, const char *action, char **return_data)
{
	int r = 0;
	const char *path = NULL;
	sd_bus_error error = SD_BUS_ERROR_NULL;
	sd_bus_message *m = NULL;

	sd_bus *bus = acquire_client_bus();
	if (!bus) 
	{
		goto finish;
	}

	/* Issue the method call and store the respons message in m */
	r = sd_bus_call_method(bus,
							"org.freedesktop.systemd1",		/* service to contact */
							"/org/freedesktop/systemd1",	/* object path */
							"org.freedesktop.systemd1.Manager",	/* interface name */
							action,			/* method name */
							&error,			/* object to return error in */
							&m,				/* return message on success */
							"ss",			/* input signature */
							unit_name,		/* first argument */
							"replace");		/* second argument */

	if (r < 0) 
	{
		//fprintf(stderr, "Failed to issue method call: %s\n", error.message);
		set_dc_error_msg(FAILED_TO_ISSUE_METHOD_CALL, error.message);
		goto finish;
	}

	/* Parse the response message */
	r = sd_bus_message_read(m, "o", &path);
	if (r < 0) 
	{
		set_dc_error(FAILED_TO_PARSE_REPLY);
		goto finish;
	}

	if(path)
	{
		*return_data = strdup(path);
	}

	finish:
		if(error.message)
			sd_bus_error_free(&error);
		if(m)
			sd_bus_message_unref(m);
	
	return r < 0 ? 0 : 1;
}

// https://manpages.ubuntu.com/manpages/impish/man5/org.freedesktop.login1.5.html
int machine_reboot(const char *wall_message)
{
	return logind_method("Reboot", wall_message);
}

int machine_poweroff(const char *wall_message)
{
	return logind_method("PowerOff", wall_message);
}

