#pragma once

#include <systemd/sd-bus.h>

sd_bus* get_gbus(void);
sd_bus* bus_open(void);
void bus_close(void);
sd_bus *acquire_gbus(void);
void bus_init(void);
void bus_deinit(void);