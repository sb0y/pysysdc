//#include <stdio.h>
#include "sdbus.h"
#include "connect.h"
#include "variant.h"
#include "dc_error_handling.h"
#include "systemd_methods.h"

//#define DC_DEBUG 1
#include "debug.h"

#include <Python.h>

#define _STR_IS_EMPTY(STR)  ((!(STR) || (STR)[0]=='\0' ? 1 : 0))

static char module_docstring[] =
	"This module provides an interface for doing vector subtraction.";
static char service_register_docstring[] =
	"Register service";
static char get_last_error_docstring[] =
	"Get last error text";
static char listen_docstring[] =
	"Listen the bus";
static char deinit_client_docstring[] = 
	"client destructor";
static char deinit_server_docstring[] = 
	"server destructor";
static char init_docstring[] = 
	"constructor";
static char send_docstring[] = 
	"send message to bus";
static char unit_enable_docstring[] = 
	"enables unit";
static char unit_disable_docstring[] = 
	"disables unit";
static char unit_mask_docstring[] = 
	"masks unit";
static char unit_unmask_docstring[] = 
	"unmasks unit";
static char unit_status_docstring[] = 
	"returns unit status";
static char set_hostname_docstring[] = 
	"sets machine hostname via systemd";
static char get_hostname_docstring[] = 
	"get machine hostnames via systemd";
static char machine_reboot_docstring[] = 
	"reboot the machine via systemd (logind)";
static char machine_poweroff_docstring[] = 
	"poweroff the machine via systemd (logind)";
static char set_wall_message_docstring[] = 
	"set wall message";
static char set_timezone_docstring[] = 
	"set timezone in system";
static char daemon_reload_docstring[] = 
	"reload systemd daemon";
static char unit_control_docstring[] = 
	"common unit control method";
static char get_sd_bus_fd_docstring[] = 
	"get sd-bus descriptor";

static PyObject* pysysdc_deinit_client(PyObject *self, PyObject *args)
{
	deinit_client();
	Py_RETURN_TRUE;
}

static PyObject* pysysdc_deinit_server(PyObject *self, PyObject *args)
{
	deinit_server();
	Py_RETURN_TRUE;
}

static PyObject* pysysdc_init(PyObject *self, PyObject *args)
{
	int methods_amount = 0;
	if(!PyArg_ParseTuple(args, "i", &methods_amount))
	{
		return NULL;
	}

	init(methods_amount);

	Py_RETURN_TRUE;
}

static PyObject* pysysdc_service_register(PyObject *self, PyObject *args)
{
	int ret = -1;
	char *path = NULL;
	char *if_name = NULL;
	char *method_name = NULL;
	char *output_sig = NULL;
	char *input_sig = NULL;
	int method_index = 0;

	PyObject *python_method = NULL;

	if(!PyArg_ParseTuple
	(
		args,
		"s|s|s|s|s|O|i",
		&path,
		&if_name,
		&method_name,
		&output_sig,
		&input_sig,
		&python_method,
		&method_index
	))
	{
		return NULL;
	}

	if(_STR_IS_EMPTY(path) || _STR_IS_EMPTY(if_name) || _STR_IS_EMPTY(method_name) || _STR_IS_EMPTY(output_sig)
		|| _STR_IS_EMPTY(input_sig))
	{
		PyErr_SetString(PyExc_AttributeError, "service_register() is called without required arguments");
		Py_RETURN_NONE;
	}

	if(python_method)
	{
		if(!PyCallable_Check(python_method))
		{
			PyErr_SetString(PyExc_AttributeError, "service_register() argument `python_method` must be callable");
			Py_RETURN_NONE;
		}

		Py_INCREF(python_method);
	}

	Py_BEGIN_ALLOW_THREADS
		ret = service_register
		(
			acquire_server_bus(),
			path,
			if_name,
			method_name,
			output_sig,
			input_sig,
			python_method,
			method_index
		);
	Py_END_ALLOW_THREADS

	return Py_BuildValue("i", ret);
}

static PyObject* pysysdc_get_last_error(PyObject *self, PyObject *args)
{
	PyObject *ret = NULL;
	int code = 0;
	char *err_text_mem = NULL;
		
	if(!PyArg_ParseTuple(args, "i", &code))
	{
		return NULL;
	}
	
	Py_BEGIN_ALLOW_THREADS
		err_text_mem = get_last_error(code);
	Py_END_ALLOW_THREADS
	
	ret = Py_BuildValue("s", err_text_mem);
	if(err_text_mem)
		free(err_text_mem);
	
	return ret;
}

static PyObject* pysysdc_get_sd_bus_fd(PyObject *self, PyObject *args)
{
	int sd_fd = get_sd_bus_fd();
	return Py_BuildValue("i", sd_fd);
}

