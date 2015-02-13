#include "vm.hpp"

#include "util/thread.hpp"
#include "gc/managed.hpp"
#include "shared_state.hpp"
#include "metrics.hpp"

namespace rubinius {
  utilities::thread::ThreadData<ManagedThread*> _current_thread;

  ManagedThread::ManagedThread(uint32_t id, SharedState& ss, ManagedThread::Kind kind)
    : shared_(ss)
    , name_(kind == eRuby ? "<ruby>" : "<system>")
    , run_state_(eIndependent)
    , kind_(kind)
    , id_(id)
  {
    metrics_.init(metrics::eNone);
  }

  ManagedThread::~ManagedThread() {
    if(metrics::Metrics* metrics = shared_.metrics()) {
      metrics->add_historical_metrics(metrics_);
    }
  }

  ManagedThread* ManagedThread::current() {
    return _current_thread.get();
  }

  void ManagedThread::set_current(ManagedThread* th, std::string name) {
    th->os_thread_ = pthread_self();
    th->set_name(name);
    _current_thread.set(th);
  }
}
