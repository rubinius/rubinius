#ifndef RBX_UTIL_TIME_H
#define RBX_UTIL_TIME_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <time.h>
const char* timezone_extended(const struct tm* tptr);
time_t mktime_extended(struct tm* tptr, int utc_p, int* err);
size_t strftime_extended(char *s, size_t maxsize, const char *format, const struct tm *timeptr, const struct timespec *ts, int gmt, int off);

#if defined(__cplusplus)
}
#endif

#endif
