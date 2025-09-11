#include <windows.h>
#include <winternl.h>
#include <stdio.h>
#include "pthread.h"
#include "spinlock.h"
#include "ref.h"
#include "mutex.h"
#include "misc.h"

extern int do_sema_b_wait_intern (HANDLE sema, int nointerrupt, DWORD timeout);
static __attribute__((noinline)) int mutex_static_init(pthread_mutex_t *m);
static __attribute__((noinline)) int _mutex_trylock(pthread_mutex_t *m);

static spin_t mutex_global = {0,LIFE_SPINLOCK,0};
static spin_t mutex_global_static = {0,LIFE_SPINLOCK,0};

static __attribute__((noinline)) int
mutex_unref(pthread_mutex_t *m, int r)
{
    mutex_t *m_ = (mutex_t *)*m;
    _spin_lite_lock(&mutex_global);
#ifdef WINPTHREAD_DBG
    assert((m_->valid == LIFE_MUTEX) && (m_->busy > 0));
#endif
    m_->busy --;
    _spin_lite_unlock(&mutex_global);
    return r;
}

/* Set the mutex to busy in a thread-safe way */
/* A busy mutex can't be destroyed */
static __attribute__((noinline)) int
mutex_ref(pthread_mutex_t *m )
{
    int r = 0;

    _spin_lite_lock(&mutex_global);
    if (!m || !*m)
    {
      _spin_lite_unlock(&mutex_global);
      return EINVAL;
    }
    if (STATIC_INITIALIZER(*m))
    {
      _spin_lite_unlock(&mutex_global);
      r = mutex_static_init(m);
      _spin_lite_lock(&mutex_global);
      if (r != 0 && r != EBUSY)
      {
	_spin_lite_unlock(&mutex_global);
	return r;
      }
    }
    r = 0;
    if (!m || !*m || ((mutex_t *)*m)->valid != LIFE_MUTEX) r = EINVAL;
    else {
        ((mutex_t *)*m)->busy ++;
    }

    _spin_lite_unlock(&mutex_global);

    return r;
}

/* An unlock can simply fail with EPERM instead of auto-init (can't be owned) */
static __attribute__((noinline)) int
mutex_ref_unlock(pthread_mutex_t *m)
{
    int r = 0;
    mutex_t *m_ = (mutex_t *)*m;

    _spin_lite_lock(&mutex_global);

    if (!m || !*m || ((mutex_t *)*m)->valid != LIFE_MUTEX) r = EINVAL;
    else if (STATIC_INITIALIZER(*m) || !COND_LOCKED(m_)) {
      r= EPERM;
    }
    else {
        ((mutex_t *)*m)->busy ++;
    }

    _spin_lite_unlock(&mutex_global);

    return r;
}

/* doesn't lock the mutex but set it to invalid in a thread-safe way */
/* A busy mutex can't be destroyed -> EBUSY */
static __attribute__((noinline)) int
mutex_ref_destroy(pthread_mutex_t *m, pthread_mutex_t *mDestroy )
{
    int r = 0;

    *mDestroy = NULL;
    /* also considered as busy, any concurrent access prevents destruction: */
    if (_spin_lite_trylock(&mutex_global)) return EBUSY;
    
    if (!m || !*m) r = EINVAL;
    else {
        mutex_t *m_ = (mutex_t *)*m;
        if (STATIC_INITIALIZER(*m)) *m = NULL;
        else if (m_->valid != LIFE_MUTEX) r = EINVAL;
        else if (m_->busy || COND_LOCKED(m_)) r = EBUSY;
        else {
            *mDestroy = *m;
            *m = NULL;
        }
    }

    _spin_lite_unlock(&mutex_global);
    return r;
}

static __attribute__((noinline)) int mutex_ref_init(pthread_mutex_t *m )
{
    int r = 0;

    _spin_lite_lock(&mutex_global);
    
    if (!m)  r = EINVAL;
 
    _spin_lite_unlock(&mutex_global);
    return r;
}

#ifdef WINPTHREAD_DBG
static int print_state = 1;

void mutex_print_set(int state)
{
    print_state = state;
}

