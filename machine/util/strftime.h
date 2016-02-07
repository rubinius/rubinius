#ifndef RBX_UTIL_TIME_H
#define RBX_UTIL_TIME_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <time.h>
#include "util/time64.h"
size_t strftime_extended(char *s, size_t maxsize, const char *format, const struct tm64 *timeptr, const struct timespec64 *ts, int gmt, int off);

#if defined(__cplusplus)
}
#endif

#endif
