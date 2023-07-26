#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "dc_error_handling.h"

#define STRERR_SIZE 1024

enum dc_error_codes_t dc_error_code = DC_NO_ERROR;
char *sd_error = NULL;

static inline const char *enum_to_str(enum dc_error_codes_t err)
{
	static const char *strings[] = 
	{
		[DC_NO_ERROR] = "",
		[BUS_VALUE_IS_NULL] = "bus value is NULL",
		[FUNC_ARG_IS_NULL] = "func arg is NULL",
		[FAILED_TO_PARSE_REPLY] = "Failed to parse response message",
		[FAILED_TO_CONNECT_TO_SYSTEM_BUS] = "Failed to connect to system bus",
		[FAILED_TO_GET_PROPERTY] = "Failed to get property",
		[USER_DATA_IS_NULL] = "User data is null",
		[FAILED_TO_PARSE_PARAMETERS] = "Failed to parse parameters",
		[FAILED_TO_PROCESS_BUS] = "Failed to process bus",
		[FAILED_TO_WAIT_BUS] = "Failed to wait bus",
		[FAILED_TO_ISSUE_METHOD_CALL] = "Failed to issue method call",
		[FAILED_TO_ACQUIRE_SERVICE_NAME] = "Failed to acquire service name",
		[FAILED_TO_CALL_METHOD] = "Failed to call method",
		[FAILED_TO_ADD_OBJECT_VTABLE] = "Failed to add object vtable",
		[FAILED_TO_REPLY_METHOD_RETURN] = "Failed to reply method return",
		[FAILED_TO_RUN_PYTHON_FUNCTION] = "Failed to run python function"
	};

	return strings[err];
}

void set_dc_error(enum dc_error_codes_t err)
{
	dc_error_code = err;
	//fprintf(stderr, "dc_error_msg %s\n", msg);
}

void set_dc_error_msg(enum dc_error_codes_t er_code, const char *err)
{
	dc_error_code = er_code;
	sd_error = strdup(err);
}

char* get_last_error(int code)
{
	char *buf = NULL;
	char *strerr_buf = (char*) calloc(STRERR_SIZE, sizeof(char));
	const char *dc_err_str = enum_to_str(dc_error_code);

	strerror_r(-code, strerr_buf, STRERR_SIZE);

	if(sd_error == NULL)
	{
		buf = (char*) calloc(strlen(dc_err_str)+STRERR_SIZE, sizeof(char));
		sprintf(buf, "%s, %s", dc_err_str, strerr_buf);
	} else {
		buf = (char*) calloc(strlen(dc_err_str)+strlen(sd_error)+STRERR_SIZE, sizeof(char));
		sprintf(buf, "%s, %s, %s", dc_err_str, sd_error, strerr_buf);
	}

	//free(strerr_buf);

	dc_clean_error();

	return buf;
}

int dc_has_error(void)
{
	return dc_error_code != DC_NO_ERROR;
}

void dc_clean_error(void)
{
	dc_error_code = DC_NO_ERROR;
	// python GC free?
	if(sd_error)
	{
		free(sd_error);
		sd_error = NULL;
	}
}

void dc_error_init(void)
{
	//dc_errors = realloc(dc_errors, ++Naccounts * sizeof(*tmp))
}