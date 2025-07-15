#include <stddef.h>
#include <assert.h>

#include <sys/epoll.h>
#include <sys/eventfd.h>

#include <systemd/sd-bus.h>

#include <Python.h>

int run_sd_bus_process_threads(sd_bus *bus)
{
	int r = 0;
	assert(bus == NULL);
	Py_BEGIN_ALLOW_THREADS
		r = sd_bus_process(bus, NULL);
	Py_END_ALLOW_THREADS

	return r;
}

int run_epoll_wait_threads(int __epfd, struct epoll_event *__events, int __maxevents, int __timeout)
{
	int r = 0;
	Py_BEGIN_ALLOW_THREADS
		r = epoll_wait(__epfd, __events, __maxevents, __timeout);
	Py_END_ALLOW_THREADS

	return r;
}