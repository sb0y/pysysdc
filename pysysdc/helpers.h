#pragma once

#include <sys/resource.h>
#include <systemd/sd-bus.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define RLIMIT_MAKE_CONST(lim) ((struct rlimit) { lim, lim })

int setrlimit_closest(int resource, const struct rlimit *rlim);