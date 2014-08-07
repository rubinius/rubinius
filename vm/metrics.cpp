#include "vm.hpp"
#include "metrics.hpp"

#include "object_utils.hpp"

#include "builtin/class.hpp"
#include "builtin/thread.hpp"

#include "gc/managed.hpp"

#include "dtrace/dtrace.h"

#include "util/logger.hpp"

// kevent
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

namespace rubinius {
  namespace metrics {
    Object* metrics_trampoline(STATE) {
      state->shared().metrics()->process_metrics(state);
      GCTokenImpl gct;
      state->gc_dependent(gct, 0);
      return cNil;
    }

    Metrics::Metrics(STATE)
      : AuxiliaryThread()
      , shared_(state->shared())
      , vm_(NULL)
      , thread_exit_(false)
      , thread_(state)
    {
      shared_.auxiliary_threads()->register_thread(this);
    }

    Metrics::~Metrics() {
      shared_.auxiliary_threads()->unregister_thread(this);
    }

    void Metrics::start(STATE) {
      start_thread(state);
    }

    void Metrics::wakeup() {
      thread_exit_ = true;

      close(kq_);
    }

    void Metrics::cleanup() {
    }

    void Metrics::start_thread(STATE) {
      SYNC(state);

      if(!vm_) {
        vm_ = state->shared().new_vm();
        thread_exit_ = false;
        thread_.set(Thread::create(state, vm_, G(thread), metrics_trampoline, true));
      }

      if(thread_.get()->fork_attached(state)) {
        rubinius::bug("Unable to start metrics thread");
      }
    }

    void Metrics::stop_thread(STATE) {
      SYNC(state);

      if(vm_) {
        wakeup();

        pthread_t os = vm_->os_thread();
        thread_exit_ = true;

        void* return_value;
        pthread_join(os, &return_value);

        vm_ = NULL;
      }
    }

    void Metrics::shutdown(STATE) {
      stop_thread(state);
      cleanup();
    }

    void Metrics::before_exec(STATE) {
      stop_thread(state);
    }

    void Metrics::after_exec(STATE) {
      start_thread(state);
    }

    void Metrics::before_fork(STATE) {
      stop_thread(state);
    }

    void Metrics::after_fork_parent(STATE) {
      start_thread(state);
    }

    void Metrics::after_fork_child(STATE) {
      cleanup();
      start(state);
    }

    void Metrics::process_metrics(STATE) {
      GCTokenImpl gct;
      RBX_DTRACE_CONST char* thread_name =
        const_cast<RBX_DTRACE_CONST char*>("rbx.metrics");
      vm_->set_name(thread_name);

      RUBINIUS_THREAD_START(const_cast<RBX_DTRACE_CONST char*>(thread_name),
                            state->vm()->thread_id(), 1);

      state->vm()->thread->hard_unlock(state, gct, 0);
      state->gc_independent(gct, 0);

      kq_ = kqueue();
      struct kevent filter;
      struct kevent event;
      int counter = 1;

      EV_SET(&filter, 1, EVFILT_TIMER, EV_ADD | EV_ENABLE, NOTE_SECONDS, 10, NULL);

      while(!thread_exit_) {
        kevent(kq_, &filter, 1, &event, 1, NULL);
        std::cerr << "metrics: beep " << counter++ << std::endl;
      }

      RUBINIUS_THREAD_STOP(const_cast<RBX_DTRACE_CONST char*>(thread_name),
                           state->vm()->thread_id(), 1);
    }
  }
}
