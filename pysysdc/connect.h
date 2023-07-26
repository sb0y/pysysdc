#pragma once

#include <systemd/sd-bus.h>

sd_bus* get_server_bus(void);
sd_bus* get_client_bus(void);

sd_bus* acquire_server_bus(void);
sd_bus* acquire_client_bus(void);

sd_bus* client_bus_open(void);
sd_bus* server_bus_open(void);

void client_bus_close(void);
void server_bus_close(void);

void server_bus_deinit(void);
void client_bus_deinit(void);

sd_bus* bus_open(void);
void bus_close(sd_bus**);