static PyObject* pysysdc_listen(PyObject *self, PyObject *args)
{
	int ret = listen();
	return Py_BuildValue("i", ret);
}

static PyObject* pysysdc_unit_control(PyObject *self, PyObject *args)
{
	char *unit_name = NULL; 
	char *action = NULL;
	char *return_data = NULL;
	int sd_ret = 0;

	PyObject *cmd_ret = NULL;

	if (!PyArg_ParseTuple(args, "s|s", &unit_name, &action))
	{
		return NULL;
	}

	Py_BEGIN_ALLOW_THREADS
		sd_ret = unit_control(unit_name, action, &return_data);
	Py_END_ALLOW_THREADS
	
	cmd_ret = Py_BuildValue("s", return_data);

	if(return_data)
		free(return_data);
	
	return PyTuple_Pack(2, sd_ret ? Py_True : Py_False, cmd_ret);
}

static PyObject* pysysdc_send(PyObject *self, PyObject *args)
{
	char *svc_name = NULL;
	char *path = NULL;
	char *if_name = NULL;
	char *method_name = NULL;
	char *input_sig = NULL;
	char *output_sig = NULL;
	char *first_arg = NULL;
	char *second_arg = NULL;

	variant_array_t return_data = init_variant_array();
	int sd_ret = -1;
	
	PyObject *py_return_tuple = NULL;

	if (!PyArg_ParseTuple(
		args,
		"s|s|s|s|s|s|s|s",
		&svc_name,
		&path,
		&if_name,
		&method_name,
		&input_sig,
		&output_sig,
		&first_arg,
		&second_arg
	))
	{
		return NULL;
	}

	if(_STR_IS_EMPTY(svc_name) || _STR_IS_EMPTY(path) || _STR_IS_EMPTY(if_name) || _STR_IS_EMPTY(method_name) 
		|| _STR_IS_EMPTY(input_sig) || _STR_IS_EMPTY(output_sig))
	{
		PyErr_SetString(PyExc_AttributeError, "send() is called without required arguments");
		Py_RETURN_NONE;
	}

	Py_BEGIN_ALLOW_THREADS
		sd_ret = client(
			svc_name,
			path,
			if_name,
			method_name,
			input_sig,
			output_sig,
			first_arg,
			second_arg,
			&return_data
		);
	Py_END_ALLOW_THREADS

	if(sd_ret < 0)
	{
		return Py_BuildValue("(i)", sd_ret);
	}

	// return code error check
	if(variant_array_is_allocated(return_data) && !variant_is_int(return_data.variants[0]))
	{
		PyErr_SetString(PyExc_AttributeError, "First return argument must be int return code.");
		if(variant_array_is_allocated(return_data))
			variant_array_free(return_data);
		Py_RETURN_NONE;
	}

	size_t py_return_tuple_sz = variant_array_get_size(return_data) + 1; // return data size + sd_ret
	py_return_tuple = PyTuple_New(py_return_tuple_sz);
	// add sd_ret to return tuple
	PyTuple_SetItem(py_return_tuple, 0, Py_BuildValue("i", sd_ret));

	if(variant_array_is_allocated(return_data))
	{
		for(size_t i = 0, j = 1; py_return_tuple_sz > i; ++i, ++j)
		{
			switch(variant_get_state(return_data.variants[i]))
			{
				case variant_state_is_int:
					PyTuple_SetItem(py_return_tuple, j, PyLong_FromLong(variant_get_int(return_data.variants[i])));
				break;
				case variant_state_is_str:
					PyTuple_SetItem(
						py_return_tuple,
						j,
						PyUnicode_FromString(variant_get_str(return_data.variants[i]))
					);
				break;
				default:
				break;
			}
		}
		/* release returned data */
		variant_array_free(return_data);
	}
	
	return py_return_tuple;
}

static PyObject* pysysdc_unit_enable(PyObject *self, PyObject *args)
{
	char *unit_name = NULL;
	int ret = 0;
	char *return_data[3] = { NULL };
	
	PyObject *py_return_data = NULL;

	if(!PyArg_ParseTuple(args, "s", &unit_name))
	{
		return NULL;
	}

	Py_BEGIN_ALLOW_THREADS
		ret = unit_enable(unit_name, return_data);
	Py_END_ALLOW_THREADS

	py_return_data = Py_BuildValue("(sss)", return_data[0], return_data[1], return_data[2]);
		
	for(register int i = 0; i < 3; ++i)
	{
		if(return_data[i])
			free(return_data[i]);
	}

	return PyTuple_Pack(2, ret ? Py_True : Py_False, py_return_data);
}

