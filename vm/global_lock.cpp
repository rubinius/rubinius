#include "global_lock.hpp"
#include "prelude.hpp"
#include "vm.hpp"

#include "builtin/nativemethod.hpp"

#include <sched.h>

namespace rubinius {
  void GlobalLock::yield(STATE, CallFrame* cf) {
    // unlock..
    GlobalLock::UnlockGuard lock(state, cf);
    // if we reload right away, we'll likely get the lock again, 
    // which will starve other threads.
#ifdef linux
    // sched_yield doesn't seem to work here on linux...
    struct timespec ts = {0, 0};
    nanosleep(&ts, NULL);
#endif
    // and relock automatically!
  }

  GlobalLock::UnlockGuard::UnlockGuard(VM* state, CallFrame* call_frame)
    : thread::LockGuardTemplate<GlobalLock>(state->global_lock(), true)
  {
    state->set_call_frame(call_frame);
    this->unlock();
    if(debug_locking) std::cout << "[ Unlocked GIL ] " << pthread_self() << "\n";
  }

  GlobalLock::UnlockGuard::UnlockGuard(NativeMethodEnvironment* nme)
    : thread::LockGuardTemplate<GlobalLock>(nme->state()->global_lock(), true)
  {
    nme->state()->set_call_frame(nme->current_call_frame());
    this->unlock();
    if(debug_locking) std::cout << "[ Unlocked GIL ] " << pthread_self() << "\n";
  }
}
