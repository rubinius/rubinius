#include "config.h"
#include "vm.hpp"
#include "state.hpp"
#include "on_stack.hpp"
#include "memory.hpp"
#include "call_frame.hpp"
#include "metrics.hpp"
#include "exception_point.hpp"

#include "builtin/array.hpp"
#include "builtin/class.hpp"
#include "builtin/module.hpp"
#include "builtin/thread.hpp"
#include "builtin/native_method.hpp"

#include "capi/handle.hpp"

#include "memory/finalizer.hpp"

#include "logger.hpp"

#include "dtrace/dtrace.h"

namespace rubinius {
namespace memory {
  FinalizerThread::iterator::iterator(FinalizerThread* fh)
    : handler_(fh)
    , current_list_(NULL)
  {
    if(handler_->live_list_->empty()) {
      if(handler_->lists_->empty()) {
        current_ = handler_->live_list_->begin();
        end_ = handler_->live_list_->end();
        return;
      } else {
        current_list_ = handler_->lists_->front();
      }
    } else {
      current_list_ = handler_->live_list_;
    }

    current_ = current_list_->begin();

    if(handler_->lists_->empty()) {
      end_ = current_list_->end();
    } else {
      lists_iterator_ = handler_->lists_->begin();
      end_ = handler_->lists_->back()->end();
    }
  }

  void FinalizerThread::iterator::next(bool live) {
    if(current_list_ == handler_->live_list_) {
      if(!live) current_->queued();

      if(++current_ == current_list_->end()) {
        if(!handler_->lists_->empty()) {
          current_list_ = *lists_iterator_;
          current_ = current_list_->begin();
        }
      }
    } else {
      if(++current_ == end_) {
        return;
      } else if(current_ == current_list_->end()) {
        if(++lists_iterator_ != handler_->lists_->end()) {
          current_list_ = *lists_iterator_;
          current_ = current_list_->begin();
        }
      }
    }
  }

  bool FinalizerThread::iterator::end() {
    return current_ == end_;
  }

  FinalizerThread::FinalizerThread(STATE)
    : InternalThread(state, "rbx.finalizer", InternalThread::eXLarge)
    , lists_(NULL)
    , live_list_(NULL)
    , process_list_(NULL)
    , iterator_(NULL)
    , process_item_kind_(eRuby)
    , finishing_(false)
  {
    state->shared().set_finalizer_handler(this);

    lists_ = new FinalizeObjectsList();
    live_list_ = new FinalizeObjects();
  }

  FinalizerThread::~FinalizerThread() {
    if(iterator_) delete iterator_;
    if(live_list_) delete live_list_;

    if(lists_) {
      for(FinalizeObjectsList::iterator i = lists_->begin(); i != lists_->end(); ++i) {
        delete *i;
      }
      delete lists_;
    }
  }

  void FinalizerThread::initialize(STATE) {
    InternalThread::initialize(state);

    Thread::create(state, vm());

    live_guard_.init();
    worker_lock_.init();
    worker_cond_.init();
    supervisor_lock_.init();
    supervisor_cond_.init();
    finishing_ = false;
  }

  void FinalizerThread::wakeup(STATE) {
    utilities::thread::Mutex::LockGuard lg(worker_lock_);

    InternalThread::wakeup(state);

    worker_signal();
  }

  void FinalizerThread::stop(STATE) {
    state->shared().internal_threads()->unregister_thread(this);

    stop_thread(state);
  }

  void FinalizerThread::run(STATE) {
    state->vm()->become_managed();

    while(!thread_exit_) {
      if(!process_list_) first_process_item();

      if(!process_list_) {
        {
          utilities::thread::Mutex::LockGuard lg(worker_lock_);

          // exit_ might have been set after we grabbed the worker_lock
          if(thread_exit_) break;

          state->vm()->become_unmanaged();
          worker_wait();

          if(thread_exit_) break;
        }

        state->vm()->become_managed();

        {
          utilities::thread::Mutex::LockGuard lg(worker_lock_);
          if(thread_exit_) break;
        }

        continue;
      }

      finalize(state);
      next_process_item();
    }
  }

