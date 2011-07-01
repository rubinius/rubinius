#include <windows.h>
#include <stdio.h>
#include "pthread.h"
#include "spinlock.h"
#include "misc.h"
      
static int scnt = 0;
static LONG bscnt = 0;
static int scntMax = 0;

static spin_t spin_locked = {0,LIFE_SPINLOCK,0};


static inline int
spinlock_static_init (pthread_spinlock_t *l)
{
  int ret;
  _spin_lite_lock(&spin_locked);
  if (PTHREAD_SPINLOCK_INITIALIZER != *l)
  {
    /* Check that somebody called destroy already.  Otherwise assume someone crept in between.  */
    ret = (*l == NULL ? EINVAL : 0);
  }
  else
  {
    ret = pthread_spin_init(l, PTHREAD_PROCESS_PRIVATE);
  }
  _spin_lite_unlock(&spin_locked);
  return ret;
}

int pthread_spin_init(pthread_spinlock_t *l, int pshared)
{
    spin_t *_l;
    
    if (!l) return EINVAL; 
    if (pshared == PTHREAD_PROCESS_SHARED)
      return ENOSYS;
    if (pshared != PTHREAD_PROCESS_SHARED && pshared != PTHREAD_PROCESS_PRIVATE) return EINVAL;
    if (!(_l = (pthread_spinlock_t)calloc(1, sizeof(*_l))))
        return ENOMEM;

    _l->valid = LIFE_SPINLOCK;
    *l = _l;
    return 0;
}

int pthread_spin_destroy(pthread_spinlock_t *l)
{
  spin_t *_l;
  if (!l || !*l) return EINVAL;
  _spin_lite_lock(&spin_locked);
  if (*l == PTHREAD_SPINLOCK_INITIALIZER)
  {
    *l = NULL;
    _spin_lite_unlock(&spin_locked);
    return 0;
  }
  _spin_lite_unlock(&spin_locked);
  _l = (spin_t *)*l;
  if (((spin_t *)(*l))->valid != (unsigned int)LIFE_SPINLOCK)
    return EINVAL;
  
  if (_l->l != 0)
    return EBUSY;
  *l= NULL; /* dereference first, free later */
  _l->valid  = DEAD_SPINLOCK;
  _l->l = 0;
  free(_l);
  return 0;
}

/* No-fair spinlock due to lack of knowledge of thread number.  */
int pthread_spin_lock(pthread_spinlock_t *l)
{
  spin_t *_l;

  if (!l || *l == NULL)
    return EINVAL;
  if (PTHREAD_SPINLOCK_INITIALIZER == *l)
  {
    int r = spinlock_static_init(l);
    if (r != 0)
      return r;
  }
  _l = (spin_t *)*l;
  while (InterlockedExchange((long*)&_l->l, EBUSY) == EBUSY)
  {
    YieldProcessor();
  }
  return 0;
}

int pthread_spin_trylock(pthread_spinlock_t *l)
{
  spin_t *_l;
  int r = 0;
  if (!l || *l == NULL)
    return EINVAL;
  if (PTHREAD_SPINLOCK_INITIALIZER == *l)
  {
    r = spinlock_static_init(l);
    if (r != 0)
      return r;
  }

  _l = (spin_t *)*l;

  r = InterlockedExchange(&_l->l, EBUSY);
  return r;
}

int _spin_lite_getsc(int reset)
{
    int r = scnt;
    if (reset) scnt = 0;
    return r;
}

int _spin_lite_getbsc(int reset)
{
    int r = bscnt;
    if (reset) bscnt = 0;
    return r;
}

int _spin_lite_getscMax(int reset)
{
    int r = scntMax;
    if (reset) scntMax = 0;
    return r;
}

int _spin_lite_trylock(spin_t *l)
{
    CHECK_SPINLOCK_LITE(l);
    return InterlockedExchange(&l->l, EBUSY);
}
int _spin_lite_unlock(spin_t *l)
{
    CHECK_SPINLOCK_LITE(l);
    /* Compiler barrier.  The store below acts with release symmantics.  */
    _ReadWriteBarrier();
    l->l = 0;

    return 0;
}
int _spin_lite_lock(spin_t *l)
{
    CHECK_SPINLOCK_LITE(l);
    int lscnt = 0;

    _vol_spinlock v;
    v.l = (LONG *)&l->l;
    _spin_lite_lock_inc(bscnt);
    while (InterlockedExchange(v.lv, EBUSY))
    {
        _spin_lite_lock_cnt(lscnt);
        /* Don't lock the bus whilst waiting */
        while (*v.lv)
        {
            _spin_lite_lock_cnt(lscnt);
            YieldProcessor();

            /* Compiler barrier.  Prevent caching of *l */
            _ReadWriteBarrier();
        }
    }
    _spin_lite_lock_dec(bscnt);
    _spin_lite_lock_stat(lscnt);
    return 0;
}


int
pthread_spin_unlock (pthread_spinlock_t *l)
{
  spin_t *_l;
  int r;
  if (!l || *l == NULL)
    return EINVAL;
  if (*l == PTHREAD_SPINLOCK_INITIALIZER)
    return EPERM;
  _l = (spin_t *)*l;

  /* Compiler barrier.  The store below acts with release symmantics.  */
  r = InterlockedExchange((long*)&_l->l, 0);
  return (!r ? EPERM : 0);
}
