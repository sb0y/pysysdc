#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from pysysdc.unit import Unit

# systemd service name
u = Unit("token_manager")
print(u.restart())
print(u.stop())
print(u.start())
print(u.reload())
