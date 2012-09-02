#ifndef TIME64_H
#define TIME64_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>
#include <time.h>

/* Only works when system provides a 64 bit type. If the system has this
 * type available, loading stdint.h means int64_t is required to be available:
 * If an implementation provides integer types with width 64 that meet these
 * requirements, then the following types are required: int64_t uint64_t
 */
typedef int64_t time64_t;

/* Uses 64 bit years so we can handle the whole range of time64_t timestamps.
 * tm_year here stores normal year numbers, so not from 1900.
 */
struct tm64 {
    int64_t tm_year;
    int tm_mon;
    int tm_mday;
    int tm_hour;
    int tm_min;
    int tm_sec;
    int tm_nano;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
    int tm_gmtoff;
    char *tm_zone;
};

struct timespec64 {
  time64_t tv_sec;
  long tv_nsec;
};

/* Only provide reentrant versions, nobody should use the non reentrant
 * versions anyway imho so we don't bother providing them. */
struct tm64* gmtime64_r   (const time64_t *, struct tm64 *);
struct tm64* localtime64_r(const time64_t *, struct tm64 *);

time64_t   mktime64   (struct tm64 *);
time64_t   timegm64   (struct tm64 *);
time64_t   timelocal64(struct tm64 *);

#if defined(__cplusplus)
}
#endif

#endif