void mutex_print(pthread_mutex_t *m, char *txt)
{
    if (!print_state) return;
    mutex_t *m_ = (mutex_t *)*m;
    if (m_ == NULL) {
        printf("M%p %d %s\n",*m,(int)GetCurrentThreadId(),txt);
    } else {
        printf("M%p %d V=%0X B=%d t=%d o=%d C=%d R=%d H=%p %s\n",
            *m, 
            (int)GetCurrentThreadId(), 
            (int)m_->valid, 
            (int)m_->busy,
            m_->type,
            (int)GET_OWNER(m_),(int)(m_->count),(int)GET_RCNT(m_),GET_HANDLE(m_),txt);
    }
}
#endif

static __attribute__((noinline)) int
mutex_static_init(pthread_mutex_t *m)
{
    static pthread_mutexattr_t mxattr_recursive = PTHREAD_MUTEX_RECURSIVE;
    static pthread_mutexattr_t mxattr_errorcheck = PTHREAD_MUTEX_ERRORCHECK;

    int r;

    _spin_lite_lock(&mutex_global_static);
    if (!STATIC_INITIALIZER(*m)) {
        /* Assume someone crept in between: */
        r = 0;
    }
    else
    {
      if (*m == PTHREAD_MUTEX_INITIALIZER)
	r = pthread_mutex_init (m, NULL);
      else if (*m == PTHREAD_RECURSIVE_MUTEX_INITIALIZER)
	r = pthread_mutex_init (m, &mxattr_recursive);
      else if (*m == PTHREAD_ERRORCHECK_MUTEX_INITIALIZER)
	r = pthread_mutex_init (m, &mxattr_errorcheck);
      else if (*m == NULL)
	r = EINVAL;
      else
	r = pthread_mutex_init(m, NULL);
    }
    _spin_lite_unlock(&mutex_global_static);
    return r;
}

static int pthread_mutex_lock_intern(pthread_mutex_t *m, DWORD timeout);

int pthread_mutex_lock(pthread_mutex_t *m)
{
  return pthread_mutex_lock_intern(m, INFINITE);
}

static int pthread_mutex_lock_intern(pthread_mutex_t *m, DWORD timeout)
{
    mutex_t *_m;
    int r;
    r = mutex_ref(m);
    if(r) return r;

    _m = (mutex_t *)*m;
    if (_m->type != PTHREAD_MUTEX_NORMAL)
    {
      if (COND_LOCKED(_m))
      {
	if (COND_OWNER(_m))
	{
	  if (_m->type == PTHREAD_MUTEX_RECURSIVE)
	  {
	    InterlockedIncrement(&_m->count);
	    return mutex_unref(m,0);
	  }
	  return mutex_unref(m, EDEADLK);
	}
      }
    } else {
#if !defined USE_MUTEX_Mutex
      if (COND_OWNER(_m))
      {
	do {
	  Sleep(0); /* waiter that owner gets released.  */
	} while (COND_OWNER(_m));
      }
#endif
    }
#if defined USE_MUTEX_Mutex
    r = do_sema_b_wait_intern (_m->h, 1, timeout);
#else /* USE_MUTEX_CriticalSection */
    EnterCriticalSection(&_m->cs.cs);
    r = 0;
#endif
    if (r == 0)
    {
      _m->count = 1;
      SET_OWNER(_m);
    }
    return mutex_unref(m,r);

}

