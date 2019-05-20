#include <windows.h>
#include <stdio.h>
#include <signal.h>
#include "pthread.h"
#include "thread.h"
#include "misc.h"
#include "spinlock.h"

void (**_pthread_key_dest)(void *);

static volatile long _pthread_cancelling;
static int _pthread_concur;

/* FIXME Will default to zero as needed */
static pthread_once_t _pthread_tls_once;
static DWORD _pthread_tls = 0xffffffff;

static pthread_rwlock_t _pthread_key_lock = PTHREAD_RWLOCK_INITIALIZER;
static unsigned long _pthread_key_max=0L;
static unsigned long _pthread_key_sch=0L;

static _pthread_v *pthr_root = NULL, *pthr_last = NULL;
static spin_t spin_pthr_locked = {0,LIFE_SPINLOCK,0};

static void push_pthread_mem(_pthread_v *sv)
{
  int x;
  if (!sv || sv->next != NULL)
    return;
  x = sv->x + 1;
  memset (sv, 0, sizeof(struct _pthread_v));
  _spin_lite_lock(&spin_pthr_locked);
  if (pthr_last == NULL)
    pthr_root = pthr_last = sv;
  else
    pthr_last->next = sv;
  sv->hlp.x = sv->x = x;
  _spin_lite_unlock(&spin_pthr_locked);
}

static _pthread_v *pop_pthread_mem(void)
{
  _pthread_v *r = NULL;

  _spin_lite_lock(&spin_pthr_locked);
  if ((r = pthr_root) == NULL)
  {
    _spin_lite_unlock(&spin_pthr_locked);
    r = (_pthread_v *)calloc(1,sizeof(struct _pthread_v));
    if (r)
      r->hlp.p = r;
    return r;
  }
  if((pthr_root = r->next) == NULL)
    pthr_last = NULL;
  _spin_lite_unlock(&spin_pthr_locked);
  r->next = NULL;
  r->hlp.p = r;
  return r;
}

static void free_pthread_mem(void)
{
  _pthread_v *t;
  
  _spin_lite_lock(&spin_pthr_locked);
  t = pthr_root;
  pthr_root = pthr_last = NULL;
  _spin_lite_unlock(&spin_pthr_locked);
  while (t != NULL)
  {
    _pthread_v *sv = t;
    t = t->next;
    free (sv);
  }
}

static BOOL WINAPI
__dyn_tls_pthread (HANDLE hDllHandle, DWORD dwReason, LPVOID lpreserved)
{
  _pthread_v *t = NULL;
  if (dwReason == DLL_PROCESS_DETACH)
  {
    free_pthread_mem();
  }
  else if (dwReason == DLL_THREAD_DETACH)
  {
    if (_pthread_tls != 0xffffffff)
      t = (_pthread_v *)TlsGetValue(_pthread_tls);
    if (t && t->thread_noposix != 0)
    {
      _pthread_cleanup_dest(t->hlp);
      if (t->h != NULL)
      {
        CloseHandle(t->h);
	if (t->evStart)
	  CloseHandle(t->evStart);
	t->evStart = NULL;
        t->h = NULL;
      }
      pthread_mutex_destroy(&t->p_clock);
      push_pthread_mem(t);
      t = NULL;
      TlsSetValue(_pthread_tls, t);
    }
    else if (t && t->ended == 0)
    {
      if (t->evStart)
	CloseHandle(t->evStart);
      t->evStart = NULL;
      t->ended = 1;
      _pthread_cleanup_dest(t->hlp);
      if ((t->p_state & PTHREAD_CREATE_DETACHED) == PTHREAD_CREATE_DETACHED)
      {
	t->valid = DEAD_THREAD;
	if (t->h != NULL)
	  CloseHandle (t->h);
	t->h = NULL;
	push_pthread_mem(t);
	t = NULL;
	TlsSetValue(_pthread_tls, t);
      }
      pthread_mutex_destroy(&t->p_clock);
    }
    else if (t)
    {
      if (t->evStart)
	CloseHandle(t->evStart);
      t->evStart = NULL;
      pthread_mutex_destroy(&t->p_clock);
    }
  }
  return TRUE;
}

PIMAGE_TLS_CALLBACK __xl_f __attribute__ ((section (".CRT$XLF"))) = (PIMAGE_TLS_CALLBACK) __dyn_tls_pthread;

#ifdef WINPTHREAD_DBG
static int print_state = 0;
void thread_print_set(int state)
{
    print_state = state;
}

