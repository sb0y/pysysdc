#pragma once

//extern sd_bus *bus;
#include "helpers.h"

struct dc_method_data {
	char method_return[100];
	char method_args[100];
	char method_name[255];
} method_data;

int service_register(
	const char *path, 
	const char *if_name, 
	const char *method_name, 
	const char *method_return, 
	const char *method_args);

int listen(void);
void set_cfunc(void *func);
void init(void);
void deinit(void);

int client(
	const char *s_name, 
	const char *s_path, 
	const char* i_name, 
	const char *m_name, 
	const char *input_sig, 
	const char *output_sig,
	const char *first_arg,
	const char *second_arg,
	char **return_data);

int b_client(
	sd_bus *bus,
	const char *s_name, 
	const char *s_path, 
	const char* i_name, 
	const char *m_name, 
	const char *input_sig, 
	const char *output_sig,
	const char *first_arg,
	const char *second_arg,
	char **return_data);