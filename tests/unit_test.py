#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from pysysdc.unit import Unit

# systemd service name
u = Unit("nginx")
print(repr(u.stop()))
print(u.start())
print(u.reload())
print(u.restart())