void thread_print(volatile pthread_t t, char *txt)
{
    if (!print_state) return;
    if (t.p == NULL) {
        printf("T%p %d %s\n",t.p,(int)GetCurrentThreadId(),txt);
    } else {
        printf("T%p %d V=%0X H=%p %s\n",
            t.p, 
            (int)GetCurrentThreadId(), 
            (int)t.p->valid, 
            t.p->h,
            txt
            );
    }
}
#endif

typedef struct collect_once_t {
  pthread_once_t *o;
  pthread_mutex_t m;
  int count;
  struct collect_once_t *next;
} collect_once_t;

static collect_once_t *once_obj = NULL;

static spin_t once_global = {0,LIFE_SPINLOCK,0};

static collect_once_t *enterOnceObject(pthread_once_t *o)
{
  collect_once_t *c, *p = NULL;
  _spin_lite_lock(&once_global);
  c = once_obj;
  while (c != NULL && c->o != o)
  {
    c = (p = c)->next;
  }
  if (!c)
  {
    c = (collect_once_t *) calloc(1,sizeof(collect_once_t));
    c->o = o;
    c->count = 1;
    if (!p) once_obj = c;
    else p->next = c;
    pthread_mutex_init(&c->m, NULL);
  }
  else
    c->count += 1;
  _spin_lite_unlock(&once_global);
  return c;
}

static void leaveOnceObject(collect_once_t *c)
{
  collect_once_t *h, *p = NULL;
  if (!c)
    return;
  _spin_lite_lock(&once_global);
  h = once_obj;
  while (h != NULL && c != h)
  {
    h = (p = h)->next;
  }
  if (h)
  {
    c->count -= 1;
    if (c->count == 0)
    {
      pthread_mutex_destroy(&c->m);
      if (!p) once_obj = c->next;
      else p->next = c->next;
      free (c);
    }
  }
  else fprintf(stderr, "%p not found?!?!\n", c);
  _spin_lite_unlock(&once_global);
}

static void _pthread_once_cleanup(void *o)
{
  collect_once_t *co = (collect_once_t *) o;
  pthread_mutex_unlock(&co->m);
  leaveOnceObject(co);
}

static int _pthread_once_raw(pthread_once_t *o, void (*func)(void))
{
  collect_once_t *co;
  long state = *o;

  CHECK_PTR(o);
  CHECK_PTR(func);

  if (state == 1)
    return 0;
  co = enterOnceObject(o);
  pthread_mutex_lock(&co->m);
  if (*o == 0)
  {
    func();
    *o = 1;
  } else if (*o != 1)
    fprintf (stderr," once %p is %d\n", o, (int) *o);
  pthread_mutex_unlock(&co->m);
  leaveOnceObject(co);

  /* Done */
  return 0;
}

void *pthread_timechange_handler_np(void * dummy)
{
    return NULL;
}

int pthread_delay_np (const struct timespec *interval)
{
  DWORD to = (!interval ? 0 : dwMilliSecs(_pthread_time_in_ms_from_timespec(interval)));
  pthread_t s = pthread_self();
  if(!to)
  {
    pthread_testcancel();
    Sleep(0);
    pthread_testcancel();
    return 0;
  }
  pthread_testcancel();
  if (s.p->evStart)
  {
    WaitForSingleObject(s.p->evStart, to);
  }
  else
  {
    Sleep(to);
  }
  pthread_testcancel();
  return 0;
}

int pthread_num_processors_np(void) 
{
    int r = 0; 
	DWORD_PTR ProcessAffinityMask, SystemAffinityMask;

	if (GetProcessAffinityMask(GetCurrentProcess(), &ProcessAffinityMask, &SystemAffinityMask)) {
	    for(; ProcessAffinityMask != 0; ProcessAffinityMask>>=1){
            r += ProcessAffinityMask&1;
        }
    }
	return r ? r : 1; /* assume at least 1 */
}

int pthread_set_num_processors_np(int n) 
{
    int r = 0; 
    n = n ? n : 1;  /* need at least 1 */
	DWORD_PTR ProcessAffinityMask, ProcessNewAffinityMask=0, SystemAffinityMask;

	if (GetProcessAffinityMask(GetCurrentProcess(), &ProcessAffinityMask, &SystemAffinityMask)) {
	    for (; ProcessAffinityMask != 0; ProcessAffinityMask>>=1){
           ProcessNewAffinityMask<<=1;
           if ( ProcessAffinityMask&1 ) {
               if (r < n) {
                   ProcessNewAffinityMask |= 1;
                   r ++;
               }
           }
        }
        SetProcessAffinityMask(GetCurrentProcess(),ProcessNewAffinityMask);
    }
	return r;
}

