#include "global_lock.hpp"

#include <sched.h>

namespace rubinius {
  void GlobalLock::yield() {
    // unlock..
    GlobalLock::UnlockGuard lock(*this);
    // if we reload right away, we'll likely get the lock again, 
    // which will starve other threads.
#ifdef linux
    // sched_yield doesn't seem to work here on linux...
    struct timespec ts = {0, 0};
    nanosleep(&ts, NULL);
#endif
    // and relock automatically!
  }
}
