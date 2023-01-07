#include <Python.h>
#include <string.h>

int run_python_func(void *func, const char *operation, char **result)
{
	int _func_result = 1;
	PyGILState_STATE gstate;
	gstate = PyGILState_Ensure();

	PyObject *pf = (PyObject*)func;
	PyObject *pymargs = Py_BuildValue("(s)", operation);
	PyObject *cr = PyObject_CallObject(pf, pymargs);
	PyObject *func_result = PyTuple_GetItem(cr, 0);
	_func_result = PyLong_AsLong(func_result);
	PyObject *func_data = PyTuple_GetItem(cr, 1);
	PyObject *str = NULL;
	
	if(func_data && func_data != Py_None)
		str = PyUnicode_AsEncodedString(func_data, "utf-8", "~E~");
	
	if(str)
		*result = strdup(PyBytes_AS_STRING(str));
	
	Py_XDECREF(cr);
	Py_XDECREF(str);
	Py_XDECREF(pymargs);
	//Py_XDECREF(func_result);
	//Py_XDECREF(func_data);

	/* Release the thread. No Python API allowed beyond this point. */
	PyGILState_Release(gstate);

	return _func_result;
}