/*	
    See this article: http://msdn.microsoft.com/nl-nl/magazine/cc164040(en-us).aspx#S8
    Unfortunately, behaviour has changed in Win 7 and maybe Vista (untested). Who said
    "undocumented"? Some testing and tracing revealed the following:
    On Win 7:
    - LockCount is DECREMENTED with 4. The low bit(0) is set when the cs is unlocked. 
      Also bit(1) is set, so lock sequence goes like -2,-6, -10 ... etc.
    - The LockSemaphore event is only auto-initialized at contention. 
      Although pthread_mutex_timedlock() seemed to work, it just busy-waits, because
      WaitForSingleObject() always returns WAIT_FAILED+INVALID_HANDLE. 
    Solution on Win 7:
    - The LockSemaphore member MUST be auto-initialized and only at contention. 
      Pre-initializing messes up the cs state (causes EnterCriticalSection() to hang)
    - Before the lock (wait): 
      - auto-init LockSemaphore
      - LockCount is DECREMENTED with 4
    - Data members must be also correctly updated when the lock is obtained, after
      waiting on the event:
      - Increment LockCount, setting the low bit
      - set RecursionCount to 1
      - set OwningThread to CurrentThreadId()
    - When the wait failed or timed out, reset LockCount (INCREMENT with 4)
    On Win XP (2000): (untested)
    - LockCount is updated with +1 as documented in the article,
      so lock sequence goes like -1, 0, 1, 2 ... etc.
    - The event is obviously also auto-initialized in TryEnterCriticalSection. That is, 
      we hope so, otherwise the original implementation never worked correctly in the
      first place (just busy-waits, munching precious CPU time).
    - Data members are also correctly updated in TryEnterCriticalSection. Same
      hope / assumption here.
    We do an one-time test lock and draw conclusions based on the resulting value
    of LockCount: 0=XP behaviour, -2=Win 7 behaviour. Also crossing fingers helps.
*/
static LONG LockDelta	= -4; /* Win 7 default */

static inline int _InitWaitCriticalSection(RTL_CRITICAL_SECTION *prc)
{
    int r = 0;
    HANDLE evt;
    LONG LockCount = prc->LockCount;

    r = 0;
    if (!prc->OwningThread || !prc->RecursionCount || (LockCount & 1)) {
        /* not locked (anymore), caller should redo trylock sequence: */
        return EAGAIN;
    } else {
        _ReadWriteBarrier();
        if( LockCount != InterlockedCompareExchange(&prc->LockCount, LockCount+LockDelta, LockCount) ) {
            /* recheck here too: */
            return EAGAIN;
        }
    }

    if ( !prc->LockSemaphore) {
        if (!(evt =  CreateEvent(NULL,FALSE,FALSE,NULL)) ) {
            InterlockedExchangeAdd(&prc->LockCount, -LockDelta);
            return ENOMEM;
        }
        if(InterlockedCompareExchangePointer(&prc->LockSemaphore,evt,NULL)) {
            /* someone sneaked in between, keep the original: */
            CloseHandle(evt);
        }
    }

    return r;
}

/* the wait failed, so we have to restore the LockCount member */
static inline void _UndoWaitCriticalSection(RTL_CRITICAL_SECTION *prc)
{
        InterlockedExchangeAdd(&prc->LockCount, -LockDelta);
}

int pthread_mutex_timedlock(pthread_mutex_t *m, const struct timespec *ts)
{
    unsigned long long t, ct;
#ifndef USE_MUTEX_Mutex
    int i = 0;
#endif
    int r;

    if (!ts) return pthread_mutex_lock(m);
    r = mutex_ref(m);
    if (r) return r;

    /* Try to lock it without waiting */
    r=_mutex_trylock(m);
    if (r != EBUSY) return mutex_unref(m,r);
    
    mutex_t *_m = (mutex_t *)*m;
    if (_m->type != PTHREAD_MUTEX_NORMAL && COND_LOCKED(_m) && COND_OWNER(_m))
      return mutex_unref(m,EDEADLK);
    ct = _pthread_time_in_ms();
    t = _pthread_time_in_ms_from_timespec(ts);
#ifdef USE_MUTEX_Mutex
   mutex_unref(m,r);
   r = pthread_mutex_lock_intern(m, (ct > t ? 0 : (t - ct)));
#else
    mutex_unref(m,EDEADLK);
    while (1)
    {
        /* Have we waited long enough? A high count means we busy-waited probably.*/
        if (ct >= t) {
            printf("%d: Timeout after %d times\n",(int)GetCurrentThreadId(), i);
            return ETIMEDOUT;
        }
        if ((i & 1) == 0)
          Sleep(0);  /* waiter that owner gets released.  */
        /* Try to grab lock */
	r = pthread_mutex_trylock(m);
	if (r != EBUSY) break;
        /* Get current time */
        ct = _pthread_time_in_ms();
        i ++;
    }
#endif
    return  r;
}

