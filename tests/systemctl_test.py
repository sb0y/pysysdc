#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import pysysdc.systemctl as systemctl

print("status: ", systemctl(svc="nginx", action="status"))
print("extended_status: ", systemctl(svc="nginx", action="extended_status"))
#
print("mask: ", systemctl(svc="nginx", action="mask"))
print("unmask: ", systemctl(svc="nginx", action="unmask"))
print("disable: ", systemctl(svc="nginx", action="disable"))
print("enable: ", systemctl(svc="nginx", action="enable"))
#
print("stop: ", systemctl(svc="nginx", action="stop"))
print("start: ", systemctl(svc="nginx", action="start"))
print("restart: ", systemctl(svc="nginx", action="restart"))
print("reload: ", systemctl(svc="nginx", action="reload"))