  void FinalizerThread::finalize(STATE) {
    switch(process_item_kind_) {
    case eRuby: {
      if(process_item_->ruby_finalizer) {
        // Rubinius specific code. If the finalizer is cTrue, then send the
        // object the __finalize__ message.
        if(process_item_->ruby_finalizer->true_p()) {
          process_item_->object->send(state, state->symbol("__finalize__"));
        } else {
          Array* ary = Array::create(state, 1);
          ary->set(state, 0, process_item_->object->id(state));
          if(!process_item_->ruby_finalizer->send(state, G(sym_call), ary)) {
            if(state->vm()->thread_state()->raise_reason() == cException) {
              logger::warn(
                  "finalizer: an exception occurred running a Ruby finalizer: %s",
                  state->vm()->thread_state()->current_exception()->message_c_str(state));
            }
          }
        }
      }

      process_item_->status = FinalizeObject::eRubyFinalized;
      break;
    }
    case eNative:
      if(process_item_->finalizer) {
        if(process_item_->kind == FinalizeObject::eUnmanaged) {
          (*process_item_->finalizer)(state, process_item_->object);
        } else {
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
          call_frame->optional_jit_data = 0;
          call_frame->top_scope_ = 0;
          call_frame->scope = 0;
          call_frame->arguments = 0;

          env->set_current_call_frame(0);
          env->set_current_native_frame(&nmf);

          // Register the CallFrame, because we might GC below this.
          state->vm()->push_call_frame(call_frame, previous_frame);

          nmf.setup(Qnil, Qnil, Qnil, Qnil);

          PLACE_EXCEPTION_POINT(ep);

          if(unlikely(ep.jumped_to())) {
            logger::warn(
                "finalizer: an exception occurred running a NativeMethod finalizer");
          } else {
            (*process_item_->finalizer)(state, process_item_->object);
          }

          state->vm()->pop_call_frame(previous_frame);
          env->set_current_call_frame(0);
          env->set_current_native_frame(0);
        }
      }
      process_item_->status = FinalizeObject::eNativeFinalized;
      break;
    case eRelease:
      // Unhook any handle used by fi->object so that we don't accidentally
      // try and mark it later (after we've finalized it)
      if(capi::Handle* handle = process_item_->object->handle(state)) {
        handle->forget_object();
        process_item_->object->clear_handle(state);
      }

      process_item_->status = FinalizeObject::eReleased;

      break;
    }
  }

  void FinalizerThread::first_process_item() {
    if(!process_list_ && !lists_->empty()) {
      process_list_ = lists_->back();
      process_item_ = process_list_->begin();
    }
  }

  void FinalizerThread::next_process_item() {
    if(++process_item_ == process_list_->end()) {
      switch(process_item_kind_) {
      case eRuby:
        process_item_ = process_list_->begin();
        process_item_kind_ = eNative;
        break;
      case eNative:
        process_item_ = process_list_->begin();
        process_item_kind_ = eRelease;
        break;
      case eRelease:
        delete process_list_;
        process_list_ = NULL;
        process_item_kind_ = eRuby;
        lists_->pop_back();
        vm()->metrics().gc.objects_finalized++;
        break;
      }
    }
  }

  void FinalizerThread::finish(STATE) {
    finishing_ = true;
    if(process_list_ || !lists_->empty() || !live_list_->empty()) {
      while(true) {
        if(!process_list_) {
          if(live_list_->empty() && lists_->empty()) break;

          // Everything is garbage when halting so keep adding live objects to
          // finalize queue until done.
          if(!live_list_->empty()) {
            for(FinalizeObjects::iterator i = live_list_->begin();
                i != live_list_->end();
                ++i)
            {
              i->queued();
            }

            queue_objects(state);
          }

          first_process_item();
          if(!process_list_) break;
        }

        while(process_list_) {
          finalize(state);
          next_process_item();
        }
      }
    }

    if(!lists_->empty() || !live_list_->empty() || process_list_ != NULL) {
      logger::warn("FinalizerThread exiting with pending finalizers");
    }

    VM::discard(state, vm());
  }

