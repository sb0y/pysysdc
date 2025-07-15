#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from pysysdc.sdbus import SDClient

sd = SDClient(
	service_name="dc.cloud.auth",
	path="/dc/cloud/auth", 
	if_name="dc.cloud.auth", 
	method_name="Token", 
	method_args="s", 
	method_return="is", 
	output_sig="is")
ret = sd.send(method_name="Token")
print(ret)