int pthread_once(pthread_once_t *o, void (*func)(void))
{
  collect_once_t *co;
  long state = *o;

  CHECK_PTR(o);
  CHECK_PTR(func);

  if (state == 1)
    return 0;
  co = enterOnceObject(o);
  pthread_mutex_lock(&co->m);
  if (*o == 0)
  {
    pthread_cleanup_push(_pthread_once_cleanup, co);
    func();
    pthread_cleanup_pop(0);
    *o = 1;
  } else if (*o != 1)
    fprintf (stderr," once %p is %d\n", o, (int) *o);
  pthread_mutex_unlock(&co->m);
  leaveOnceObject(co);

  /* Done */
  return 0;
}

int pthread_key_create(pthread_key_t *key, void (* dest)(void *))
{
    unsigned int i;
    long nmax;
    void (**d)(void *);

    if (!key) return EINVAL;

    pthread_rwlock_wrlock(&_pthread_key_lock);

    for (i = _pthread_key_sch; i < _pthread_key_max; i++)
    {
        if (!_pthread_key_dest[i])
        {
            *key = i;
            if (dest)
            {
                _pthread_key_dest[i] = dest;
            }
            else
            {
                _pthread_key_dest[i] = (void(*)(void *))1;
            }
            pthread_rwlock_unlock(&_pthread_key_lock);

            return 0;
        }
    }

    for (i = 0; i < _pthread_key_sch; i++)
    {
        if (!_pthread_key_dest[i])
        {
            *key = i;
            if (dest)
            {
                _pthread_key_dest[i] = dest;
            }
            else
            {
                _pthread_key_dest[i] = (void(*)(void *))1;
            }
            pthread_rwlock_unlock(&_pthread_key_lock);

            return 0;
        }
    }

    if (!_pthread_key_max) _pthread_key_max = 1;
    if (_pthread_key_max == PTHREAD_KEYS_MAX)
    {
        pthread_rwlock_unlock(&_pthread_key_lock);

        return ENOMEM;
    }

    nmax = _pthread_key_max * 2;
    if (nmax > PTHREAD_KEYS_MAX) nmax = PTHREAD_KEYS_MAX;

    /* No spare room anywhere */
    d = (void (__cdecl **)(void *))realloc(_pthread_key_dest, nmax * sizeof(*d));
    if (!d)
    {
        pthread_rwlock_unlock(&_pthread_key_lock);

        return ENOMEM;
    }

    /* Clear new region */
    memset((void *) &d[_pthread_key_max], 0, (nmax-_pthread_key_max)*sizeof(void *));

    /* Use new region */
    _pthread_key_dest = d;
    _pthread_key_sch = _pthread_key_max + 1;
    *key = _pthread_key_max;
    _pthread_key_max = nmax;

    if (dest)
    {
        _pthread_key_dest[*key] = dest;
    }
    else
    {
        _pthread_key_dest[*key] = (void(*)(void *))1;
    }

    pthread_rwlock_unlock(&_pthread_key_lock);

    return 0;
}

int pthread_key_delete(pthread_key_t key)
{
    if (key > _pthread_key_max) return EINVAL;
    if (!_pthread_key_dest) return EINVAL;

    pthread_rwlock_wrlock(&_pthread_key_lock);
    _pthread_key_dest[key] = NULL;

    /* Start next search from our location */
    if (_pthread_key_sch > key) _pthread_key_sch = key;

    pthread_rwlock_unlock(&_pthread_key_lock);

    return 0;
}

void *pthread_getspecific(pthread_key_t key)
{
    _pthread_v *t = pthread_self().p;

    if (key >= t->keymax) return NULL;

    return t->keyval[key];

}

int pthread_setspecific(pthread_key_t key, const void *value)
{
    _pthread_v *t = pthread_self().p;

    if (key > t->keymax)
    {
        int keymax = (key + 1) * 2;
        void **kv = (void **)realloc(t->keyval, keymax * sizeof(void *));

        if (!kv) return ENOMEM;

        /* Clear new region */
        memset(&kv[t->keymax], 0, (keymax - t->keymax)*sizeof(void*));

        t->keyval = kv;
        t->keymax = keymax;
    }

    t->keyval[key] = (void *) value;

    return 0;
}

