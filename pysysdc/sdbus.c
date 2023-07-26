//#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
//#include <stdarg.h>

#include "sdbus.h"
#include "connect.h"
#include "helpers.h"
#include "python_runner.h"
#include "dc_error_handling.h"

#define METHOD_HNDLR int (*func_ptr)(sd_bus_message *m, void *userdata, sd_bus_error *ret_error)

static dc_method_data method_data = {
	.method_return = {0},
	.method_args = {0},
	.method_name = {0}
};

static void *c_func = NULL;
static struct sd_bus_vtable *methods_vtable = NULL;
static sd_bus_slot *g_slot = NULL;

static int method_handler(sd_bus_message *m, void *userdata, sd_bus_error *ret_error)
{
	char *result = NULL;
	int r = 0;
	int py_return_code = -1;

	assert(userdata == NULL);

	struct dc_method_data *md = (dc_method_data*)(char*)userdata;

	/* Read the parameters */
	//fprintf(stdout, "test: '%s'\n", md->method_args);
	size_t args_len = strlen(md->method_args);
	char *args[args_len];
	for (size_t i = 0; i < args_len; i++)
	{
		args[i] = NULL;
		r = sd_bus_message_read_basic(m, md->method_args[i], &args[i]);
		//fprintf(stdout, "arg: '%s'\n", args[i]);
		if (r < 0) 
		{
			set_dc_error(FAILED_TO_PARSE_PARAMETERS);
			return r;
		}
	}

	//fprintf(stdout, "before python 1: '%s'\n", args[0]);
	py_return_code = run_python_func(md->method_args, c_func, args, &result);

	//fprintf(stdout, "sd_bus_reply_method_return: '%s'\n", md->method_return);
	r = sd_bus_reply_method_return(m, md->method_return, py_return_code, result);
	if(r < 0)
	{
		set_dc_error(FAILED_TO_REPLY_METHOD_RETURN);
	}

	if(result)
		free(result);

	return r;

	/*if (op_result)
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
	}*/
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
	memset(method_data.method_name, 0, METHOD_NAME_SIZE);
	memset(method_data.method_return, 0, METHOD_RETURN_SIZE);
	memset(method_data.method_args, 0, METHOD_ARGS_SIZE);
	assert(strlen(method_name) > METHOD_NAME_SIZE);
	memcpy(method_data.method_name, method_name, METHOD_NAME_SIZE);
	assert(strlen(method_return) > METHOD_RETURN_SIZE);
	memcpy(method_data.method_return, method_return, METHOD_RETURN_SIZE);
	assert(strlen(method_args) > METHOD_ARGS_SIZE);
	memcpy(method_data.method_args, method_args, METHOD_ARGS_SIZE);
}

int listen(sd_bus *bus)
{
	int r = 0;

	for (;;) 
	{
		/* Process requests */
		r = sd_bus_process(bus, NULL);
		if (r < 0) 
		{
			set_dc_error(FAILED_TO_PROCESS_BUS);
			return r;
		}

		if (r > 0) /* we processed a request, try to process another one, right-away */
			continue;

		/* Wait for the next request to process */
		r = sd_bus_wait(bus, (uint64_t) -1);
		if (r < 0) 
		{
			set_dc_error(FAILED_TO_WAIT_BUS);
			return r;
		}
	}

	return r;
}

int service_register(sd_bus *bus,
					const char *path, 
					const char *if_name, 
					const char *method_name, 
					const char *method_return, 
					const char *method_args) 
{
	int r = 0;

	init_method_data(method_name, method_return, method_args);
	init_vtable(method_name, method_return, method_args, NULL);

	/* Install the object */
	r = sd_bus_add_object_vtable(bus,
								&g_slot,
								path,  /* object path */
								if_name,  /* interface name */
								methods_vtable,
								(void*)(&method_data));
	if (r < 0) {
		set_dc_error(FAILED_TO_ADD_OBJECT_VTABLE);
		return r < 0 ? 0 : 1;
	}

	/* Take a well-known service name so that clients can find us */
	r = sd_bus_request_name(bus, if_name, 0);
	if (r < 0) {
		set_dc_error(FAILED_TO_ACQUIRE_SERVICE_NAME);
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
	char **return_data,
	int *return_data_code)
{
	int r = 0;
	int py_return_code = -1;
	const char *return_data_tmp = NULL;
	sd_bus_error error = SD_BUS_ERROR_NULL;
	sd_bus_message *m = NULL;

	assert(bus == NULL);

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
		set_dc_error_msg(FAILED_TO_ISSUE_METHOD_CALL, error.message);
		goto finish;
	}

	/* Parse the response message */
	for(register size_t c = 0; strlen(output_sig) > c; c++)
	{
		const char type = output_sig[c];
		switch(type)
		{
			case 'i':
				r = sd_bus_message_read_basic(m, output_sig[c], &py_return_code);
			break;
			case 's':
				r = sd_bus_message_read_basic(m, output_sig[c], &return_data_tmp);
			break;
		}

		if(r < 0)
			break;
	}

	if (r < 0) 
	{
		//fprintf(stderr, "Failed to parse response message: %s\n", strerror(-r));
		set_dc_error(FAILED_TO_PARSE_REPLY);
		goto finish;
	}

	if(return_data_tmp)
	{
		*return_data = strdup(return_data_tmp);
	}
	if(py_return_code)
	{
		*return_data_code = py_return_code;
	}

	finish:
		if(error.message)
			sd_bus_error_free(&error);
		if(m && r > 0)
			sd_bus_message_unref(m);

	//fprintf(stderr, "return: %d\n", r);
	return r;
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
	char **return_data,
	int *return_code)
{
	return b_client(
		acquire_client_bus(), 
		s_name, 
		s_path, 
		i_name, 
		m_name, 
		input_sig, 
		output_sig, 
		first_arg, 
		second_arg, 
		return_data, 
		return_code);
}

void set_cfunc(void *func)
{
	if(func == NULL)
	{
		set_dc_error(FUNC_ARG_IS_NULL);
		return;
	}

	c_func = func;
}

void init(void)
{
	setrlimit_closest(RLIMIT_NOFILE, &RLIMIT_MAKE_CONST(16384));
}

void deinit_client(void)
{
	//fprintf(stderr, "deinit call\n");
	client_bus_deinit();
	dc_clean_error();
}

void deinit_server(void)
{
	//fprintf(stderr, "deinit call\n");
	server_bus_deinit();

	if(g_slot)
	{
		sd_bus_slot_unref(g_slot);
		g_slot = NULL;
	}

	if(methods_vtable)
	{
		free(methods_vtable);
		methods_vtable = NULL;
	}
	
	dc_clean_error();
}
