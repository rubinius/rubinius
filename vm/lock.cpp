#include "vm.hpp"
#include "lock.hpp"

namespace rubinius {
  LockableScopedLock::LockableScopedLock(STATE, Lockable* lock,
                       const char* file, int line)
      : thread_(state->vm())
      , lock_(lock)
      , locked_(false)
      , recursive_(false)
    {
      if(lock_->mutex().locking_thread() == thread_) {
        recursive_ = true;
        // Don't relock, already got it locked.
      } else {
        lock_->lock(thread_, file, line);
        locked_ = true;
      }
    }


}
