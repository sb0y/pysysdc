#pragma once

#include "helpers.h"

#define METHOD_RETURN_SIZE 256
#define METHOD_ARGS_SIZE 256
#define METHOD_NAME_SIZE 256

typedef struct dc_method_data {
	char method_return[METHOD_RETURN_SIZE];
	char method_args[METHOD_ARGS_SIZE];
	char method_name[METHOD_NAME_SIZE];
} dc_method_data;

int service_register(
	sd_bus *bus,
	const char *path, 
	const char *if_name, 
	const char *method_name, 
	const char *method_return, 
	const char *method_args);

int listen(sd_bus *bus);
void set_cfunc(void *func);
void init(void);
void deinit_server(void);
void deinit_client(void);

int client(
	const char *s_name, 
	const char *s_path, 
	const char* i_name, 
	const char *m_name, 
	const char *input_sig, 
	const char *output_sig,
	const char *first_arg,
	const char *second_arg,
	char **return_data,
	int *return_code);

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
	int *return_data_code);