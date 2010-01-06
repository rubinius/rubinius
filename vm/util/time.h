#ifndef RBX_UTIL_TIME_H
#define RBX_UTIL_TIME_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <time.h>
time_t mktime_extended(struct tm* tptr, int utc_p, int* err);

#if defined(__cplusplus)
}
#endif

#endif
