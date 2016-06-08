#include "config.h"
#include "vm.hpp"
#include "state.hpp"
#include "on_stack.hpp"
#include "memory.hpp"
#include "call_frame.hpp"
#include "metrics.hpp"
#include "exception_point.hpp"
#include "thread_phase.hpp"

#include "builtin/array.hpp"
#include "builtin/class.hpp"
#include "builtin/module.hpp"
#include "builtin/thread.hpp"
#include "builtin/native_method.hpp"
#include "builtin/call_site.hpp"

#include "capi/handle.hpp"

#include "memory/finalizer.hpp"

#include "logger.hpp"

#include "dtrace/dtrace.h"

namespace rubinius {
  namespace memory {
    void NativeFinalizer::finalize(STATE) {
      (*finalizer_)(state, object());
    }

    void NativeFinalizer::mark(ImmixGC* gc) {
      if(Object* fwd = gc->saw_object(object())) {
        object(fwd);
      }
    }

    void ExtensionFinalizer::finalize(STATE) {
      ManagedPhase managed(state);

      NativeMethodEnvironment* env = state->vm()->native_method_environment;
      NativeMethodFrame nmf(env, 0, 0);
      ExceptionPoint ep(env);

      CallFrame* previous_frame = 0;
      CallFrame* call_frame = ALLOCA_CALL_FRAME(0);

      call_frame->previous = NULL;
      call_frame->constant_scope_ = 0;
      call_frame->dispatch_data = (void*)&nmf;
      call_frame->compiled_code = 0;
      call_frame->flags = CallFrame::cNativeMethod;
      call_frame->top_scope_ = 0;
      call_frame->scope = 0;
      call_frame->arguments = 0;

      env->set_current_call_frame(0);
      env->set_current_native_frame(&nmf);

      // Register the CallFrame, because we might GC below this.
      if(state->vm()->push_call_frame(state, call_frame, previous_frame)) {
        nmf.setup(Qnil, Qnil, Qnil, Qnil);

        PLACE_EXCEPTION_POINT(ep);

        if(unlikely(ep.jumped_to())) {
          logger::warn(
              "finalizer: an exception occurred running a NativeMethod finalizer");
        } else {
          (*finalizer_)(state, object());
        }

        state->vm()->pop_call_frame(state, previous_frame);
        env->set_current_call_frame(0);
        env->set_current_native_frame(0);
      } else {
        logger::warn("finalizer: stack error");
      }
    }

    void ExtensionFinalizer::mark(ImmixGC* gc) {
      if(Object* fwd = gc->saw_object(object())) {
        object(fwd);
      }
    }

    void ManagedFinalizer::finalize(STATE) {
      ManagedPhase managed(state);

      /* Rubinius specific code. If the finalizer is cTrue, then send the
       * object the __finalize__ message.
       */
      if(finalizer_->true_p()) {
        object()->send(state, state->symbol("__finalize__"));
      } else {
        Array* ary = Array::create(state, 1);
        ary->set(state, 0, object()->id(state));
        if(!finalizer_->send(state, G(sym_call), ary)) {
          if(state->vm()->thread_state()->raise_reason() == cException) {
            logger::warn(
                "finalizer: an exception occurred running a Ruby finalizer: %s",
                state->vm()->thread_state()->current_exception()->message_c_str(state));
          }
        }
      }
    }