int pthread_equal(pthread_t t1, pthread_t t2)
{
    return (t1.p == t2.p && t1.x == t2.x);
}

void pthread_tls_init(void)
{
    _pthread_tls = TlsAlloc();

    /* Cannot continue if out of indexes */
    if (_pthread_tls == TLS_OUT_OF_INDEXES) abort();
}

void _pthread_cleanup_dest(pthread_t t)
{
    _pthread_v *tv;
    unsigned int i, j;
    if (t.p == NULL)
      return;
    tv = t.p;

    for (j = 0; j < PTHREAD_DESTRUCTOR_ITERATIONS; j++)
    {
        int flag = 0;

        for (i = 0; i < tv->keymax; i++)
        {
            void *val = tv->keyval[i];

            if (val)
            {
                pthread_rwlock_rdlock(&_pthread_key_lock);
                if ((uintptr_t) _pthread_key_dest[i] > 1)
                {
                    /* Call destructor */
                    tv->keyval[i] = NULL;
                    _pthread_key_dest[i](val);
                    flag = 1;
                }
                pthread_rwlock_unlock(&_pthread_key_lock);
            }
        }

        /* Nothing to do? */
        if (!flag) return;
    }
}

pthread_t pthread_self(void)
{
    pthread_t ret;
    _pthread_v *t;
    ret.x = 0;
    ret.p = NULL;

    _pthread_once_raw(&_pthread_tls_once, pthread_tls_init);

    t = (_pthread_v *)TlsGetValue(_pthread_tls);

    /* Main thread? */
    if (!t)
    {
        t = (_pthread_v *) pop_pthread_mem();

        /* If cannot initialize main thread, then the only thing we can do is return null pthread_t */
        if (!__xl_f || !t) return ret;

        t->p_state = PTHREAD_DEFAULT_ATTR /*| PTHREAD_CREATE_DETACHED*/;
        t->tid = GetCurrentThreadId();
        t->evStart = CreateEvent (NULL, 1, 0, NULL);
        t->p_clock = PTHREAD_MUTEX_INITIALIZER;
        t->sched_pol = SCHED_OTHER;
        t->h = NULL; //GetCurrentThread();
	if (!DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &t->h, 0, FALSE, DUPLICATE_SAME_ACCESS))
	  abort ();
	t->sched.sched_priority = GetThreadPriority(t->h);
        t->ended = 0;
        t->thread_noposix = 1;

        /* Save for later */
        if (!TlsSetValue(_pthread_tls, t)) abort();

        if (setjmp(t->jb))
        {
	  unsigned rslt = 128;
	  /* Make sure we free ourselves if we are detached */
	  t = (_pthread_v *)TlsGetValue(_pthread_tls);
	  if (t)
	  {
	    if (!t->h) {
		t->valid = DEAD_THREAD;
		if (t->evStart)
		  CloseHandle(t->evStart);
		t->evStart = NULL;
		rslt = (unsigned) (size_t) t->ret_arg;
		push_pthread_mem(t);
		t = NULL;
		TlsSetValue(_pthread_tls, t);
	    } else
	    {
	      rslt = (unsigned) (size_t) t->ret_arg;
	      t->ended = 1;
	      if (t->evStart)
		CloseHandle(t->evStart);
	      t->evStart = NULL;
	      if ((t->p_state & PTHREAD_CREATE_DETACHED) == PTHREAD_CREATE_DETACHED)
	      {
		t->valid = DEAD_THREAD;
		CloseHandle (t->h);
		t->h = NULL;
		push_pthread_mem(t);
		t = NULL;
		TlsSetValue(_pthread_tls, t);
	      }
	    }
	  }
	  /* Time to die */
	  _endthreadex(rslt);
        }
    }
    if (!t)
      return ret;
    ret = t->hlp;

    return ret;
}

int pthread_get_concurrency(int *val)
{
    *val = _pthread_concur;
    return 0;
}

int pthread_set_concurrency(int val)
{
    _pthread_concur = val;
    return 0;
}

int pthread_exit(void *res)
{
    pthread_t t = pthread_self();
    if (t.p == NULL)
      return EINVAL;

    t.p->ret_arg = res;

    _pthread_cleanup_dest(t);

    longjmp(t.p->jb, 1);
}

