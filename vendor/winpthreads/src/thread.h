#ifndef WIN_PTHREAD_H
#define WIN_PTHREAD_H

#include <windows.h>
#include <setjmp.h>
#include "rwlock.h"

#define LIFE_THREAD 0xBAB1F00D
#define DEAD_THREAD 0xDEADBEEF

typedef struct _pthread_v _pthread_v;
struct _pthread_v
{
    pthread_t hlp;
    unsigned int valid;   
    void *ret_arg;
    void *(* func)(void *);
    _pthread_cleanup *clean;
    int nobreak;
    HANDLE h;
    HANDLE evStart;
    pthread_mutex_t p_clock;
    int cancelled : 2;
    int in_cancel : 2;
    int thread_noposix : 2;
    unsigned int p_state;
    unsigned int keymax;
    void **keyval;
    DWORD tid;
    int rwlc;
    pthread_rwlock_t rwlq[RWLS_PER_THREAD];
    int sched_pol;
    int ended;
    struct sched_param sched;
    jmp_buf jb;
    struct _pthread_v *next;
    int x; /* Internal posix handle.  */
};

int _pthread_tryjoin(pthread_t t, void **res);
void _pthread_setnobreak(int);
#ifdef WINPTHREAD_DBG
void thread_print_set(int state);
void thread_print(volatile pthread_t t, char *txt);
#endif
int  __pthread_shallcancel(void);

#endif
