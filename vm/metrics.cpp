#include "vm.hpp"
#include "metrics.hpp"

#include "object_utils.hpp"
#include "shared_state.hpp"
#include "configuration.hpp"

#include "builtin/class.hpp"
#include "builtin/thread.hpp"

#include "llvm/state.hpp"

#include "gc/managed.hpp"

#include "dtrace/dtrace.h"

#include "util/logger.hpp"

#include <stdint.h>

namespace rubinius {
  using namespace utilities;

  namespace metrics {
    void RubyMetrics::add(MetricsData* data) {
      bytes_read += data->m.ruby_metrics.bytes_read;
      bytes_written += data->m.ruby_metrics.bytes_written;
      signals_received += data->m.ruby_metrics.signals_received;
      signals_processed += data->m.ruby_metrics.signals_processed;
    }

    void FinalizerMetrics::add(MetricsData* data) {
      objects_queued += data->m.finalizer_metrics.objects_queued;
      objects_finalized += data->m.finalizer_metrics.objects_finalized;
    }

    void ImmixMetrics::add(MetricsData* data) {
    }

    void JITMetrics::add(MetricsData* data) {
      methods_queued += data->m.jit_metrics.methods_queued;
      methods_compiled += data->m.jit_metrics.methods_compiled;
    }

    void ConsoleMetrics::add(MetricsData* data) {
    }

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
      , interval_(state->shared().config.vm_metrics_interval)
      , timer_(NULL)
    {
      shared_.auxiliary_threads()->register_thread(this);
    }

    Metrics::~Metrics() {
      if(timer_) delete timer_;

      shared_.auxiliary_threads()->unregister_thread(this);
    }

    void Metrics::start(STATE) {
      timer_ = new timer::Timer;
      metrics_collection_.init();
      metrics_history_.init();

      start_thread(state);
    }

    void Metrics::wakeup() {
      thread_exit_ = true;

      atomic::memory_barrier();

      if(timer_) timer_->cancel();
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

        void* return_value;
        pthread_join(os, &return_value);

        vm_ = NULL;
      }
    }

    void Metrics::shutdown(STATE) {
      stop_thread(state);
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
      state->gc_dependent(gct, 0);

      timer_->set(interval_);

      while(!thread_exit_) {
        {
          GCIndependent guard(state, 0);

          if(timer_->wait_for_tick() < 0) {
            logger::error("metrics: error waiting for timer, exiting thread");
            break;
          }
        }

        if(thread_exit_) break;

        {
          StopTheWorld world_lock(state, gct, 0);

          ThreadList* threads = state->shared().threads();

          for(ThreadList::iterator i = threads->begin();
              i != threads->end();
              ++i) {
            if(VM* vm = (*i)->as_vm()) {
              if(MetricsData* data = vm->metrics()) {
                metrics_collection_.add(data);
              }
            }
          }

          if(state->shared().llvm_state) {
            metrics_collection_.add(state->shared().llvm_state->metrics());
          } else {
            std::cerr << "metrics: no damn llvm_state" << std::endl;
          }
        }

        {
          GCIndependent guard(state, 0);

        }
      }

      timer_->clear();

      RUBINIUS_THREAD_STOP(const_cast<RBX_DTRACE_CONST char*>(thread_name),
                           state->vm()->thread_id(), 1);
    }
  }
}
