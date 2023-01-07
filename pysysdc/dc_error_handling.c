#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "dc_error_handling.h"

char *dc_error_msg = NULL;
//dc_error_t dc_errors[] = NULL;

void set_dc_error(const char *msg)
{
	if(msg == NULL) return;

	dc_clean_error();

	dc_error_msg = (char*) calloc(sizeof(*msg)*strlen(msg)+1, sizeof(*msg));
	strcpy(dc_error_msg, msg);
	//fprintf(stderr, "dc_error_msg %s\n", msg);
}

char* get_last_error(int code)
{
	if(!dc_error_msg) return NULL;

	char *strerr_buf = (char*) calloc(1024, sizeof(char));
	strerror_r(-code, strerr_buf, 1024);

	char *buf = (char*) calloc(strlen(dc_error_msg)+1024, sizeof(char));
	sprintf(buf, "%s: %s", dc_error_msg, strerr_buf);

	dc_clean_error();

	return buf;
}

int dc_has_error(void)
{
	return dc_error_msg != NULL;
}

void dc_clean_error(void)
{
	if(dc_error_msg)
	{
		free(dc_error_msg);
		dc_error_msg = NULL;
	}
}

void dc_error_init(void)
{
	//dc_errors = realloc(dc_errors, ++Naccounts * sizeof(*tmp))
}