void _pthread_invoke_cancel(void)
{
  pthread_t se = pthread_self();
  se.p->in_cancel = 1;
  _pthread_cleanup *pcup;
  _pthread_setnobreak (1);
  InterlockedDecrement(&_pthread_cancelling);

  /* Call cancel queue */
  for (pcup = pthread_self().p->clean; pcup; pcup = pcup->next)
  {
      pcup->func((pthread_once_t *)pcup->arg);
  }

  _pthread_setnobreak (0);
  pthread_exit(PTHREAD_CANCELED);
}

int  __pthread_shallcancel(void)
{
    pthread_t t;
    if (!_pthread_cancelling)
      return 0;
    t = pthread_self();
    if (t.p == NULL)
      return 0;
    if (t.p->nobreak <= 0 && t.p->cancelled && (t.p->p_state & PTHREAD_CANCEL_ENABLE))
      return 1;
    return 0;
}

void _pthread_setnobreak(int v)
{
  pthread_t t = pthread_self();
  if (t.p == NULL)
    return;
  if (v > 0) InterlockedIncrement ((long*)&t.p->nobreak);
  else InterlockedDecrement((long*)&t.p->nobreak);
}

void pthread_testcancel(void)
{
  pthread_t self = pthread_self();
  if (!self.p)
    return;
  if (!_pthread_cancelling)
    return;
  pthread_mutex_lock(&self.p->p_clock);
  if (self.p->cancelled && (self.p->p_state & PTHREAD_CANCEL_ENABLE) && self.p->nobreak <= 0)
  {
    self.p->p_state &= ~PTHREAD_CANCEL_ENABLE;
    ResetEvent(self.p->evStart);
    pthread_mutex_unlock(&self.p->p_clock);
    _pthread_invoke_cancel();
  }
  pthread_mutex_unlock(&self.p->p_clock);
}


int pthread_cancel(pthread_t t)
{
    struct _pthread_v *tv = t.p;

    if (t.p == NULL) return ESRCH;
    CHECK_OBJECT(tv, ESRCH);
    /*if (tv->ended) return ESRCH;*/
    pthread_mutex_lock(&t.p->p_clock);
    if (pthread_equal(pthread_self(), t))
    {
      if(tv->cancelled) return (tv->in_cancel ? ESRCH : 0);
      tv->cancelled = 1;
      InterlockedIncrement(&_pthread_cancelling);
      if(tv->evStart) SetEvent(tv->evStart);
      if ((tv->p_state & PTHREAD_CANCEL_ASYNCHRONOUS) != 0 && (tv->p_state & PTHREAD_CANCEL_ENABLE) != 0)
      {
        tv->p_state &= ~PTHREAD_CANCEL_ENABLE;
        tv->in_cancel = 1;
        pthread_mutex_unlock(&t.p->p_clock);
        _pthread_invoke_cancel();
      }
      pthread_mutex_unlock(&t.p->p_clock);
      return 0;
    }

    if ((tv->p_state & PTHREAD_CANCEL_ASYNCHRONOUS) != 0 && (tv->p_state & PTHREAD_CANCEL_ENABLE) != 0)
    {
        /* Dangerous asynchronous cancelling */
        CONTEXT ctxt;

        /* Already done? */
        if(tv->cancelled || tv->in_cancel) return ESRCH;

        ctxt.ContextFlags = CONTEXT_CONTROL;

        SuspendThread(tv->h);
        if (WaitForSingleObject(tv->h, 0) == WAIT_TIMEOUT)
        {
	  GetThreadContext(tv->h, &ctxt);
#ifdef _M_X64
	  ctxt.Rip = (uintptr_t) _pthread_invoke_cancel;
#else
	  ctxt.Eip = (uintptr_t) _pthread_invoke_cancel;
#endif
	  SetThreadContext(tv->h, &ctxt);

	  /* Also try deferred Cancelling */
	  tv->cancelled = 1;
          tv->p_state &= ~PTHREAD_CANCEL_ENABLE;
          tv->in_cancel = 1;

	  /* Notify everyone to look */
	  InterlockedIncrement(&_pthread_cancelling);
	  if(tv->evStart) SetEvent(tv->evStart);
	  pthread_mutex_unlock(&t.p->p_clock);

	  ResumeThread(tv->h);
        }
    }
    else
    {
      if (tv->cancelled == 0)
      {
        /* Safe deferred Cancelling */
        tv->cancelled = 1;

        /* Notify everyone to look */
        InterlockedIncrement(&_pthread_cancelling);
        if(tv->evStart) SetEvent(tv->evStart);
      }
      else
      {
	pthread_mutex_unlock(&t.p->p_clock);
	return (tv->in_cancel ? ESRCH : 0);
      }
    }
    pthread_mutex_unlock(&t.p->p_clock);
    return 0;
}

