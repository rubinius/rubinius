#ifndef __SHOTGUN__
#define __SHOTGUN__ 1
#define INTERNAL_MACROS 1
#define INTERNAL_DEBUG 1

#define TRACK_STATS 1
#define DISABLE_CHECKS 1

#ifndef MAIN
extern void *__main_address;
#endif

#include "rubinius.h"
#include "state.h"
#include "object.h"
#include "auto.h"
#include "log.h"
#endif
