#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import signal
import threading
import time
from pysysdc.sdbus import SDServer

token_data = "Zm9yaDhhOWViZWJjMjYxZmU1MTVlOTlhNjczMWE3NWQ3N2ZiOjoyMDQ3NzY6Omh0dHA6Ly9yZWdpb25zZWZmZWN0aXZlLmNvbS86Ojo6MTUxMDczMjE6OjY2NDcxNDo6MDo6MTo6MTo6OjowOjpkZWZhdWx0Ojow"
def token(arg: str) -> tuple:
	print("Hello from handler token().")
	print("Function runned with argument: '%s'." % arg)
	return True, token_data

def sigterm_handler(signum, frame):
	print("Received SIGTERM (signal %d), exiting gracefully!" % signum)
	exit(0)

def sigquit_handler(signum, frame):
	print("Received SIGQUIT (signal %d), continuing..." % signum)

def bus_listen():
	try:
		sd = SDServer(
			path="/dc/cloud/auth",
			if_name="dc.cloud.auth",
			method_name="Token",
			method_args="s",
			output_sig="is",
			method_return="is"
		)
		sd.set_func(token)
		sd.listen()
	except Exception as e:
		print(e)

signal.signal(signal.SIGTERM, sigterm_handler)
signal.signal(signal.SIGINT, sigterm_handler)
signal.signal(signal.SIGQUIT, sigquit_handler)
thread = threading.Thread(target=bus_listen, daemon=True)
thread.start()
#bus_listen()
ev = threading.Event()
while True:
	ev.wait()
	#time.sleep(5)