/* half-stubbed version as we don't really well support signals */
int pthread_kill(pthread_t t, int sig)
{
    struct _pthread_v *tv = t.p;

    if (!tv) return ESRCH;
    CHECK_OBJECT(tv, ESRCH);
    if (tv->ended) return ESRCH;
    if (!sig) return 0;
    if (sig < SIGINT || sig > NSIG) return EINVAL;
    return pthread_cancel(t);
}

unsigned _pthread_get_state(const pthread_attr_t *attr, unsigned flag)
{
    return (attr->p_state & flag);
}

int _pthread_set_state(pthread_attr_t *attr, unsigned flag, unsigned val)
{
    if (~flag & val) return EINVAL;
    attr->p_state &= ~flag;
    attr->p_state |= val;

    return 0;
}

int pthread_attr_init(pthread_attr_t *attr)
{
  memset (attr, 0, sizeof (pthread_attr_t));
  attr->p_state = PTHREAD_DEFAULT_ATTR;
  attr->stack = NULL;
  attr->s_size = 0;
  return 0;
}

int pthread_attr_destroy(pthread_attr_t *attr)
{
    /* No need to do anything */
    memset (attr, 0, sizeof(pthread_attr_t));
    return 0;
}

int pthread_attr_setdetachstate(pthread_attr_t *a, int flag)
{
    return _pthread_set_state(a, PTHREAD_CREATE_DETACHED, flag);
}

int pthread_attr_getdetachstate(const pthread_attr_t *a, int *flag)
{
    *flag = _pthread_get_state(a, PTHREAD_CREATE_DETACHED);
    return 0;
}

int pthread_attr_setinheritsched(pthread_attr_t *a, int flag)
{
  if (!a || (flag != PTHREAD_INHERIT_SCHED && flag != PTHREAD_EXPLICIT_SCHED))
    return EINVAL;
  return _pthread_set_state(a, PTHREAD_INHERIT_SCHED, flag);
}

int pthread_attr_getinheritsched(const pthread_attr_t *a, int *flag)
{
    *flag = _pthread_get_state(a, PTHREAD_INHERIT_SCHED);
    return 0;
}

int pthread_attr_setscope(pthread_attr_t *a, int flag)
{
    return _pthread_set_state(a, PTHREAD_SCOPE_SYSTEM, flag);
}

int pthread_attr_getscope(const pthread_attr_t *a, int *flag)
{
    *flag = _pthread_get_state(a, PTHREAD_SCOPE_SYSTEM);
    return 0;
}

int pthread_attr_getstackaddr(pthread_attr_t *attr, void **stack)
{
    *stack = attr->stack;
    return 0;
}

int pthread_attr_setstackaddr(pthread_attr_t *attr, void *stack)
{
    attr->stack = stack;
    return 0;
}

int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *size)
{
    *size = attr->s_size;
    return 0;
}

int pthread_attr_setstacksize(pthread_attr_t *attr, size_t size)
{
    attr->s_size = size;
    return 0;
}

static void test_cancel_locked (pthread_t t)
{
  if (t.p->in_cancel || t.p->ended != 0 || (t.p->p_state & PTHREAD_CANCEL_ENABLE) == 0)
    return;
  if ((t.p->p_state & PTHREAD_CANCEL_ASYNCHRONOUS) == 0)
    return;
  if (WaitForSingleObject(t.p->evStart, 0) != WAIT_OBJECT_0)
    return;
  pthread_mutex_unlock(&t.p->p_clock);
  _pthread_invoke_cancel();
}

int pthread_setcancelstate(int state, int *oldstate)
{
    pthread_t t = pthread_self();
    if (t.p == NULL) return EINVAL;

    if ((state & PTHREAD_CANCEL_ENABLE) != state) return EINVAL;
    pthread_mutex_lock (&t.p->p_clock);
    if (oldstate) *oldstate = t.p->p_state & PTHREAD_CANCEL_ENABLE;
    t.p->p_state &= ~PTHREAD_CANCEL_ENABLE;
    t.p->p_state |= state;
    test_cancel_locked(t);
    pthread_mutex_unlock(&t.p->p_clock);

    return 0;
}

