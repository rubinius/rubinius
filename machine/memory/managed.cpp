#include "vm.hpp"
#include "state.hpp"

#include "util/thread.hpp"
#include "memory/managed.hpp"
#include "shared_state.hpp"
#include "metrics.hpp"

#include <thread>
#include <sstream>

namespace rubinius {
namespace memory {
  utilities::thread::ThreadData<ManagedThread*> _current_thread;

  ManagedThread::ManagedThread(uint32_t id, SharedState& ss,
      ManagedThread::Kind kind, const char* name)
    : shared_(ss)
    , kind_(kind)
    , metrics_()
    , os_thread_(0)
    , id_(id)
  {
    if(name) {
      name_ = std::string(name);
    } else {
      std::ostringstream thread_name;
      thread_name << "ruby." << id_;
      name_ = thread_name.str();
    }
  }

  ManagedThread::~ManagedThread() {
    if(metrics::Metrics* metrics = shared_.metrics()) {
      metrics->add_historical_metrics(metrics_);
    }
  }

  void ManagedThread::set_name(STATE, const char* name) {
    if(pthread_self() == os_thread_) {
      utilities::thread::Thread::set_os_name(name);
    }
    name_.assign(name);
  }

  ManagedThread* ManagedThread::current() {
    return _current_thread.get();
  }

  void ManagedThread::set_current_thread(ManagedThread* th) {
    utilities::thread::Thread::set_os_name(th->name().c_str());
    th->os_thread_ = pthread_self();
    _current_thread.set(th);
  }
}
}
