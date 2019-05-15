#include "config.h"
#include "vm.hpp"
#include "state.hpp"
#include "on_stack.hpp"
#include "memory.hpp"
#include "call_frame.hpp"
#include "exception_point.hpp"
#include "thread_nexus.hpp"
#include "thread_phase.hpp"

#include "class/array.hpp"
#include "class/class.hpp"
#include "class/module.hpp"
#include "class/thread.hpp"
#include "class/native_method.hpp"
#include "class/call_site.hpp"

#include "diagnostics/gc.hpp"
#include "diagnostics/timing.hpp"

#include "memory/collector.hpp"
#include "memory/mark_sweep.hpp"

#include "dtrace/dtrace.h"

namespace rubinius {
  namespace memory {
    void NativeFinalizer::dispose(STATE) {
      // TODO: consider building this on the TypeInfo structure.
      if(Fiber* fiber = try_as<Fiber>(object())) {
        if(!fiber->vm()->zombie_p()) fiber->cancel(state);
      }
    }

    void NativeFinalizer::finalize(STATE) {
      (*finalizer_)(state, object());
    }

    void NativeFinalizer::mark(ImmixGC* gc) {
      if(Object* fwd = gc->saw_object(0, object())) {
        object(fwd);
      }
    }

    void ExtensionFinalizer::dispose(STATE) {
    }

    void ExtensionFinalizer::finalize(STATE) {
      NativeMethodEnvironment* env = state->vm()->native_method_environment;
      NativeMethodFrame nmf(env, 0, 0);
      ExceptionPoint ep(env);

      CallFrame* previous_frame = nullptr;
      CallFrame* call_frame = ALLOCA_CALL_FRAME(0);

      call_frame->previous = nullptr;
      call_frame->lexical_scope_ = nullptr;
      call_frame->dispatch_data = (void*)&nmf;
      call_frame->compiled_code = nullptr;
      call_frame->flags = CallFrame::cNativeMethod;
      call_frame->top_scope_ = nullptr;
      call_frame->scope = nullptr;
      call_frame->arguments = nullptr;
      call_frame->return_value = nullptr;
      call_frame->unwind = nullptr;

      env->set_current_call_frame(0);
      env->set_current_native_frame(&nmf);

      // Register the CallFrame, because we might GC below this.
      if(state->vm()->push_call_frame(state, call_frame, previous_frame)) {
        nmf.setup(Qnil, Qnil, Qnil, Qnil);

        PLACE_EXCEPTION_POINT(ep);

        if(unlikely(ep.jumped_to())) {
          logger::warn(
              "collector: an exception occurred running a NativeMethod finalizer");
        } else {
          (*finalizer_)(state, object());
        }

        state->vm()->pop_call_frame(state, previous_frame);
        env->set_current_call_frame(0);
        env->set_current_native_frame(0);
      } else {
        logger::warn("collector: stack error");
      }
    }

    void ExtensionFinalizer::mark(ImmixGC* gc) {
      if(Object* fwd = gc->saw_object(0, object())) {
        object(fwd);
      }
    }

    void ManagedFinalizer::dispose(STATE) {
    }

    void ManagedFinalizer::finalize(STATE) {
      /* Rubinius specific code. If the finalizer is cTrue, then send the
       * object the __finalize__ message.
       */
      if(finalizer_->true_p()) {
        object()->send(state, state->symbol("__finalize__"));
      } else {
        Array* ary = Array::create(state, 1);
        ary->set(state, 0, object()->object_id(state));
        if(!finalizer_->send(state, G(sym_call), ary)) {
          if(state->vm()->thread_state()->raise_reason() == cException) {
            logger::warn(
                "collector: an exception occurred running a Ruby finalizer: %s",
                state->vm()->thread_state()->current_exception()->message_c_str(state));
          }
        }
      }
    }

    void ManagedFinalizer::mark(ImmixGC* gc) {
      if(Object* fwd = gc->saw_object(0, finalizer_)) {
        finalizer_ = fwd;
      }

      if(Object* fwd = gc->saw_object(finalizer_, object())) {
        object(fwd);
      }
    }

    bool ManagedFinalizer::match_p(STATE, Object* object, Object* finalizer) {
      bool match = false;

      if(this->object() == object) {
        if(!finalizer->nil_p()) {
          Array* args = Array::create(state, 1);
          args->set(state, 0, finalizer_);

          Object* result = finalizer->send(state, G(sym_equal), args);
          match = result && CBOOL(result);
        }
      }

      return match;
    }

