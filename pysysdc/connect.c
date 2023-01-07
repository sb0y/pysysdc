//#include <stdio.h>
#include <pthread.h>
//#include <stdlib.h>
//#include <sys/types.h>
//#include <unistd.h>
//#include <sys/syscall.h>
#include "connect.h"
#include "dc_error_handling.h"

//pthread_mutex_t mutex;
sd_bus *g_bus = NULL;

sd_bus* get_gbus(void)
{
	return g_bus;
}

sd_bus *acquire_gbus(void)
{
	if(g_bus == NULL)
		bus_open();
	return g_bus;
}

sd_bus* bus_open()
{
	//pthread_mutex_lock(&mutex);
		int r = sd_bus_open_system(&g_bus);
		if (r < 0) {
			set_dc_error("Failed to connect to system bus");
			g_bus = NULL;
			return NULL;
		}

	//	fprintf(stderr, "new bus connection: %p\n", g_bus);
	//pthread_mutex_unlock(&mutex);

	return g_bus;
}

void bus_close()
{
	//pthread_mutex_lock(&mutex);
		if(g_bus != NULL) {
			//pid_t x = syscall(__NR_gettid);
			//fprintf(stderr, "unref %d\n", x);
			sd_bus_unref(g_bus);
			g_bus = NULL;
		}
	//pthread_mutex_unlock(&mutex);
}

void bus_init(void)
{
	//pthread_mutex_init(&mutex, NULL);
}

void bus_deinit(void)
{
	bus_close();
	//pthread_mutex_destroy(&mutex);
}