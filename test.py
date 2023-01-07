#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from pysysdc.sdbus import SDBus

token_data = "Zm9yaDhhOWViZWJjMjYxZmU1MTVlOTlhNjczMWE3NWQ3N2ZiOjoyMDQ3NzY6Omh0dHA6Ly9yZWdpb25zZWZmZWN0aXZlLmNvbS86Ojo6MTUxMDczMjE6OjY2NDcxNDo6MDo6MTo6MTo6OjowOjpkZWZhdWx0Ojow"
def token():
	return True, token_data

sd = SDBus(path="/dc/cloud/auth", if_name="dc.cloud.auth", method_name="Token", method_args="s", output_sig="s", method_return="s")
sd.set_func(token)
sd.listen()
