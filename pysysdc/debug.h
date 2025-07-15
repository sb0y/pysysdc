#pragma once

#ifdef DC_DEBUG

#include <stdio.h>

#define dbg(str) printf(str"\n")

#else

#define dbg(str) ;;

#endif