  void FinalizerThread::record(STATE, Object* obj, FinalizerFunction func,
      FinalizeObject::FinalizeKind kind)
  {
    utilities::thread::Mutex::LockGuard lg(live_guard_);

    if(!obj) {
      Exception::raise_runtime_error(state, "NULL object added to finalization list");
    }

    if(finishing_) return;

    FinalizeObject fi;
    fi.object = obj;
    fi.kind = kind;
    fi.status = FinalizeObject::eLive;
    fi.finalizer = func;

    // Makes a copy of fi.
    live_list_->push_front(fi);

    vm()->metrics().gc.objects_queued++;
  }

  void FinalizerThread::set_ruby_finalizer(Object* obj, Object* finalizer) {
    utilities::thread::Mutex::LockGuard lg(live_guard_);

    // Ignore Ruby finalizers created when finishing running finalizers.
    if(finishing_) return;

    // Check if the object is already in the finalizer list.
    for(FinalizeObjects::iterator i = live_list_->begin();
        i != live_list_->end();
        ++i)
    {
      if(i->object == obj) {
        if(finalizer->nil_p()) {
          live_list_->erase(i);
        } else {
          i->ruby_finalizer = finalizer;
        }
        return;
      }
    }

    // Adding a nil finalizer is only used to delete an existing finalizer,
    // which we apparently don't have if we get here.
    if(finalizer->nil_p()) {
      return;
    }

    // Ok, create it.

    FinalizeObject fi;
    fi.object = obj;
    fi.status = FinalizeObject::eLive;

    // Rubinius specific API. If the finalizer is the object, we're going to send
    // the object __finalize__. We mark that the user wants this by putting cTrue
    // as the ruby_finalizer.
    if(obj == finalizer) {
      fi.ruby_finalizer = cTrue;
    } else {
      fi.ruby_finalizer = finalizer;
    }

    // Makes a copy of fi.
    live_list_->push_front(fi);
  }

  void FinalizerThread::queue_objects(STATE) {
    if(live_list_->empty()) return;

    FinalizeObjects* dead_list = new FinalizeObjects();

    for(FinalizeObjects::iterator i = live_list_->begin();
        i != live_list_->end();
        /* advance is handled in the loop */)
    {
      if(i->queued_p()) {
        if(i->kind == FinalizeObject::eUnmanaged && !i->ruby_finalizer) {
          i->finalizer(state, i->object);
        } else {
          dead_list->push_front(*i);
        }

        i = live_list_->erase(i);
      } else {
        ++i;
      }
    }

    if(!dead_list->empty()) {
      lists_->push_front(dead_list);
    } else {
      delete dead_list;
    }
  }

  void FinalizerThread::start_collection(STATE) {
    if(process_item_kind_ == eRelease) {
      while(process_list_) {
        finalize(state);
        next_process_item();
      }
    }
  }

  void FinalizerThread::finish_collection(STATE) {
    queue_objects(state);

    if(iterator_) {
      delete iterator_;
      iterator_ = NULL;
    }

    worker_signal();
  }

  void FinalizerThread::supervisor_signal() {
    supervisor_cond_.signal();
  }

  void FinalizerThread::supervisor_wait() {
    supervisor_cond_.wait(supervisor_lock_);
  }

  void FinalizerThread::worker_signal() {
    worker_cond_.signal();
  }

  void FinalizerThread::worker_wait() {
    worker_cond_.wait(worker_lock_);
  }

  FinalizerThread::iterator& FinalizerThread::begin() {
    if(iterator_) delete iterator_;
    iterator_ = new iterator(this);
    return *iterator_;
  }
}
}
