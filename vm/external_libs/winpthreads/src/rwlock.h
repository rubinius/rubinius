#ifndef WIN_PTHREADS_RWLOCK_H
#define WIN_PTHREADS_RWLOCK_H

#define LIFE_RWLOCK 0xBAB1F0ED
#define DEAD_RWLOCK 0xDEADB0EF

#undef USE_RWLOCK_SRWLock
#undef USE_RWLOCK_pthread_cond
#define USE_RWLOCK_pthread_cond 1

#define INIT_RWLOCK(rwl)  { int r; \
    if (STATIC_RWL_INITIALIZER(*rwl)) { if ((r = rwlock_static_init(rwl))) { if (r != EBUSY) return r; }}}

#define STATIC_RWL_INITIALIZER(x)		((pthread_rwlock_t)(x) == ((pthread_rwlock_t)PTHREAD_RWLOCK_INITIALIZER))

typedef struct rwlock_t rwlock_t;
struct rwlock_t {
    unsigned int valid;
    int busy;
    LONG nex_count; /* Exclusive access counter.  */
    LONG nsh_count; /* Shared access counter. */
    LONG ncomplete; /* Shared completed counter. */
    pthread_mutex_t mex; /* Exclusive access protection.  */
    pthread_mutex_t mcomplete; /* Shared completed protection. */
    pthread_cond_t ccomplete; /* Shared access completed queue.  */
};

#define RWL_SET	0x01
#define RWL_TRY	0x02

void rwl_print(volatile pthread_rwlock_t *rwl, char *txt);
void rwl_print_set(int state);

#endif
