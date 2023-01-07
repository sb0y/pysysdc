#include <sys/time.h>
#include <sys/resource.h>
#include <assert.h>
#include <errno.h>

#include "helpers.h"
#include "dc_error_handling.h"

int setrlimit_closest(int resource, const struct rlimit *rlim) 
{
	struct rlimit highest;
	struct rlimit fixed;

	assert(rlim);

	if (setrlimit(resource, rlim) >= 0)
		return 0;

	if (errno != EPERM)
		return -errno;

	/* So we failed to set the desired setrlimit, then let's try
	 * to get as close as we can */
	getrlimit(resource, &highest);

	fixed.rlim_cur = MIN(rlim->rlim_cur, highest.rlim_max);
	fixed.rlim_max = MIN(rlim->rlim_max, highest.rlim_max);

	if (setrlimit(resource, &fixed) < 0)
		return -errno;

	return 0;
}