#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <assert.h>
#include <errno.h>

#include <systemd/sd-bus.h>

#include "sdbus.h"
#include "connect.h"
#include "helpers.h"
#include "variant.h"
#include "python_runner.h"
#include "sd_python_runner.h"
#include "dc_error_handling.h"
#include "signals.h"

//#define DC_DEBUG 1
#include "debug.h"

#include <Python.h>

#define METHOD_HNDLR int (*func_ptr)(sd_bus_message *m, void *userdata, sd_bus_error *ret_error)

static dc_method *g_methods = NULL;
static int g_methods_amount = 0;
static int g_event_fd = 0;

void init(int methods_amount)
{
	setrlimit_closest(RLIMIT_NOFILE, &RLIMIT_MAKE_CONST(16384));

	g_methods_amount = methods_amount;
	if(g_methods_amount > 0)
	{
		//set_terminate_func(&deinit_server);

		g_methods = (dc_method*)malloc(methods_amount*sizeof(dc_method));
		for(int i = 0; methods_amount > i; ++i)
		{
			g_methods[i].slot = NULL;
			g_methods[i].method_vtable = NULL;
			//
			g_methods[i].method_data.method_cfunc = NULL;
			g_methods[i].method_data.method_name = NULL;
			g_methods[i].method_data.output_sig = NULL;
			g_methods[i].method_data.input_sig = NULL;
		}
	} else {
		//set_terminate_func(&deinit_client);
	}

	//set_signal_actions();
}

static int method_handler(sd_bus_message *m, void *userdata, sd_bus_error *ret_error)
{
	assert(userdata == NULL);

	sd_bus_message *reply = NULL;

	variant_array_t results = init_variant_array();
	int r = 0;
	//int py_return_code = -1;

	dc_method_data *md = (dc_method_data*)(char*)userdata;

	/* Read the parameters */
	//fprintf(stdout, "test: '%s'\n", md->method_args);
	size_t args_len = strlen(md->input_sig);
	char *args[args_len];
	for (size_t i = 0; i < args_len; i++)
	{
		args[i] = NULL;
		r = sd_bus_message_read_basic(m, md->input_sig[i], &args[i]);
		//fprintf(stdout, "arg: '%s'\n", args[i]);
		if (r < 0) 
		{
			set_dc_error(FAILED_TO_PARSE_PARAMETERS);
			return r;
		}
	}
	//fprintf(stdout, "before python 1: '%s'\n", args[0]);
	run_python_func(md->input_sig, md->method_cfunc, args, &results);

	r = sd_bus_message_new_method_return(m, &reply);
	if(!(r < 0))
	{
		size_t m_ret_size = strlen(md->output_sig);
		for(size_t i = 0; m_ret_size > i; ++i)
		{
			const char type = md->output_sig[i];
			switch(type)
			{
				case 'i':
				{
					int tmp = -1;
					if(variant_is_int(results.variants[i]))
					{
						tmp = (int)variant_get_int(results.variants[i]);
						r = sd_bus_message_append(reply, "i", tmp);
					}
					break;
				}
				case 's':
				{
					const char *tmp_s = NULL;
					if(variant_is_str(results.variants[i]))
					{
						tmp_s = variant_get_str(results.variants[i]);
						if(tmp_s)
							r = sd_bus_message_append(reply, "s", tmp_s);
					}
					break;
				}
				default:
				break;
			}
		}
		r = sd_bus_send(acquire_server_bus(), reply, NULL);
		if(!(r < 0))
		{
			set_dc_error(FAILED_TO_REPLY_METHOD_RETURN);
		}
	} else {
		set_dc_error(FAILED_TO_REPLY_METHOD_RETURN);
	}

	if(variant_array_is_allocated(results))
	{
		variant_array_free(results);
	}
	return r;
}

void init_vtable(const char *method_name, const char *method_return, const char *method_args, int index_data, METHOD_HNDLR)
{
	sd_bus_vtable new_methods_vtable[] = {
		SD_BUS_VTABLE_START(0),
		SD_BUS_METHOD(method_name, method_args, method_return, method_handler, SD_BUS_VTABLE_UNPRIVILEGED),
		SD_BUS_VTABLE_END
	};

	assert(g_methods == NULL);
	g_methods[index_data].method_vtable = malloc(3 * sizeof(sd_bus_vtable));

	memcpy(g_methods[index_data].method_vtable, new_methods_vtable, 3 * sizeof(sd_bus_vtable));
}

void init_method_data(const char *method_name, const char *output_sig, const char *input_sig, void *method_cfunc, int index_data)
{
	g_methods[index_data].method_data.method_name = strdup(method_name);
	g_methods[index_data].method_data.output_sig = strdup(output_sig);
	g_methods[index_data].method_data.input_sig = strdup(input_sig);
	//
	g_methods[index_data].method_data.method_cfunc = method_cfunc;
}

int get_sd_bus_fd()
{
	return sd_bus_get_fd(acquire_server_bus());
}

