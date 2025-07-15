#pragma once

#define PY_CODE_INTERRUPTED -2000

typedef struct variant_array_t variant_array_t;

int run_python_func(const char *args_fmt, void *func, char **args, variant_array_t *results);
void set_py_interrupt(void);
int py_signals_need_iterrupt(void);
