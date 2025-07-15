#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from pysysdc.sdbus import SDClient

sd = SDClient(
	service_name="dc.cloud.auth",
	path="/dc/cloud/auth",
	if_name="dc.cloud.auth",
	method_name="Token",
	input_sig="s",
	output_sig="is"
)
ret = sd.send(method_name="Token", first_arg="get")
print(ret)
print(repr(ret))