    Collector::Collector(STATE)
      : worker_(nullptr)
      , live_list_()
      , process_list_()
      , list_mutex_()
      , list_condition_()
      , finishing_(false)
      , inhibit_collection_(0)
      , collect_requested_(false)
    {
    }

    Collector::~Collector() {
      if(!live_list_.empty()) {
        logger::error("collector: destructed with remaining finalizer objects");
      }

      if(!process_list_.empty()) {
        logger::error("collector: destructed with remaining to-be-finalized objects");
      }
    }

    void Collector::stop(STATE) {
      if(worker_) {
        worker_->stop(state);
        worker_ = nullptr;
      }
    }

    void Collector::start(STATE) {
      if(!worker_) {
        worker_ = new Worker(state, this, process_list_, list_mutex_, list_condition_);
        worker_->start(state);
      }
    }

    void Collector::stop_for_collection(STATE, std::function<void ()> process) {
      logger::info("collector: collection cycle started");

      if(state->vm()->thread_nexus()->try_lock_wait(state, state->vm())) {
        state->vm()->metrics()->stops++;
        state->vm()->thread_nexus()->set_stop();

        logger::info("collector: stop initiated, waiting for all threads to checkpoint");

        state->vm()->thread_nexus()->wait_for_all(state, state->vm());

#ifdef RBX_GC_STACK_CHECK
        state->vm()->thread_nexus()->check_stack(state, state->vm());
#endif

        logger::info("collector: collection started");

        process();

        logger::info("collector: collection finished");

        state->vm()->thread_nexus()->unset_stop();
        state->vm()->thread_nexus()->unlock(state, state->vm());
      }
    }

    void Collector::trace_roots(STATE) {
    }

    void Collector::collect(STATE) {
      timer::StopWatch<timer::milliseconds> timerx(
          state->shared().gc_metrics()->immix_stop_ms);

      stop_for_collection(state, [this, state]{
          memory::GCData data(state->vm());

          state->memory()->collect_cycle();

          state->memory()->code_manager().clear_marks();
          state->memory()->immix()->reset_stats();

          trace_roots(state);

          state->memory()->immix()->collect(&data);
          state->memory()->immix()->collect_finish(&data);

          state->memory()->code_manager().sweep();
          state->memory()->immix()->sweep(&data);
          state->memory()->mark_sweep()->after_marked();
          state->shared().symbols.sweep(state);

          state->memory()->rotate_mark();

          diagnostics::GCMetrics* metrics = state->shared().gc_metrics();
          metrics->immix_count++;
          metrics->large_count++;
        });

      collect_completed(state);
    }

    Collector::Worker::Worker(STATE, Collector* collector,
        FinalizerObjects& list, std::mutex& lk, std::condition_variable& cond)
      : MachineThread(state, "rbx.collector", MachineThread::eLarge)
      , collector_(collector)
      , process_list_(list)
      , list_mutex_(lk)
      , list_condition_(cond)
    {
      initialize(state);
    }

    void Collector::Worker::initialize(STATE) {
      Thread::create(state, vm());
    }

    void Collector::Worker::wakeup(STATE) {
      MachineThread::wakeup(state);

      while(thread_running_p()) {
        collector_->notify();
      }
    }

    void Collector::Worker::stop(STATE) {
      state->shared().machine_threads()->unregister_thread(this);

      MachineThread::stop_thread(state);
    }

    void Collector::Worker::after_fork_child(STATE) {
      new(&list_mutex_) std::mutex;
      new(&list_condition_) std::condition_variable;

      MachineThread::after_fork_child(state);
    }

    void Collector::Worker::run(STATE) {
      state->vm()->managed_phase(state);

      logger::info("collector: worker thread starting");

      while(!thread_exit_) {
        if(process_list_.empty()) {
          UnmanagedPhase unmanaged(state);

          std::unique_lock<std::mutex> lk(list_mutex_);
          list_condition_.wait(lk,
              [this]{ return thread_exit_ || collector_->collect_requested_p() || !process_list_.empty(); });
        }

        if(collector_->collect_requested_p()) {
          collector_->collect(state);
        }

        while(!process_list_.empty()) {
          if(thread_exit_) break;

          FinalizerObject* object = 0;

          {
            std::lock_guard<std::mutex> guard(list_mutex_);

            object = process_list_.back();
            process_list_.pop_back();
          }

          if(object) {
            object->finalize(state);
            delete object;

            state->shared().gc_metrics()->objects_finalized++;
          }

          state->vm()->thread_nexus()->yield(state, state->vm());
        }
      }

      collector_->worker_exited(state);

      logger::info("collector: worker thread exited");

      state->vm()->unmanaged_phase(state);
      state->vm()->thread()->vm()->set_zombie(state);
    }

