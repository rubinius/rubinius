#include "vm/vm.hpp"

#include "util/thread.hpp"
#include "gc/managed.hpp"
#include "shared_state.hpp"

namespace rubinius {
  thread::ThreadData<ManagedThread*> _current_thread;

  ManagedThread::ManagedThread(uint32_t id, SharedState& ss, ManagedThread::Kind kind)
    : shared_(ss)
    , kind_(kind)
    , name_(kind == eRuby ? "<ruby>" : "<system>")
    , run_state_(eIndependent)
    , id_(id)
  {}

  ManagedThread* ManagedThread::current() {
    return _current_thread.get();
  }

  void ManagedThread::set_current(ManagedThread* th) {
    th->os_thread_ = pthread_self();
    _current_thread.set(th);
  }
}