int pthread_mutex_unlock(pthread_mutex_t *m)
{
    int r = mutex_ref_unlock(m);
    if(r) return r;

    mutex_t *_m = (mutex_t *)*m;
    if (_m->type == PTHREAD_MUTEX_NORMAL)
    {
        if (!COND_LOCKED(_m))
	  return mutex_unref(m,EPERM);
    }
    else if (!COND_LOCKED(_m) || !COND_OWNER(_m))
        return mutex_unref(m,EPERM);
    if (_m->type == PTHREAD_MUTEX_RECURSIVE)
    {
      if(InterlockedDecrement(&_m->count))
	return mutex_unref(m,0);
    }
#if defined USE_MUTEX_Mutex
    UNSET_OWNER(_m);
    if (_m->h != NULL && !ReleaseSemaphore(_m->h, 1, NULL)) {
    	SET_OWNER(_m);
        /* restore our own bookkeeping */
        return mutex_unref(m,EPERM);
    }
#else /* USE_MUTEX_CriticalSection */
    UNSET_OWNER(_m);
    LeaveCriticalSection(&_m->cs.cs);
#endif
    return mutex_unref(m,0);
}

static __attribute__((noinline)) int
_mutex_trylock(pthread_mutex_t *m)
{
    int r = 0;
    mutex_t *_m = (mutex_t *)*m;
    if (_m->type != PTHREAD_MUTEX_NORMAL)
    {
      if (COND_LOCKED(_m))
      {
	if (_m->type == PTHREAD_MUTEX_RECURSIVE && COND_OWNER(_m))
	{
	  InterlockedIncrement(&_m->count);
	  return 0;
	}
	return EBUSY;
      }
    } else if (COND_LOCKED(_m))
      return EBUSY;
#if defined USE_MUTEX_Mutex
    r = do_sema_b_wait_intern (_m->h, 1, 0);
    if (r == ETIMEDOUT) r = EBUSY;
#else /* USE_MUTEX_CriticalSection */
    r = TryEnterCriticalSection(&_m->cs.cs) ? 0 : EBUSY;
#endif
    if (!r)
    {
      _m->count = 1;
      SET_OWNER(_m);
    }
    return r;
}

int pthread_mutex_trylock(pthread_mutex_t *m)
{
    int r = mutex_ref(m);
    if(r) return r;

    return mutex_unref(m,_mutex_trylock(m));
}

static LONG InitOnce	= 1;
static void _mutex_init_once(mutex_t *m)
{
#if defined USE_MUTEX_CriticalSection
    LONG lc = 0;
    EnterCriticalSection(&m->cs.cs);
    lc = m->cs.rc.LockCount;
    LeaveCriticalSection(&m->cs.cs);
    switch (lc) {
    case 0: /* Win XP + 2k(?) */
        LockDelta = 1;
        break;
    case -2:  /* Win 7 + Vista(?) */
        LockDelta = -4;
        break;
    default:
        /* give up */
        assert(FALSE);
    }
#endif
}

