#include "util/atomic.hpp"
#include "util/spinlock.hpp"

void rbx_spinlock_lock(int* lock) {
  while(!atomic::compare_and_swap(lock, RBX_SPINLOCK_UNLOCKED, RBX_SPINLOCK_LOCKED));
}

void rbx_spinlock_unlock(int* lock) {
  *lock = RBX_SPINLOCK_UNLOCKED;
}
