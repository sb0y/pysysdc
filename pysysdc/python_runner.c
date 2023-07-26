//#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <Python.h>

static PyObject *g_py_func = NULL;

int parse_args(const char *args_fmt, char *arg_values[], Py_ssize_t size, PyObject *func_args_tuple)
{
	for(Py_ssize_t i = 0; i < size; i++) 
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

int run_python_func(const char *args_fmt, void *func, char **args, char **result)
{
	int c_ret_code = -1;

	assert(func == NULL);

	PyGILState_STATE gstate;
	gstate = PyGILState_Ensure();
		PyObject *py_func_args = PyTuple_New(strlen(args_fmt));
		parse_args(args_fmt, args, PyTuple_Size(py_func_args), py_func_args);
		if(!g_py_func)
			g_py_func = (PyObject*)func;
		/*PyObject* Module = PyImport_Import(PyUnicode_FromString("json"));
		if(!Module) 
		{
			fprintf(stderr, "failed to import module\n");
			PyErr_Print();
			PyErr_Clear();
		}*/

		PyObject *func_data = NULL;
		PyObject *call_res = PyObject_CallObject(g_py_func, py_func_args);
		if(call_res)
		{
			if(PyTuple_Size(call_res) > 1)
			{
				PyObject *py_ret_code = PyTuple_GetItem(call_res, 0);
				c_ret_code = PyLong_AsLong(py_ret_code);
				func_data = PyTuple_GetItem(call_res, 1);
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
		
		PyObject *str = NULL;

		if(func_data && func_data != Py_None)
			str = PyUnicode_AsEncodedString(func_data, "utf-8", "~E~");
		
		if(str)
			*result = strdup(PyBytes_AS_STRING(str));
		
		Py_XDECREF(call_res);
		if(str)
			Py_XDECREF(str);
		//Py_XDECREF(py_func_args);
		//Py_XDECREF(py_ret_code);
		//Py_XDECREF(func_data);
	/* Release the thread. No Python API allowed beyond this point. */
	PyGILState_Release(gstate);

	return c_ret_code;
}