static PyObject* pysysdc_unit_disable(PyObject *self, PyObject *args)
{
	char *unit_name = NULL;
	int ret = 0;
	char *return_data[3] = { NULL };
	
	PyObject *py_return_data = NULL;

	if (!PyArg_ParseTuple(args, "s", &unit_name))
	{
		return NULL;
	}
	Py_BEGIN_ALLOW_THREADS
		ret = unit_disable(unit_name, return_data);
	Py_END_ALLOW_THREADS

	py_return_data = Py_BuildValue("(sss)", return_data[0], return_data[1], return_data[2]);

	for(register int i = 0; i < 3; ++i)
	{
		if(return_data[i])
			free(return_data[i]);
	}

	return PyTuple_Pack(2, ret ? Py_True : Py_False, py_return_data);
}

static PyObject* pysysdc_unit_mask(PyObject *self, PyObject *args)
{
	char *unit_name = NULL;
	int ret = 0;
	char *return_data[3] = { NULL };
	PyObject *py_return_data = NULL;

	if(!PyArg_ParseTuple(args, "s", &unit_name))
	{
		return NULL;
	}

	Py_BEGIN_ALLOW_THREADS
		ret = unit_mask(unit_name, return_data);
	Py_END_ALLOW_THREADS

	py_return_data = Py_BuildValue("(sss)", return_data[0], return_data[1], return_data[2]);

	for(register int i = 0; i < 3; ++i)
	{
		if(return_data[i])
			free(return_data[i]);
	}
	
	return PyTuple_Pack(2, ret ? Py_True : Py_False, py_return_data);
}

static PyObject* pysysdc_unit_unmask(PyObject *self, PyObject *args)
{
	int ret = 0;
	char *unit_name = NULL;
	char *return_data[3] = { NULL };
	PyObject *py_return_data = NULL;

	if (!PyArg_ParseTuple(args, "s", &unit_name))
	{
		return NULL;
	}

	Py_BEGIN_ALLOW_THREADS
		ret = unit_unmask(unit_name, return_data);
	Py_END_ALLOW_THREADS
	
	py_return_data = Py_BuildValue("(sss)", return_data[0], return_data[1], return_data[2]);
	
	for(register int i = 0; i < 3; ++i)
	{
		if(return_data[i])
			free(return_data[i]);
	}

	return PyTuple_Pack(2, ret ? Py_True : Py_False, py_return_data);
}

static PyObject* pysysdc_unit_status(PyObject *self, PyObject *args)
{
	char *unit_name = NULL;
	int ret = 0;
	char *return_data[3] = { NULL };

	PyObject *py_return_data = NULL;

	if (!PyArg_ParseTuple(args, "s", &unit_name))
	{
		return NULL;
	}
	
	Py_BEGIN_ALLOW_THREADS
		ret = unit_status(unit_name, return_data);
	Py_END_ALLOW_THREADS

	py_return_data = Py_BuildValue(
		"{s:s,s:s,s:s}", 
		"LoadState", return_data[0], 
		"ActiveState", return_data[1], 
		"SubState", return_data[2]
	);
	
	for(register int i = 0; i < 3; ++i)
	{
		if(return_data[i])
			free(return_data[i]);
	}

	return PyTuple_Pack(2, ret ? Py_True : Py_False, py_return_data);
}

static PyObject* pysysdc_set_hostname(PyObject *self, PyObject *args)
{
	char *method = NULL;
	char *value = NULL;
	int r = 0;

	if(!PyArg_ParseTuple(args, "s|s", &method, &value))
	{
		Py_RETURN_FALSE;
	}

	Py_BEGIN_ALLOW_THREADS
		r = set_hostname(method, value);
	Py_END_ALLOW_THREADS

	if(!r)
	{
		return Py_False;
	}

	return Py_True;
}

static PyObject* pysysdc_get_hostname(PyObject *self, PyObject *args)
{
	char *attr = NULL;
	char *ret = NULL;
	int r = 0;
	PyObject *py_ret = NULL;

	if(!PyArg_ParseTuple(args, "s", &attr))
	{
		return Py_False;
	}

	Py_BEGIN_ALLOW_THREADS
		r = get_hostname(attr, &ret);
	Py_END_ALLOW_THREADS

	py_ret = Py_BuildValue("s", ret);
	if(ret)
	{
		free(ret);
	}

	return PyTuple_Pack(2, r ? Py_True : Py_False, py_ret);
}

static PyObject* pysysdc_machine_reboot(PyObject *self, PyObject *args)
{
	char *wall_message = NULL;
	int r = 0;

	PyArg_ParseTuple(args, "s", &wall_message);

	Py_BEGIN_ALLOW_THREADS
		r = machine_reboot(wall_message);
	Py_END_ALLOW_THREADS

	return r ? Py_True : Py_False;
}

