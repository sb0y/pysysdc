//#include <stdio.h>
//#include <pthread.h>
//#include <stdlib.h>
//#include <sys/types.h>
//#include <unistd.h>
//#include <sys/syscall.h>
#include "connect.h"
#include "dc_error_handling.h"

//static pthread_mutex_t mutex;
static sd_bus *g_client_bus = NULL;
static sd_bus *g_server_bus = NULL;

sd_bus* get_server_bus(void)
{
	return g_server_bus;
}

sd_bus* get_client_bus(void)
{
	return g_client_bus;
}

sd_bus* acquire_server_bus(void)
{
	if(!g_server_bus)
	{
		server_bus_open();
	}
	return g_server_bus;
}

sd_bus* acquire_client_bus(void)
{
	if(!g_client_bus)
	{
		client_bus_open();
	}
	return g_client_bus;
}

sd_bus* bus_open()
{
	//pthread_mutex_lock(&mutex);
		sd_bus *bus = NULL;
		int r = sd_bus_open_system(&bus);
		if (r < 0) {
			set_dc_error(FAILED_TO_CONNECT_TO_SYSTEM_BUS);
			bus = NULL;
		}

	//	fprintf(stderr, "new bus connection: %p\n", g_bus);
	//pthread_mutex_unlock(&mutex);

	return bus;
}

sd_bus* client_bus_open(void)
{
	g_client_bus = bus_open();
	if (dc_has_error()) 
	{
		return NULL;
	}

	return g_client_bus;
}

sd_bus* server_bus_open(void)
{
	g_server_bus = bus_open();
	if (dc_has_error()) 
	{
		return NULL;
	}

	return g_server_bus;
}

void client_bus_close(void)
{
	bus_close(&g_client_bus);
}

void server_bus_close(void)
{
	bus_close(&g_server_bus);
}

void bus_close(sd_bus **bus)
{
	//pthread_mutex_lock(&mutex);
		//pid_t x = syscall(__NR_gettid);
		//fprintf(stderr, "unref %d\n", x);
		if(*bus)
		{
			sd_bus_flush_close_unref(*bus);
			*bus = NULL;
		}
	//pthread_mutex_unlock(&mutex);
}

void server_bus_deinit(void)
{
	server_bus_close();
}

void client_bus_deinit(void)
{
	//fprintf(stdout, "CLIENT DEINIT\n");
	client_bus_close();
}