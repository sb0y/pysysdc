#include <string.h>
#include <assert.h>
#include <stdarg.h>

#include "variant.h"

#include <Python.h>

#include "python_runner.h"

void set_py_interrupt(void)
{
	PyErr_SetInterrupt();
}

int py_signals_need_iterrupt(void)
{
	if(PyErr_CheckSignals() != 0)
	{
		return 1;
	}

	return 0;
}

int parse_args(const char *args_fmt, char *arg_values[], Py_ssize_t size, PyObject *func_args_tuple)
{
	for(Py_ssize_t i = 0; i < size; ++i)
	{
		if (args_fmt[i] == 's') 
		{
			if(!arg_values[i])
				continue;
			PyTuple_SetItem(func_args_tuple, i, PyUnicode_FromString(arg_values[i]));
		}
	}
	return 1;
}

int run_python_func(const char *args_fmt, void *func, char **args, variant_array_t *results)
{
	int c_ret_code = -1;

	assert(func == NULL);
	assert(args == NULL);
	assert(results == NULL);

	PyGILState_STATE gstate = PyGILState_Ensure();
		if(py_signals_need_iterrupt())
		{
			PyGILState_Release(gstate);
			return PY_CODE_INTERRUPTED;
		}

		PyObject *py_func_args = PyTuple_New(strlen(args_fmt));
		parse_args(args_fmt, args, PyTuple_Size(py_func_args), py_func_args);

		PyObject *call_res = PyObject_CallObject((PyObject*)func, py_func_args);
		if(call_res)
		{
			Py_ssize_t call_res_size = PyTuple_Size(call_res);
			if(call_res_size > 0)
			{
				allocate_variant_array(&results, call_res_size);
				// first element of tuple is always the ret_code
				PyObject *py_ret_code = PyTuple_GetItem(call_res, 0);
				c_ret_code = PyLong_AsLong(py_ret_code);

				for(Py_ssize_t i = 0; call_res_size > i; ++i)
				{
					PyObject *func_data = PyTuple_GetItem(call_res, i);
					if(func_data && func_data != Py_None)
					{
						if(PyLong_Check(func_data))
						{
							variant_set_int(&results->variants[i], PyLong_AsLong(func_data));
						//} else if(PyBytes_Check(func_data))
						} else if(PyUnicode_Check(func_data))
						{
							PyObject *str = PyUnicode_AsEncodedString(func_data, "utf-8", "~E~");
							if(str)
							{
								variant_set_str(&results->variants[i], PyBytes_AS_STRING(str));
								Py_XDECREF(str);
							}
						}
					}
				}
			}
		} else {
			if(PyErr_Occurred()) 
			{
				//PyErr_Print();
				//PyErr_PrintEx(0);
				PyErr_PrintEx(1);
				PyErr_Clear();
			}
		}
		Py_XDECREF(call_res);
		//Py_XDECREF(py_func_args);
		//Py_XDECREF(py_ret_code);
		//Py_XDECREF(func_data);
	/* Release the thread. No Python API allowed beyond this point. */
	PyGILState_Release(gstate);

	return c_ret_code;
}