static PyObject* pysysdc_machine_poweroff(PyObject *self, PyObject *args)
{
	char *wall_message = NULL;
	int r = 0;
	PyArg_ParseTuple(args, "s", &wall_message);

	Py_BEGIN_ALLOW_THREADS
		r = machine_poweroff(wall_message);
	Py_END_ALLOW_THREADS

	return r ? Py_True : Py_False;
}

static PyObject* pysysdc_set_wall_message(PyObject *self, PyObject *args)
{
	char *wall_message = NULL;
	int r = 0;

	if(!PyArg_ParseTuple(args, "s", &wall_message))
	{
		return Py_False;
	}

	Py_BEGIN_ALLOW_THREADS
		r = set_wall_message(wall_message);
	Py_END_ALLOW_THREADS

	return r ? Py_True : Py_False;
}

static PyObject* pysysdc_set_timezone(PyObject *self, PyObject *args)
{
	char *timezone = NULL;
	int r = 0;

	if(!PyArg_ParseTuple(args, "s", &timezone))
	{
		return Py_False;
	}

	Py_BEGIN_ALLOW_THREADS
		r = set_timezone(timezone);
	Py_END_ALLOW_THREADS
	
	return r ? Py_True : Py_False;
}

static PyObject* pysysdc_daemon_reload(PyObject *self, PyObject *args)
{
	int r = 0;
	Py_BEGIN_ALLOW_THREADS
		r = daemon_reload();
	Py_END_ALLOW_THREADS

	return r ? Py_True : Py_False;
}

/*static int clear_module(PyObject *self)
{
	deinit_client();
	deinit_server();
	return 1;
}

static void free_module(void *p)
{
	deinit_client();
	deinit_server();
}*/

static PyMethodDef module_methods[] = {
	{"get_sd_bus_fd", pysysdc_get_sd_bus_fd, METH_VARARGS, get_sd_bus_fd_docstring},
	{"service_register", pysysdc_service_register, METH_VARARGS, service_register_docstring},
	{"get_last_error", pysysdc_get_last_error, METH_VARARGS, get_last_error_docstring},
	{"listen", pysysdc_listen, METH_VARARGS, listen_docstring},
	{"deinit_client", pysysdc_deinit_client, METH_VARARGS, deinit_client_docstring},
	{"deinit_server", pysysdc_deinit_server, METH_VARARGS, deinit_server_docstring},
	{"init", pysysdc_init, METH_VARARGS, init_docstring},
	{"send", pysysdc_send, METH_VARARGS, send_docstring},
	{"unit_enable", pysysdc_unit_enable, METH_VARARGS, unit_enable_docstring},
	{"unit_disable", pysysdc_unit_disable, METH_VARARGS, unit_disable_docstring},
	{"unit_mask", pysysdc_unit_mask, METH_VARARGS, unit_mask_docstring},
	{"unit_unmask", pysysdc_unit_unmask, METH_VARARGS, unit_unmask_docstring},
	{"unit_status", pysysdc_unit_status, METH_VARARGS, unit_status_docstring},
	{"unit_control", pysysdc_unit_control, METH_VARARGS, unit_control_docstring},
	{"set_hostname", pysysdc_set_hostname, METH_VARARGS, set_hostname_docstring},
	{"get_hostname", pysysdc_get_hostname, METH_VARARGS, get_hostname_docstring},
	{"machine_reboot", pysysdc_machine_reboot, METH_VARARGS, machine_reboot_docstring},
	{"machine_poweroff", pysysdc_machine_poweroff, METH_VARARGS, machine_poweroff_docstring},
	{"set_wall_message", pysysdc_set_wall_message, METH_VARARGS, set_wall_message_docstring},
	{"set_timezone", pysysdc_set_timezone, METH_VARARGS, set_timezone_docstring},
	{"daemon_reload", pysysdc_daemon_reload, METH_VARARGS, daemon_reload_docstring},
	{NULL, NULL, 0, NULL}
};

static struct PyModuleDef _coremodule = {
	PyModuleDef_HEAD_INIT,
	.m_name = "_pysysdc",			/* name of module */
	.m_doc = module_docstring,	/* module documentation, may be NULL */
	.m_size = -1,					/* size of per-interpreter state of the module,
							or -1 if the module keeps state in global variables. */
	.m_methods = module_methods,
	//.m_clear = clear_module,
	//.m_free = free_module
};

PyMODINIT_FUNC PyInit__pysysdc(void)
{
	#if PY_VERSION_HEX <= 0x30502F0 // python3.5
	Py_InitializeEx(0);
	if(!PyEval_ThreadsInitialized())
	{
		PyEval_InitThreads();
	}
	#endif

	return PyModule_Create(&_coremodule);
}