int pthread_setcanceltype(int type, int *oldtype)
{
    pthread_t t = pthread_self();

    if (t.p == NULL)
      return EINVAL;
    if ((type & PTHREAD_CANCEL_ASYNCHRONOUS) != type) return EINVAL;
    pthread_mutex_lock(&t.p->p_clock);
    if (oldtype) *oldtype = t.p->p_state & PTHREAD_CANCEL_ASYNCHRONOUS;
    t.p->p_state &= ~PTHREAD_CANCEL_ASYNCHRONOUS;
    t.p->p_state |= type;
    test_cancel_locked(t);
    pthread_mutex_unlock(&t.p->p_clock);

    return 0;
}

int pthread_create_wrapper(void *args)
{
    unsigned rslt = 0;
    struct _pthread_v *tv = (struct _pthread_v *)args;

    pthread_mutex_lock(&tv->p_clock);
    _pthread_once_raw(&_pthread_tls_once, pthread_tls_init);
    TlsSetValue(_pthread_tls, tv);
    tv->tid = GetCurrentThreadId();
    pthread_mutex_unlock(&tv->p_clock);


    if (!setjmp(tv->jb))
    {
      intptr_t trslt = (intptr_t) 128;
      /* Provide to this thread a default exception handler.  */
      #ifdef __SEH__
	asm ("\t.tl_start:\n"
	  "\t.seh_handler __C_specific_handler, @except\n"
	  "\t.seh_handlerdata\n"
	  "\t.long 1\n"
	  "\t.rva .tl_start, .tl_end, _gnu_exception_handler ,.tl_end\n"
	  "\t.text"
	  );
      #endif      /* Call function and save return value */
      trslt = (intptr_t) tv->func(tv->ret_arg);
      #ifdef __SEH__
	asm ("\tnop\n\t.tl_end: nop\n");
      #endif
      tv->ret_arg = (void*) trslt;
      /* Clean up destructors */
      _pthread_cleanup_dest(tv->hlp);
    }
    pthread_mutex_lock(&tv->p_clock);
    rslt = (unsigned) (size_t) tv->ret_arg;
    /* Make sure we free ourselves if we are detached */
    if (tv->evStart)
      CloseHandle(tv->evStart);
    tv->evStart = NULL;
    if (!tv->h) {
        tv->valid = DEAD_THREAD;
        pthread_mutex_unlock(&tv->p_clock);
        pthread_mutex_destroy(&tv->p_clock);
        push_pthread_mem(tv);
        tv = NULL;
        TlsSetValue(_pthread_tls, tv);
    }
    else
    {
      tv->ended = 1;
      pthread_mutex_unlock(&tv->p_clock);
      pthread_mutex_destroy(&tv->p_clock);
    }

    _endthreadex(rslt);
    return rslt;
}

int pthread_create(pthread_t *th, const pthread_attr_t *attr, void *(* func)(void *), void *arg)
{
    HANDLE thrd = NULL;
    struct _pthread_v *tv;
    size_t ssize = 0;

    tv = pop_pthread_mem();

    if (!tv) return EAGAIN;

    if (th)
    	*th = tv->hlp;

    /* Save data in pthread_t */
    tv->ended = 0;
    tv->ret_arg = arg;
    tv->func = func;
    tv->p_state = PTHREAD_DEFAULT_ATTR;
    tv->h = INVALID_HANDLE_VALUE;
    tv->evStart = CreateEvent (NULL, 1, 0, NULL);
    tv->p_clock = PTHREAD_MUTEX_INITIALIZER;
    //tv->tmpEv = CreateEvent (NULL, 1, 0, NULL);
    tv->valid = LIFE_THREAD;
    tv->sched.sched_priority = THREAD_PRIORITY_NORMAL;
    tv->sched_pol = SCHED_OTHER;
    if (tv->evStart == NULL)
    {
      if (tv->evStart)
        CloseHandle(tv->evStart);
      if (th) memset(th,0, sizeof(pthread_t));
      push_pthread_mem(tv);
      return EAGAIN;
    }
 
    if (attr)
    {
        int inh = 0;
        tv->p_state = attr->p_state;
        ssize = attr->s_size;
        pthread_attr_getinheritsched (attr, &inh);
        if (inh)
        {
          tv->sched.sched_priority = pthread_self().p->sched.sched_priority;
	}
	else
	  tv->sched.sched_priority = attr->param.sched_priority;
    }

    /* Make sure tv->h has value of INVALID_HANDLE_VALUE */
    _ReadWriteBarrier();

    thrd = (HANDLE) _beginthreadex(NULL, ssize, (unsigned int (__stdcall *)(void *))pthread_create_wrapper, tv, 0x4/*CREATE_SUSPEND*/, NULL);
    if (thrd == INVALID_HANDLE_VALUE)
      thrd = 0;
    /* Failed */
    if (!thrd)
    {
      if (tv->evStart)
        CloseHandle(tv->evStart);
      pthread_mutex_destroy(&tv->p_clock);
      tv->evStart = NULL;
      if (th) memset(th,0, sizeof(pthread_t));
      push_pthread_mem(tv);
      return EAGAIN;
    }
    {
      int pr = tv->sched.sched_priority;
      if (pr <= THREAD_PRIORITY_IDLE) {
	  pr = THREAD_PRIORITY_IDLE;
      } else if (pr <= THREAD_PRIORITY_LOWEST) {
	  pr = THREAD_PRIORITY_LOWEST;
      } else if (pr >= THREAD_PRIORITY_TIME_CRITICAL) {
	  pr = THREAD_PRIORITY_TIME_CRITICAL;
      } else if (pr >= THREAD_PRIORITY_HIGHEST) {
	  pr = THREAD_PRIORITY_HIGHEST;
      }
      SetThreadPriority(thrd, pr);
    }
    ResetEvent(tv->evStart);
    if (tv->p_state & PTHREAD_CREATE_DETACHED)
    {
      tv->h = 0;
      ResumeThread(thrd);
      CloseHandle(thrd);
    }
    else
    {
      tv->h = thrd;
      ResumeThread(thrd);
    }
    return 0;
}

