#include <stdlib.h>
#include <string.h>
#include <signal.h>
//#include <stdio.h>
#include "python_runner.h"

static void (*g_term_func)(void) = NULL;

void set_terminate_func(void (*f)())
{
	g_term_func = f;
}

static void signal_handler(int signal)
{
	switch(signal)
	{
		case SIGINT:
		case SIGTERM:
		case SIGQUIT:
		{
			if(g_term_func)
			{
				(*g_term_func)();
			};
			set_py_interrupt();
		}
		break;
		default:
		break;
	}
}

void set_signal_actions(void)
{
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGQUIT, signal_handler);
}