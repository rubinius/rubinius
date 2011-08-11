#ifndef WIN_PTHREADS_COND_H
#define WIN_PTHREADS_COND_H

#include <windows.h>

#define CHECK_COND(c)  { \
    if (!(c) || !*c || (*c == PTHREAD_COND_INITIALIZER) \
        || ( ((cond_t *)(*c))->valid != (unsigned int)LIFE_COND ) ) \
        return EINVAL; }

#define LIFE_COND 0xC0BAB1FD
#define DEAD_COND 0xC0DEADBF

#define STATIC_COND_INITIALIZER(x)		((pthread_cond_t)(x) == ((pthread_cond_t)PTHREAD_COND_INITIALIZER))

typedef struct cond_t cond_t;
struct cond_t
{
    unsigned int valid;   
    int busy;
    LONG waiters_count_; /* Number of waiting threads.  */
    LONG waiters_count_unblock_; /* Number of waiting threads whitch can be unblocked.  */
    LONG waiters_count_gone_; /* Number of waiters which are gone.  */
    CRITICAL_SECTION waiters_count_lock_; /* Serialize access to <waiters_count_>.  */
    CRITICAL_SECTION waiters_q_lock_; /* Serialize access to sema_q.  */
    LONG value_q;
    CRITICAL_SECTION waiters_b_lock_; /* Serialize access to sema_b.  */
    LONG value_b;
    HANDLE sema_q; /* Semaphore used to queue up threads waiting for the condition to
                 become signaled.  */
    HANDLE sema_b; /* Semaphore used to queue up threads waiting for the condition which
                 became signaled.  */
};

void cond_print_set(int state, FILE *f);

void cond_print(volatile pthread_cond_t *c, char *txt);

#endif
