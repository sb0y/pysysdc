#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from pysysdc.sdbus import SDServer

token_data = "Zm9yaDhhOWViZWJjMjYxZmU1MTVlOTlhNjczMWE3NWQ3N2ZiOjoyMDQ3NzY6Omh0dHA6Ly9yZWdpb25zZWZmZWN0aXZlLmNvbS86Ojo6MTUxMDczMjE6OjY2NDcxNDo6MDo6MTo6MTo6OjowOjpkZWZhdWx0Ojow"
def token(arg: str) -> tuple:
	print("Hello from handler token().")
	print("Function runned with argument: '%s'." % arg)
	return True, token_data

def config(mode: str, data: str) -> tuple:
	print("Hello from handler config().")
	print("Mode: '%s'" % mode)
	print("Data: '%s'" % data)
	return True, 'done', 'test', 10

sd = SDServer(
	(
		{"path": "/dc/cloud/auth", 
		"if_name": "dc.cloud.auth", 
		"method_name": "Token", 
		"input_sig": "s", 
		"output_sig": "is", 
		"method_handler": token},

		{"path": "/dc/cloud/auth",
		"if_name": "dc.cloud.auth.config", 
		"method_name": "Config", 
		"input_sig": "ss", 
		"output_sig": "issi", 
		"method_handler": config},
	),

)
sd.listen() # blocking
