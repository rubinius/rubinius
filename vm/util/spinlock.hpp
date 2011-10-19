#ifndef RBX_SPINLOCK_H
#define RBX_SPINLOCK_H

#ifdef __cplusplus
extern "C" {
#endif

#define RBX_SPINLOCK_LOCKED   0
#define RBX_SPINLOCK_UNLOCKED 1
#define RBX_SPINLOCK_INIT     RBX_SPINLOCK_UNLOCKED

  void rbx_spinlock_lock(int*);
  void rbx_spinlock_unlock(int*);

#ifdef __cplusplus
}
#endif

#endif
