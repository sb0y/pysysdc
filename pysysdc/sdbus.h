#pragma once

#include "helpers.h"
#include "variant.h"

typedef struct dc_method_data {
	char *output_sig;
	char *input_sig;
	char *method_name;
	void *method_cfunc;
} dc_method_data;

typedef struct dc_method {
	dc_method_data method_data;
	sd_bus_vtable *method_vtable;
	sd_bus_slot *slot;
} dc_method;

int service_register(
	sd_bus *bus,
	const char *path,
	const char *if_name,
	const char *method_name,
	const char *output_sig,
	const char *input_sig,
	void *method_cfunc,
	int method_index
);

int listen();
void init(int methods_amount);
void deinit_server(void);
void deinit_client(void);

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
);

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
);

int get_sd_bus_fd();