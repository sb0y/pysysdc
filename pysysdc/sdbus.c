//#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <systemd/sd-bus.h>

#include "python_runner.h"
#include "sdbus.h"
#include "dc_error_handling.h"
#include "helpers.h"
#include "connect.h"

#define METHOD_HNDLR int (*func_ptr)(sd_bus_message *m, void *userdata, sd_bus_error *ret_error)

void *c_func = NULL;
struct sd_bus_vtable *methods_vtable = NULL;
sd_bus_slot *g_slot = NULL;

static int method_handler(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) 
{
	char *operation = NULL;
	int r = 0;

	if(userdata == NULL)
	{
		set_dc_error("userdata is NULL");
		return -10;
	}

	struct dc_method_data *md = (struct dc_method_data*)(char*)userdata;

	/* Read the parameters */
	r = sd_bus_message_read(m, md->method_args, &operation);
	//fprintf(stdout, "Requested operation: '%s'\n", operation);
	if (r < 0) 
	{
		set_dc_error("Failed to parse parameters");
		return r;
	}

	char *result = NULL;
	int op_result = run_python_func(c_func, operation, &result);

	if (op_result)
	{
		int ret = sd_bus_reply_method_return(m, md->method_args, result);
		if(result)
			free(result);
		return ret;
	} else {
		sd_bus_error_set_const(ret_error, md->method_name, "unknown error");
		if(result)
			free(result);
		return -EINVAL;
	}
}

void init_vtable(const char *method_name, const char *method_return, const char *method_args, METHOD_HNDLR)
{
	sd_bus_vtable new_methods_vtable[] = {
		SD_BUS_VTABLE_START(0),
		SD_BUS_METHOD(method_name, method_args, method_return, method_handler, SD_BUS_VTABLE_UNPRIVILEGED),
		SD_BUS_VTABLE_END
	};

	methods_vtable = malloc(3 * sizeof(sd_bus_vtable));

	memcpy(methods_vtable, new_methods_vtable, 3 * sizeof(sd_bus_vtable));
}

void init_method_data(const char *method_name, const char *method_return, const char *method_args)
{
	strcpy(method_data.method_name, method_name);
	strcpy(method_data.method_return, method_return);
	strcpy(method_data.method_args, method_args);
}

int listen(void)
{
	int r = 0;

	for (;;) 
	{
		/* Process requests */
		r = sd_bus_process(acquire_gbus(), NULL);
		if (r < 0) 
		{
			set_dc_error("Failed to process bus");
			return r;
		}

		if (r > 0) /* we processed a request, try to process another one, right-away */
			continue;

		/* Wait for the next request to process */
		r = sd_bus_wait(acquire_gbus(), (uint64_t) -1);
		if (r < 0) 
		{
			//fprintf(stderr, "Failed to wait on bus: %s\n", strerror(-r));
			set_dc_error("Failed to wait on bus");
			return r;
		}
	}

	return r;
}

int service_register(const char *path, 
					const char *if_name, 
					const char *method_name, 
					const char *method_return, 
					const char *method_args) 
{
	int r = 0;

	init_method_data(method_name, method_return, method_args);
	init_vtable(method_data.method_name, method_data.method_return, method_data.method_args, NULL);

	/* Install the object */
	r = sd_bus_add_object_vtable(acquire_gbus(),
								&g_slot,
								path,  /* object path */
								if_name,  /* interface name */
								methods_vtable,
								(void*)(&method_data));
	if (r < 0) {
		set_dc_error("Failed to issue method call");
		return r < 0 ? 0 : 1;
	}

	/* Take a well-known service name so that clients can find us */
	r = sd_bus_request_name(acquire_gbus(), if_name, 0);
	if (r < 0) {
		set_dc_error("Failed to acquire service name");
	}

	return r < 0 ? 0 : 1;
}

int b_client(
	sd_bus *bus,
	const char *s_name, 
	const char *s_path, 
	const char *i_name, 
	const char *m_name, 
	const char *input_sig, 
	const char *output_sig,
	const char *first_arg,
	const char *second_arg,
	char **return_data
)
{
	int r = 0;
	const char *return_data_tmp = NULL;
	sd_bus_error error = SD_BUS_ERROR_NULL;
	sd_bus_message *m = NULL;

	if(!bus) {
		set_dc_error("bus value is NULL");
		goto finish;
	}

	/* Issue the method call and store the respons message in m */
	r = sd_bus_call_method(bus,
							s_name,          /* service to contact */
							s_path,          /* object path */
							i_name,          /* interface name */
							m_name,          /* method name */
							&error,          /* object to return error in */
							&m,              /* return message on success */
							input_sig,       /* input signature */
							first_arg,       /* first argument */
							second_arg);     /* second argument */

	if (r < 0) 
	{
		//fprintf(stderr, "Failed to issue method call: %s\n", error.message);
		set_dc_error(error.message);
		goto finish;
	}

	/* Parse the response message */
	r = sd_bus_message_read(m, output_sig, &return_data_tmp);
	if (r < 0) 
	{
		//fprintf(stderr, "Failed to parse response message: %s\n", strerror(-r));
		set_dc_error("Failed to parse response message");
		goto finish;
	}

	*return_data = strdup(return_data_tmp);

	finish:
		if(error.message)
			sd_bus_error_free(&error);
		if(m)
			sd_bus_message_unref(m);

	return r < 0 ? 0 : 1;
}

int client(
	const char *s_name, 
	const char *s_path, 
	const char *i_name, 
	const char *m_name, 
	const char *input_sig, 
	const char *output_sig,
	const char *first_arg,
	const char *second_arg,
	char **return_data
)
{
	int r = b_client(acquire_gbus(), s_name, s_path, i_name, m_name, input_sig, output_sig, first_arg, second_arg, return_data);

	return r < 0 ? 0 : 1;
}

void set_cfunc(void *func)
{
	if(func == NULL)
	{
		set_dc_error("func arg is NULL");
		return;
	}

	c_func = func;
}

void init(void)
{
	setrlimit_closest(RLIMIT_NOFILE, &RLIMIT_MAKE_CONST(16384));
	bus_init();
	//acquire_gbus();
}

void deinit(void)
{
	//fprintf(stderr, "deinit call\n");
	bus_deinit();

	if(g_slot)
		sd_bus_slot_unref(g_slot);

	if(methods_vtable)
		free(methods_vtable);
	
	dc_clean_error();
}
