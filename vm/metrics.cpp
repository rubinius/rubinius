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
      io_bytes_read += data->m.ruby_metrics.io_bytes_read;
      io_bytes_written += data->m.ruby_metrics.io_bytes_written;
      os_signals_received += data->m.ruby_metrics.os_signals_received;
      os_signals_processed += data->m.ruby_metrics.os_signals_processed;
      total_data_objects += data->m.ruby_metrics.total_data_objects;
      total_capi_handles += data->m.ruby_metrics.total_capi_handles;
      capi_handles += data->m.ruby_metrics.capi_handles;
      inflated_headers += data->m.ruby_metrics.inflated_headers;
      inline_cache_resets += data->m.ruby_metrics.inline_cache_resets;
      memory_young_bytes += data->m.ruby_metrics.memory_young_bytes;
      memory_young_objects += data->m.ruby_metrics.memory_young_objects;
      memory_young_percent_used += data->m.ruby_metrics.memory_young_percent_used;
      memory_immix_bytes += data->m.ruby_metrics.memory_immix_bytes;
      memory_immix_objects += data->m.ruby_metrics.memory_immix_objects;
      memory_immix_chunks += data->m.ruby_metrics.memory_immix_chunks;
      memory_large_bytes += data->m.ruby_metrics.memory_large_bytes;
      memory_large_objects += data->m.ruby_metrics.memory_large_objects;
      memory_symbols_bytes += data->m.ruby_metrics.memory_symbols_bytes;
      memory_code_bytes += data->m.ruby_metrics.memory_code_bytes;
      memory_jit_bytes += data->m.ruby_metrics.memory_jit_bytes;
      memory_total_young_bytes += data->m.ruby_metrics.memory_total_young_bytes;
      memory_total_young_objects += data->m.ruby_metrics.memory_total_young_objects;
      memory_total_immix_bytes += data->m.ruby_metrics.memory_total_immix_bytes;
      memory_total_immix_objects += data->m.ruby_metrics.memory_total_immix_objects;
      memory_total_immix_chunks += data->m.ruby_metrics.memory_total_immix_chunks;
      memory_total_large_bytes += data->m.ruby_metrics.memory_total_large_bytes;
      memory_total_large_objects += data->m.ruby_metrics.memory_total_large_objects;
      memory_total_promoted_bytes += data->m.ruby_metrics.memory_total_promoted_bytes;
      memory_total_promoted_objects += data->m.ruby_metrics.memory_total_promoted_objects;
      memory_total_slab_refills += data->m.ruby_metrics.memory_total_slab_refills;
      memory_total_slab_refill_fails += data->m.ruby_metrics.memory_total_slab_refill_fails;
      gc_young_count += data->m.ruby_metrics.gc_young_count;
      gc_young_last_ms += data->m.ruby_metrics.gc_young_last_ms;
      gc_young_total_ms += data->m.ruby_metrics.gc_young_total_ms;
      gc_immix_count += data->m.ruby_metrics.gc_immix_count;
      gc_immix_last_stop_ms += data->m.ruby_metrics.gc_immix_last_stop_ms;
      gc_immix_total_stop_ms += data->m.ruby_metrics.gc_immix_total_stop_ms;
      gc_immix_last_conc_ms += data->m.ruby_metrics.gc_immix_last_conc_ms;
      gc_immix_total_conc_ms += data->m.ruby_metrics.gc_immix_total_conc_ms;
      gc_large_count += data->m.ruby_metrics.gc_large_count;
      gc_large_last_sweep_ms += data->m.ruby_metrics.gc_large_last_sweep_ms;
      gc_large_total_sweep_ms += data->m.ruby_metrics.gc_large_total_sweep_ms;
    }

    void FinalizerMetrics::add(MetricsData* data) {
      objects_queued += data->m.finalizer_metrics.objects_queued;
      objects_finalized += data->m.finalizer_metrics.objects_finalized;
    }

    void JITMetrics::add(MetricsData* data) {
      methods_queued += data->m.jit_metrics.methods_queued;
      methods_compiled += data->m.jit_metrics.methods_compiled;
    }

    void ConsoleMetrics::add(MetricsData* data) {
      requests_received += data->m.console_metrics.requests_received;
      responses_sent += data->m.console_metrics.responses_sent;
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

          metrics_collection_.init();
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