    void Collector::dispose(STATE) {
      finishing_ = true;

      std::lock_guard<std::mutex> guard(list_mutex());

      for(FinalizerObjects::iterator i = process_list_.begin();
          i != process_list_.end();
          ++i)
      {
        FinalizerObject* fo = *i;
        fo->dispose(state);
      }

      for(FinalizerObjects::iterator i = live_list_.begin();
          i != live_list_.end();
          ++i)
      {
        FinalizerObject* fo = *i;
        fo->dispose(state);
      }
    }

    void Collector::finish(STATE) {
      finishing_ = true;

      while(!process_list_.empty()) {
        FinalizerObject* fo = process_list_.back();
        process_list_.pop_back();

        fo->finalize(state);
        delete fo;

        state->shared().gc_metrics()->objects_finalized++;
      }

      while(!live_list_.empty()) {
        FinalizerObject* fo = live_list_.back();
        live_list_.pop_back();

        fo->finalize(state);
        delete fo;

        state->shared().gc_metrics()->objects_finalized++;
      }
    }

    void Collector::native_finalizer(STATE, Object* obj, FinalizerFunction func) {
      if(finishing_) return;

      UnmanagedPhase unmanaged(state);
      std::lock_guard<std::mutex> guard(list_mutex());

      add_finalizer(state, new NativeFinalizer(state, obj, func));
    }

    void Collector::extension_finalizer(STATE, Object* obj, FinalizerFunction func) {
      if(finishing_) return;

      UnmanagedPhase unmanaged(state);
      std::lock_guard<std::mutex> guard(list_mutex());

      add_finalizer(state, new ExtensionFinalizer(state, obj, func));
    }

    void Collector::managed_finalizer(STATE, Object* obj, Object* finalizer) {
      if(finishing_) return;

      /* This method will be called by a managed thread during a managed
       * phase. We acquire this list mutex *while managed* to prevent
       * garbage collection from running, since that is the only place that
       * the inversion of managed phase and locking this mutex can occur.
       *
       * Since Ruby allows any number of finalizers on a single object as
       * long as the finalizer "callable" is different, we have to do a
       * complex comparison to determine if the "callable" is different.
       * This must be done during a managed phase even if it were not a
       * method send because it works with managed objects.
       */
      std::lock_guard<std::mutex> guard(list_mutex());

      for(FinalizerObjects::iterator i = live_list_.begin();
          i != live_list_.end();
          /* advance is handled in the loop */)
      {
        FinalizerObject* fo = *i;

        if(fo->match_p(state, obj, finalizer)) {
          if(finalizer->nil_p()) {
            i = live_list_.erase(i);
            continue;
          } else {
            return;
          }
        }

        ++i;
      }

      if(finalizer->nil_p()) return;

      /* Rubinius specific API. If the finalizer is the object, we're going to
       * send the object __finalize__. We mark that the user wants this by
       * putting cTrue as the ruby_finalizer.
       */
      add_finalizer(state, new ManagedFinalizer(state, obj,
            obj == finalizer ? cTrue : finalizer));
    }

    void Collector::add_finalizer(STATE, FinalizerObject* obj) {
      live_list_.push_back(obj);
      state->shared().gc_metrics()->objects_queued++;
    }

    void Collector::gc_scan(ImmixGC* gc, Memory* memory) {
      for(FinalizerObjects::iterator i = live_list_.begin();
          i != live_list_.end();
          /* advance is handled in the loop */)
      {
        FinalizerObject* fo = *i;

        if(!fo->object()->marked_p(memory->mark())) {
          process_list_.push_back(*i);
          i = live_list_.erase(i);
        } else {
          // It's possible that the ManagedFinalizer finalizer is a GC root.
          fo->mark(gc);

          ++i;
        }
      }

      for(FinalizerObjects::iterator i = process_list_.begin();
          i != process_list_.end();
          ++i)
      {
        FinalizerObject* fo = *i;

        fo->mark(gc);
      }
    }
  }
}
