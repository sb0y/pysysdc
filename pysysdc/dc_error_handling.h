#pragma once

typedef struct 
{
	char *msg;
	int code;
} dc_error_t;

char* get_last_error(int code);
void set_dc_error(const char *msg);
int dc_has_error(void);
void dc_clean_error(void);
void dc_error_init(void);