int pthread_mutex_init(pthread_mutex_t *m, const pthread_mutexattr_t *a)
{
    mutex_t *_m;

    int r = mutex_ref_init(m);
    if(r) return r;

    if (!(_m = (pthread_mutex_t)calloc(1,sizeof(*_m))))
      return ENOMEM; 

    _m->type = PTHREAD_MUTEX_DEFAULT;
    _m->count = 0;

    if (a) {
        int share = PTHREAD_PROCESS_SHARED;
        r = pthread_mutexattr_gettype(a, &_m->type);
        if (!r) r = pthread_mutexattr_getpshared(a, &share);
        if (!r && share == PTHREAD_PROCESS_SHARED) r = ENOSYS;
    }
    if (!r) {
#if defined USE_MUTEX_Mutex
        if ( (_m->h = CreateSemaphore(NULL, 1, 0x7fffffff, NULL)) != NULL) {
#else /* USE_MUTEX_CriticalSection */
        if (InitializeCriticalSectionAndSpinCount(&_m->cs.cs, USE_MUTEX_CriticalSection_SpinCount)) {
#endif
        } else {
#if defined USE_MUTEX_Mutex
            switch (GetLastError()) {
            case ERROR_ACCESS_DENIED:
                    r = EPERM;
                    break;
            default: /* We assume this, to keep it simple: */
                    r = ENOMEM;
            }
#else /* USE_MUTEX_CriticalSection */
            r = ENOMEM;
#endif
        }
    } 
    if (r)
    {
        _m->valid = DEAD_MUTEX;
        free(_m);
        *m = NULL;
        return r;
    }
    if (InterlockedExchange(&InitOnce, 0))
	    _mutex_init_once(_m);
    _m->valid = LIFE_MUTEX;
    *m = _m;

    return 0;
}

int pthread_mutex_destroy(pthread_mutex_t *m)
{
    pthread_mutex_t mDestroy;
    int r = mutex_ref_destroy(m,&mDestroy);
    if(r) return r;
    if(!mDestroy) return 0; /* destroyed a (still) static initialized mutex */

    /* now the mutex is invalid, and no one can touch it */
    mutex_t *_m = (mutex_t *)mDestroy;


#if defined USE_MUTEX_Mutex
    CloseHandle(_m->h);
#else /* USE_MUTEX_CriticalSection */
    DeleteCriticalSection(&_m->cs.cs);
#endif
    _m->valid = DEAD_MUTEX;
    _m->type  = 0;
    _m->count = 0;
    free(mDestroy);
    return 0;
}

int pthread_mutexattr_init(pthread_mutexattr_t *a)
{
    *a = PTHREAD_MUTEX_NORMAL | (PTHREAD_PROCESS_PRIVATE << 3);
    return 0;
}

int pthread_mutexattr_destroy(pthread_mutexattr_t *a)
{
    if (!a)
      return EINVAL;
    return 0;
}

int pthread_mutexattr_gettype(const pthread_mutexattr_t *a, int *type)
{
    if (!a || !type)
      return EINVAL;
    *type = *a & 3;

    return 0;
}

int pthread_mutexattr_settype(pthread_mutexattr_t *a, int type)
{
    if (!a || (type != PTHREAD_MUTEX_NORMAL && type != PTHREAD_MUTEX_RECURSIVE && type != PTHREAD_MUTEX_ERRORCHECK))
      return EINVAL;
    *a &= ~3;
    *a |= type;

    return 0;
}

int pthread_mutexattr_getpshared(const pthread_mutexattr_t *a, int *type)
{
    if (!a || !type)
      return EINVAL;
    *type = (*a & 4 ? PTHREAD_PROCESS_SHARED : PTHREAD_PROCESS_PRIVATE);

    return 0;
}

int pthread_mutexattr_setpshared(pthread_mutexattr_t * a, int type)
{
    int r = 0;
    if (!a || (type != PTHREAD_PROCESS_SHARED && type != PTHREAD_PROCESS_PRIVATE))
      return EINVAL;
    if (type == PTHREAD_PROCESS_SHARED)
    {
      type = PTHREAD_PROCESS_PRIVATE;
      r = ENOSYS;
    }
    type = (type == PTHREAD_PROCESS_SHARED ? 4 : 0);

    *a &= ~4;
    *a |= type;

    return r;
}

int pthread_mutexattr_getprotocol(const pthread_mutexattr_t *a, int *type)
{
    *type = *a & (8 + 16);

    return 0;
}

int pthread_mutexattr_setprotocol(pthread_mutexattr_t *a, int type)
{
    if ((type & (8 + 16)) != 8 + 16) return EINVAL;

    *a &= ~(8 + 16);
    *a |= type;

    return 0;
}

int pthread_mutexattr_getprioceiling(const pthread_mutexattr_t *a, int * prio)
{
    *prio = *a / PTHREAD_PRIO_MULT;
    return 0;
}

int pthread_mutexattr_setprioceiling(pthread_mutexattr_t *a, int prio)
{
    *a &= (PTHREAD_PRIO_MULT - 1);
    *a += prio * PTHREAD_PRIO_MULT;

    return 0;
}
