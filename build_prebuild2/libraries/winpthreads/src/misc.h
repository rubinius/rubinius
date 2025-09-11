#ifndef WIN_PTHREADS_MISC_H
#define WIN_PTHREADS_MISC_H

#ifndef assert

#ifndef ASSERT_TRACE
# define ASSERT_TRACE 0
#else
# undef ASSERT_TRACE
# define ASSERT_TRACE 0
#endif

# define assert(e) \
   ((e) ? ((ASSERT_TRACE) ? fprintf(stderr, \
                                    "Assertion succeeded: (%s), file %s, line %d\n", \
                        #e, __FILE__, (int) __LINE__), \
                                fflush(stderr) : \
                             0) : \
          (fprintf(stderr, "Assertion failed: (%s), file %s, line %d\n", \
                   #e, __FILE__, (int) __LINE__), exit(1), 0))

# define fixme(e) \
   ((e) ? ((ASSERT_TRACE) ? fprintf(stderr, \
                                    "Assertion succeeded: (%s), file %s, line %d\n", \
                        #e, __FILE__, (int) __LINE__), \
                                fflush(stderr) : \
                             0) : \
          (fprintf(stderr, "FIXME: (%s), file %s, line %d\n", \
                   #e, __FILE__, (int) __LINE__), 0, 0))

#endif

#define UINT2PTR(x)								((void *)(uintptr_t)(x))
#define PTR2INT(x)								((int)(uintptr_t)(x))

#if SIZE_MAX>UINT_MAX
typedef long long LONGBAG;
#else
typedef long LONGBAG;
#endif

#define CHECK_HANDLE(h) { DWORD dwFlags; \
    if (!(h) || ((h) == INVALID_HANDLE_VALUE) || !GetHandleInformation((h), &dwFlags)) \
    return EINVAL; }

#define CHECK_PTR(p)    if (!(p)) return EINVAL;

#define UPD_RESULT(x,r)    { int _r=(x); r = r ? r : _r; }

#define CHECK_THREAD(t)  { \
    CHECK_PTR(t); \
    CHECK_HANDLE(t->h); }

#define CHECK_OBJECT(o, e)  { DWORD dwFlags; \
    if (!(o)) return e; \
    if (!((o)->h) || (((o)->h) == INVALID_HANDLE_VALUE) || !GetHandleInformation(((o)->h), &dwFlags)) \
        return e; }

#define VALID(x)    if (!(p)) return EINVAL;

/* ms can be 64 bit, solve wrap-around issues: */
#define dwMilliSecs(ms)		((ms) >= INFINITE ? INFINITE : (DWORD)(ms))

#ifndef _mm_pause
#define _mm_pause()			{__asm__ __volatile__("pause");}
#endif

#ifndef _ReadWriteBarrier
#define _ReadWriteBarrier   __sync_synchronize
#endif

#ifndef YieldProcessor
#define YieldProcessor      _mm_pause
#endif

unsigned long long _pthread_time_in_ms(void);
unsigned long long _pthread_time_in_ms_from_timespec(const struct timespec *ts);
unsigned long long _pthread_rel_time_in_ms(const struct timespec *ts);

#endif
