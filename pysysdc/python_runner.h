#pragma once
#include <stdarg.h>

int run_python_func(const char *args_fmt, void *func, char **args, char **buf);
