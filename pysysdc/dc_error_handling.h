#pragma once

typedef struct 
{
	char *msg;
	int code;
} dc_error_t;

enum dc_error_codes_t
{
	DC_NO_ERROR = 0,
	BUS_VALUE_IS_NULL,
	FUNC_ARG_IS_NULL,
	FAILED_TO_PARSE_REPLY,
	FAILED_TO_CONNECT_TO_SYSTEM_BUS,
	FAILED_TO_GET_PROPERTY,
	USER_DATA_IS_NULL,
	FAILED_TO_PARSE_PARAMETERS,
	FAILED_TO_PROCESS_BUS,
	FAILED_TO_WAIT_BUS,
	FAILED_TO_ISSUE_METHOD_CALL,
	FAILED_TO_ACQUIRE_SERVICE_NAME,
	FAILED_TO_CALL_METHOD,
	FAILED_TO_ADD_OBJECT_VTABLE,
	FAILED_TO_REPLY_METHOD_RETURN,
	FAILED_TO_RUN_PYTHON_FUNCTION,
	DC_ERROR_MAX
};

char* get_last_error(int code);
void set_dc_error(enum dc_error_codes_t err);
void set_dc_error_msg(enum dc_error_codes_t er_code, const char *err);
int dc_has_error(void);
void dc_clean_error(void);
void dc_error_init(void);