int listen()
{
	dbg("listen ...");
	int r = 0;
	struct epoll_event ev;
	int bus_fd = sd_bus_get_fd(acquire_server_bus());
	int epoll_fd = epoll_create1(0);
	g_event_fd = eventfd(0, 0);

	if(epoll_fd < 0)
	{
		set_dc_error(FAILED_EPOLL_CREATE);
		return epoll_fd;
	}

	if(bus_fd < 0)
	{
		set_dc_error(FAILED_TO_GET_SD_FD);
		close(epoll_fd);
		return bus_fd;
	}

	ev.events = EPOLLIN;
	ev.data.fd = bus_fd;
	r = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, bus_fd, &ev);
	if(r < 0)
	{
		set_dc_error(FAILED_EPOLL_CTL);
		close(epoll_fd);
		return r;
	}

	struct epoll_event ev2;
	ev2.events = EPOLLIN;
	ev2.data.fd = g_event_fd;
	r = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, g_event_fd, &ev2);
	if(r < 0)
	{
		set_dc_error(FAILED_EPOLL_CTL);
		close(epoll_fd);
		return r;
	}

	dbg("before event loop");
	while(is_server_bus_open())
	{
		dbg("inside event loop");
		struct epoll_event events[10];
		int nfds = 0;

		//nfds = epoll_wait(epoll_fd, events, 10, -1);
		nfds = run_epoll_wait_threads(epoll_fd, events, 10, 10000);

		for(int i = 0; i < nfds; i++)
		{
			if(events[i].data.fd == bus_fd)
			{
				dbg("before sd_bus_process");
				//r = sd_bus_process(acquire_server_bus(), NULL);
				r = run_sd_bus_process_threads(acquire_server_bus());
				dbg("after sd_bus_process");
				if(r < 0)
				{
					set_dc_error(FAILED_TO_PROCESS_BUS);
					close(epoll_fd);
					close(g_event_fd);
					return r;
				}
			} else if(events[i].data.fd == g_event_fd)
			{
				close(epoll_fd);
				close(g_event_fd);
				break;
			}
		}
	}
	dbg("after event loop");

	close(g_event_fd);
	close(epoll_fd);

	return r;
}

int service_register
(
	sd_bus *bus,
	const char *path,
	const char *if_name,
	const char *method_name,
	const char *output_sig,
	const char *input_sig,
	void *method_cfunc,
	int method_index
)
{
	int r = 0;

	init_method_data(method_name, output_sig, input_sig, method_cfunc, method_index);
	init_vtable(method_name, output_sig, input_sig, method_index, NULL);

	/* Install the object */
	r = sd_bus_add_object_vtable(bus,
								&g_methods[method_index].slot,
								path,  /* object path */
								if_name,  /* interface name */
								g_methods[method_index].method_vtable,
								(void*)(&g_methods[method_index].method_data));
	if (r < 0)
	{
		set_dc_error(FAILED_TO_ADD_OBJECT_VTABLE);
		return r < 0 ? 0 : 1;
	}

	/* Take a well-known service name so that clients can find us */
	r = sd_bus_request_name(bus, if_name, 0);
	if (r < 0)
	{
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
	variant_array_t *return_data
)
{
	int r = 0;
	sd_bus_error error = SD_BUS_ERROR_NULL;
	sd_bus_message *m = NULL;

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

	if(r < 0)
	{
		//fprintf(stderr, "Failed to issue method call: %s\n", error.message);
		set_dc_error_msg(FAILED_TO_ISSUE_METHOD_CALL, error.message);
		goto finish;
	}

	/* Parse the response message */
	size_t output_sig_size = strlen(output_sig);
	allocate_variant_array(&return_data, output_sig_size);
	for(register size_t i = 0; output_sig_size > i; ++i)
	{
		const char type = output_sig[i];
		switch(type)
		{
			case 'i':
			{
				int val = -1;
				r = sd_bus_message_read_basic(m, output_sig[i], &val);
				if(r > 0)
				{
					variant_set_int(&return_data->variants[i], val);
				}
				break;
			}
			case 's':
			{
				char *return_data_tmp = NULL;
				r = sd_bus_message_read_basic(m, output_sig[i], &return_data_tmp);
				if(r > 0 && return_data_tmp)
				{
					variant_set_str(&return_data->variants[i], return_data_tmp);
				}
				break;
			}
			default:
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
	variant_array_t *return_data
)
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
		&(*return_data));
}

void deinit_client(void)
{
	//fprintf(stderr, "deinit call\n");
	client_bus_deinit();
	dc_clean_error();
}

void deinit_server(void)
{
	if(g_event_fd)
	{
		uint64_t dead_beef = 0xDEADBEEF;
		if(write(g_event_fd, &dead_beef, sizeof(dead_beef)) > 0){;;}
		g_event_fd = 0;
	}

	server_bus_deinit();

	for(int i = 0; g_methods && g_methods_amount > i; ++i)
	{
		if(g_methods[i].slot)
		{
			sd_bus_slot_unref(g_methods[i].slot);
			g_methods[i].slot = NULL;
		}
		if(g_methods[i].method_vtable)
		{
			free(g_methods[i].method_vtable);
			g_methods[i].method_vtable = NULL;
		}
		if(g_methods[i].method_data.input_sig)
		{
			free(g_methods[i].method_data.input_sig);
			g_methods[i].method_data.input_sig = NULL;
		}
		if(g_methods[i].method_data.output_sig)
		{
			free(g_methods[i].method_data.output_sig);
			g_methods[i].method_data.output_sig = NULL;
		}
		if(g_methods[i].method_data.method_name)
		{
			free(g_methods[i].method_data.method_name);
			g_methods[i].method_data.method_name = NULL;
		}
	}

	if(g_methods)
	{
		free(g_methods);
		g_methods = NULL;
	}
	
	dc_clean_error();
}