    void ManagedFinalizer::mark(ImmixGC* gc) {
      if(Object* fwd = gc->saw_object(finalizer_)) {
        finalizer_ = fwd;
      }

      if(Object* fwd = gc->saw_object(object())) {
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

    FinalizerThread::FinalizerThread(STATE)
      : MachineThread(state, "rbx.finalizer", MachineThread::eLarge)
      , live_list_()
      , process_list_()
      , synchronization_(NULL)
      , finishing_(false)
    {
      initialize(state);
      state->shared().set_finalizer(this);
    }

    FinalizerThread::~FinalizerThread() {
      if(!live_list_.empty()) {
        rubinius::bug("FinalizerThread destructed with remaining finalizer objects");
      }

      if(!process_list_.empty()) {
        rubinius::bug("FinalizerThread destructed with remaining to-be-finalized objects");
      }

      cleanup();
    }

    void FinalizerThread::cleanup() {
      if(synchronization_) {
        delete synchronization_;
        synchronization_ = NULL;
      }
    }

    void FinalizerThread::after_fork_child(STATE) {
      cleanup();

      MachineThread::after_fork_child(state);
    }

    void FinalizerThread::initialize(STATE) {
      synchronization_ = new Synchronization();
    }

    void FinalizerThread::wakeup(STATE) {
      MachineThread::wakeup(state);

      while(thread_running_) {
        std::lock_guard<std::mutex> guard(list_mutex());
        list_condition().notify_one();
      }
    }

    void FinalizerThread::stop(STATE) {
      state->shared().machine_threads()->unregister_thread(this);

      stop_thread(state);
    }

    void FinalizerThread::run(STATE) {
      while(true) {
        FinalizerObject* object = 0;

        {
          std::unique_lock<std::mutex> lk(list_mutex());
          list_condition().wait(lk,
              [this]{ return thread_exit_ || !process_list_.empty(); });

          if(thread_exit_) {
            return;
          } else {
            object = process_list_.back();
            process_list_.pop_back();
          }
        }

        if(object) {
          object->finalize(state);
          delete object;

          vm()->metrics().gc.objects_finalized++;
        }
      }
    }

    void FinalizerThread::finish(STATE) {
      finishing_ = true;

      // TODO: cleanup
      while(!process_list_.empty()) {
        FinalizerObject* fo = process_list_.back();
        process_list_.pop_back();

        fo->finalize(state);
        delete fo;

        vm()->metrics().gc.objects_finalized++;
      }

      while(!live_list_.empty()) {
        FinalizerObject* fo = live_list_.back();
        live_list_.pop_back();

        fo->finalize(state);
        delete fo;

        vm()->metrics().gc.objects_finalized++;
      }
    }

    void FinalizerThread::native_finalizer(STATE, Object* obj, FinalizerFunction func) {
      if(finishing_) return;

      add_finalizer(state, new NativeFinalizer(state, obj, func));
    }

    void FinalizerThread::extension_finalizer(STATE, Object* obj, FinalizerFunction func) {
      if(finishing_) return;

      add_finalizer(state, new ExtensionFinalizer(state, obj, func));
    }

    void FinalizerThread::managed_finalizer(STATE, Object* obj, Object* finalizer) {
      if(finishing_) return;

      {
        std::lock_guard<std::mutex> guard(list_mutex());

        for(FinalizerObjects::iterator i = live_list_.begin();
            i != live_list_.end();
            /* advance is handled in the loop */)
        {
          FinalizerObject* fo = *i;

          /* TODO: Since Ruby allows any number of finalizers on a single
           * object as long as the finalizer "callable" is different, we have
           * to do a complex comparison to determine if the "callable" is
           * different. This results in a method send, which can cause a
           * CompiledCode instance to be internalized, adding CallSite object
           * finalizers, so we cannot keep this locked across this call.
           *
           * This is an utter mess and either needs to be moved into the Ruby
           * .define_finalizer method or locking handled differently on this
           * list.
           */
          list_mutex().unlock();

          if(fo->match_p(state, obj, finalizer)) {
            if(finalizer->nil_p()) {
              i = live_list_.erase(i);
              continue;
            } else {
              list_mutex().lock();
              return;
            }
          }

          list_mutex().lock();
          ++i;
        }
      }

      if(finalizer->nil_p()) return;

      /* Rubinius specific API. If the finalizer is the object, we're going to
       * send the object __finalize__. We mark that the user wants this by
       * putting cTrue as the ruby_finalizer.
       */
      add_finalizer(state, new ManagedFinalizer(state, obj,
            obj == finalizer ? cTrue : finalizer));
    }

    void FinalizerThread::add_finalizer(STATE, FinalizerObject* obj) {
      std::lock_guard<std::mutex> guard(list_mutex());

      live_list_.push_back(obj);
      vm()->metrics().gc.objects_queued++;
    }

    void FinalizerThread::gc_scan(ImmixGC* gc, Memory* memory) {
      for(FinalizerObjects::iterator i = live_list_.begin();
          i != live_list_.end();
          /* advance is handled in the loop */)
      {
        FinalizerObject* fo = *i;

        if(!fo->object()->marked_p(memory->mark())) {
          process_list_.push_back(*i);
          i = live_list_.erase(i);
        } else {
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