int pthread_join(pthread_t t, void **res)
{
    DWORD dwFlags;
    struct _pthread_v *tv = t.p;
    if (!tv || tv->h == NULL || !GetHandleInformation(tv->h, &dwFlags))
      return ESRCH;
    if ((tv->p_state & PTHREAD_CREATE_DETACHED) != 0)
      return EINVAL;
    if (pthread_equal(pthread_self(), t)) return EDEADLK;

    pthread_testcancel();

    if (tv->ended == 0)
    WaitForSingleObject(tv->h, INFINITE);
    CloseHandle(tv->h);
    if (tv->evStart)
      CloseHandle(tv->evStart);
    tv->evStart = NULL;
    /* Obtain return value */
    if (res) *res = tv->ret_arg;
    pthread_mutex_destroy(&tv->p_clock);
    push_pthread_mem(tv);

    return 0;
}

int _pthread_tryjoin(pthread_t t, void **res)
{
    DWORD dwFlags;
    struct _pthread_v *tv = t.p;
 
    if (!tv || tv->h == NULL || !GetHandleInformation(tv->h, &dwFlags))
      return ESRCH;

    if ((tv->p_state & PTHREAD_CREATE_DETACHED) != 0)
      return EINVAL;
    if (pthread_equal(pthread_self(), t)) return EDEADLK;

    pthread_testcancel();

    if(tv->ended == 0 && WaitForSingleObject(tv->h, 0))
    {
      if (tv->ended == 0);
        return EBUSY;
    }
    CloseHandle(tv->h);
    if (tv->evStart)
      CloseHandle(tv->evStart);
    tv->evStart = NULL;

    /* Obtain return value */
    if (res) *res = tv->ret_arg;
    pthread_mutex_destroy(&tv->p_clock);

    push_pthread_mem(tv);

    return 0;
}

int pthread_detach(pthread_t t)
{
    int r = 0;
    DWORD dwFlags;
    struct _pthread_v *tv = t.p;
    HANDLE dw;

    /*
    * This can't race with thread exit because
    * our call would be undefined if called on a dead thread.
    */

    if (!tv || tv->h == NULL || !GetHandleInformation(tv->h, &dwFlags))
      return ESRCH;
    if ((tv->p_state & PTHREAD_CREATE_DETACHED) != 0)
    {
      return EINVAL;
    }
    //if (tv->ended) r = ESRCH;
    dw = tv->h;
    tv->h = 0;
    tv->p_state |= PTHREAD_CREATE_DETACHED;
    _ReadWriteBarrier();
    if (dw)
    {
      CloseHandle(dw);
      if (tv->ended)
      {
	if (tv->evStart)
	  CloseHandle(tv->evStart);
	tv->evStart = NULL;
	pthread_mutex_destroy(&tv->p_clock);
        push_pthread_mem(tv);
      }
    }

    return r;
}
