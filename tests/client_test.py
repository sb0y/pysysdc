#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from pysysdc.sdbus import SDBus

sd = SDBus(
	path="/dc/cloud/auth", 
	if_name="dc.cloud.auth", 
	method_name="Token", 
	method_args="s", 
	method_return="s", 
	output_sig="s")
ret = sd.send(method_name="Token")
print(ret)
