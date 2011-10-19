#include "pthread.h"
#include "misc.h"

unsigned long long _pthread_time_in_ms(void)
{
    struct _timeb tb;

    _ftime(&tb);
    return (unsigned long long)tb.time * 1000ULL + (unsigned long long) tb.millitm;
}

unsigned long long _pthread_time_in_ms_from_timespec(const struct timespec *ts)
{
    unsigned long long t = ts->tv_sec * 1000;
    t += (unsigned long long) ts->tv_nsec / 1000000ULL;

    return t;
}

unsigned long long _pthread_rel_time_in_ms(const struct timespec *ts)
{
    unsigned long long t1 = _pthread_time_in_ms_from_timespec(ts);
    unsigned long long t2 = _pthread_time_in_ms();

    /* Prevent underflow */
    if (t1 < t2) return 0;
    return t1 - t2;
}

