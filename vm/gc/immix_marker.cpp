#include "config.h"
#include "vm.hpp"
#include "immix_marker.hpp"

#include "builtin/class.hpp"
#include "builtin/thread.hpp"
#include "configuration.hpp"
#include "gc/gc.hpp"
#include "gc/immix.hpp"
#include "ontology.hpp"
#include "metrics.hpp"

#include "dtrace/dtrace.h"
#include "instruments/timing.hpp"

namespace rubinius {

  Object* immix_marker_tramp(STATE) {
    state->memory()->immix_marker()->perform(state);
    GCTokenImpl gct;
    state->gc_dependent(gct, 0);
    return cNil;
  }

  ImmixMarker::ImmixMarker(STATE, ImmixGC* immix)
    : AuxiliaryThread()
    , shared_(state->shared())
    , vm_(NULL)
    , immix_(immix)
    , data_(NULL)
    , exit_(false)
    , running_(false)
    , thread_(state)
  {
    shared_.auxiliary_threads()->register_thread(this);
    state->memory()->set_immix_marker(this);

    initialize(state);
    start_thread(state);
  }

  ImmixMarker::~ImmixMarker() {
    shared_.auxiliary_threads()->unregister_thread(this);

    if(data_) {
      delete data_;
      data_ = NULL;
    }
  }

  void ImmixMarker::initialize(STATE) {
    run_lock_.init();
    run_cond_.init();
    vm_ = NULL;
    running_ = false;
    exit_ = false;
  }

  void ImmixMarker::start_thread(STATE) {
    SYNC(state);
    if(vm_) return;
    utilities::thread::Mutex::LockGuard lg(run_lock_);
    vm_ = state->shared().new_vm();
    vm_->metrics()->init(metrics::eRubyMetrics);
    exit_ = false;
    thread_.set(Thread::create(state, vm_, G(thread), immix_marker_tramp, true));
    run(state);
  }

  void ImmixMarker::stop_thread(STATE) {
    SYNC(state);
    if(!vm_) return;

    exit_ = true;
    atomic::memory_barrier();

    {
      GCIndependent guard(state, 0);

      while(running_) {
        atomic::pause();
        run_cond_.signal();
      }
    }

    void* return_value;
    pthread_t os = vm_->os_thread();
    pthread_join(os, &return_value);

    vm_ = NULL;
  }

  void ImmixMarker::shutdown(STATE) {
    stop_thread(state);
  }

  void ImmixMarker::after_fork_child(STATE) {
    initialize(state);

    if(data_) {
      delete data_;
      data_ = NULL;
    }

    start_thread(state);
  }

  void ImmixMarker::concurrent_mark(GCData* data) {
    utilities::thread::Mutex::LockGuard lg(run_lock_);
    data_ = data;
    run_cond_.signal();
  }

  void ImmixMarker::run(STATE) {
    int error = thread_.get()->fork_attached(state);
    if(error) rubinius::bug("Unable to immix marker thread");
  }

  void ImmixMarker::perform(STATE) {
    GCTokenImpl gct;
    RBX_DTRACE_CHAR thread_name = const_cast<RBX_DTRACE_CHAR>("rbx.immix");
    vm_->set_name(thread_name);

    RUBINIUS_THREAD_START(const_cast<RBX_DTRACE_CHAR>(thread_name),
                          state->vm()->thread_id(), 1);

    state->vm()->thread->hard_unlock(state, gct, 0);
    running_ = true;

    while(!exit_) {
      if(data_) {
        {
          timer::StopWatch<timer::milliseconds> timer(
              state->vm()->metrics()->m.ruby_metrics.gc_immix_conc_last_ms,
              state->vm()->metrics()->m.ruby_metrics.gc_immix_conc_total_ms
            );

          // Allow for a young stop the world GC to occur
          // every bunch of marks. 100 is a fairly arbitrary
          // number, based mostly on the fact it didn't cause
          // big increases in young gc times because of long
          // stop the world wait times.
          while(immix_->process_mark_stack(100)) {
            state->gc_independent(gct, 0);
            state->gc_dependent(gct, 0);
          }
        }

        if(exit_) break;

        {
          timer::StopWatch<timer::milliseconds> timer(
              state->vm()->metrics()->m.ruby_metrics.gc_immix_stop_last_ms,
              state->vm()->metrics()->m.ruby_metrics.gc_immix_stop_total_ms
            );

          // Finish and pause
          while(!state->stop_the_world()) {
            if(exit_) {
              state->restart_world();
              break;
            }
            state->checkpoint(gct, 0);
          }
          state->memory()->clear_mature_mark_in_progress();
          state->memory()->collect_mature_finish(state, data_);
        }
        state->restart_world();
      }

      {
        utilities::thread::Mutex::LockGuard lg(run_lock_);
        if(data_) {
          delete data_;
          data_ = NULL;
        }
        if(exit_) break;
        state->gc_independent(gct, 0);
        run_cond_.wait(run_lock_);
      }
      state->gc_dependent(gct, 0);
    }

    state->memory()->clear_mature_mark_in_progress();
    running_ = false;

    RUBINIUS_THREAD_STOP(const_cast<RBX_DTRACE_CHAR>(thread_name),
                         state->vm()->thread_id(), 1);
  }
}
