#ifndef WIN_PTHREADS_SPINLOCK_H
#define WIN_PTHREADS_SPINLOCK_H

#define CHECK_SPINLOCK(l)  { \
    if (!(l) || !*l \
        || ( ((spin_t *)(*l))->valid != (unsigned int)LIFE_SPINLOCK ) ) \
        return EINVAL; }

#undef USE_SPINLOCK_DEADLK
#define CHECK_DEADLK_SL(l)
#define SET_OWNER_SL(l)	
#define SET_OWNER_SLIF(l,r)	
#define UNSET_OWNER_SL(l)

#ifdef USE_SPINLOCK_DBG
#define CHECK_SPINLOCK_LITE(l) if (!(l)) return EINVAL;
#define _spin_lite_lock_cnt(c)	c++
#define _spin_lite_lock_inc(c)	InterlockedIncrement(&c)
#define _spin_lite_lock_dec(c)	InterlockedDecrement(&c)
#define _spin_lite_lock_stat(c)	{scntMax = (c > scntMax) ? c : scntMax;	scnt += lscnt;}
#else
#define CHECK_SPINLOCK_LITE(l)
#define _spin_lite_lock_cnt(c)
#define _spin_lite_lock_inc(c)
#define _spin_lite_lock_stat(c)
#define _spin_lite_lock_dec(c)
#endif

#undef USE_SPINLOCK_EPERM
#define CHECK_PERM_SL(l)

#define INIT_SPINLOCK(s)  { int r; \
    if (PTHREAD_SPINLOCK_INITIALIZER == *s) { if ((r = spinlock_static_init(s))) return r; }}

typedef struct spin_t spin_t;
struct spin_t
{
    DWORD owner;
    unsigned int valid;   
    LONG l;   
};

#define LIFE_SPINLOCK 0xFEEDBAB1
#define DEAD_SPINLOCK 0xB00FDEAD

typedef union _vol_spinlock _vol_spinlock;
union _vol_spinlock {
    LONG *l;
    volatile LONG *lv;
};

int _spin_lite_trylock(spin_t *l);

int _spin_lite_unlock(spin_t *l);

int _spin_lite_lock(spin_t *l);

int _spin_lite_getsc(int reset);
int _spin_lite_getbsc(int reset);

int _spin_lite_getscMax(int reset);

#endif
