#include "util/thread.hpp"
#include "gc/managed.hpp"

namespace rubinius {
  thread::ThreadData<ManagedThread*> _current_thread;

  ManagedThread* ManagedThread::current() {
    return _current_thread.get();
  }

  void ManagedThread::set_current(ManagedThread* th) {
    th->os_thread_ = pthread_self();
    _current_thread.set(th);
  }
}
