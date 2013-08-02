#include "config.h"

#ifndef RBX_WINDOWS
#include <sys/select.h>
#endif

#include "capi/capi.hpp"
#include "capi/18/include/ruby.h"

#include "on_stack.hpp"
#include "call_frame.hpp"
#include "exception_point.hpp"
#include "builtin/exception.hpp"
#include "builtin/thread.hpp"
#include "builtin/nativemethod.hpp"
#include "builtin/ffi_pointer.hpp"
#include "builtin/string.hpp"

#include "windows_compat.h"

using namespace rubinius;

extern "C" {

  int rb_thread_critical = 0;

  void rb_thread_schedule() {
    rb_funcall2(rb_cThread, rb_intern("pass"), 0, NULL);
  }

  void rb_thread_wait_for(struct timeval time) {
    rb_thread_select(0, 0, 0, 0, &time);
  }

  int rb_thread_select(int max, fd_set* read, fd_set* write, fd_set* except,
                       struct timeval *input_tv)
  {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    int ret = 0;

    struct timeval tv;
    struct timeval absolute_tv;
    struct timeval* tvp = NULL;

    if(input_tv) {
      // We make a new timeval rather than using input_tv because we modify it.
      tv = *input_tv;
      tvp = &tv;

      gettimeofday(&absolute_tv, NULL);

      timeradd(&absolute_tv, &tv, &absolute_tv);
    }

    for(;;) {
      LEAVE_CAPI(env->state());
      {
        GCIndependent guard(env);
        ret = select(max, read, write, except, tvp);
      }

      bool ok = env->state()->check_async(env->current_call_frame());

      ENTER_CAPI(env->state());

      if(!ok) {
        // Ok, there was an exception raised by an async event. We need
        // to unwind through the caller back the entrance to the native
        // method.

        // Only handle true exceptions being raised, eat all other requests
        // for now.

        if(env->state()->thread_state()->raise_reason() == cException) {
          capi::capi_raise_backend(env->state()->thread_state()->current_exception());
        } else {
          env->state()->thread_state()->clear();
        }
      }

      if(ret < 0 && errno == EINTR) {
        if(input_tv) {
          struct timeval cur_tv;
          gettimeofday(&cur_tv, NULL);
          timersub(&absolute_tv, &cur_tv, &tv);
        }
      } else {
        break;
      }
    }

    return ret;
  }

  VALUE rb_thread_current(void) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    Thread* thread = env->state()->vm()->thread.get();

    return env->get_handle(thread);
  }


  int rb_thread_alone() {
    VALUE list = rb_funcall(rb_cThread, rb_intern("list"), 0);
    // This is int as bool, so only return 1 if the number of threads
    // is actually 1.
    return NUM2INT(rb_funcall(list, rb_intern("size"), 0)) == 1 ? 1 : 0;
  }

  VALUE rb_thread_local_aref(VALUE thread, ID id) {
    return rb_funcall(thread, rb_intern("[]"), 1, ID2SYM(id));
  }

  VALUE rb_thread_local_aset(VALUE thread, ID id, VALUE value) {
    return rb_funcall(thread, rb_intern("[]="), 2, ID2SYM(id), value);
  }

  VALUE rb_thread_wakeup(VALUE thread) {
    return rb_funcall(thread, rb_intern("wakeup"), 0);
  }

  // THAR BE DRAGONS.
  //
  // When venturing through the valleys of the unmanaged, our hero must
  // remain vigilant and disciplined! If she should ever find a VALUE for
  // a reference in her travels: Look away! For these VALUEs are pure
  // death! Our hero must steel herself and continue on her quest, returning
  // as soon as possible to the castle of the managed.
  VALUE rb_thread_blocking_region(rb_blocking_function_t func, void* data,
                                  rb_unblock_function_t ubf, void* ubf_data) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    State* state = env->state();
    VALUE ret = Qnil;

    if(ubf == RUBY_UBF_IO || ubf == RUBY_UBF_PROCESS) {
      state->vm()->interrupt_with_signal();
    } else {
      state->vm()->wait_on_custom_function(ubf, ubf_data);
    }
    LEAVE_CAPI(env->state());
    {
      GCIndependent guard(env);
      ret = (*func)(data);
    }
    ENTER_CAPI(env->state());
    state->vm()->clear_waiter();

    return ret;
  }

  // THAR BE MORE DRAGONS.
  //
  // When venturing through the valleys of the unmanaged, our hero must
  // remain vigilant and disciplined! If she should ever find a VALUE for
  // a reference in her travels: Look away! For these VALUEs are pure
  // death! Our hero must steel herself and continue on her quest, returning
  // as soon as possible to the castle of the managed.
  void* rb_thread_call_without_gvl(void *(*func)(void *data), void* data1,
                                  rb_unblock_function_t ubf, void* ubf_data) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    State* state = env->state();
    void* ret = NULL;

    if(ubf == RUBY_UBF_IO || ubf == RUBY_UBF_PROCESS) {
      state->vm()->interrupt_with_signal();
    } else {
      state->vm()->wait_on_custom_function(ubf, ubf_data);
    }
    LEAVE_CAPI(env->state());
    {
      GCIndependent guard(env);
      ret = (*func)(data1);
    }
    ENTER_CAPI(env->state());
    state->vm()->clear_waiter();

    return ret;
  }

  // THAR BE EVEN MORE DRAGONS.
  //
  // When venturing through the valleys of the unmanaged, our hero must
  // remain vigilant and disciplined! If she should ever find a VALUE for
  // a reference in her travels: Look away! For these VALUEs are pure
  // death! Our hero must steel herself and continue on her quest, returning
  // as soon as possible to the castle of the managed.
  void* rb_thread_call_without_gvl2(void *(*func)(void *data), void* data1,
                                   rb_unblock_function_t ubf, void* ubf_data) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    State* state = env->state();
    void* ret = NULL;

    if(!state->check_async(env->current_call_frame())) {
      return ret;
    }
    if(ubf == RUBY_UBF_IO || ubf == RUBY_UBF_PROCESS) {
      state->vm()->interrupt_with_signal();
    } else {
      state->vm()->wait_on_custom_function(ubf, ubf_data);
    }
    LEAVE_CAPI(env->state());
    {
      GCIndependent guard(env);
      ret = (*func)(data1);
    }
    ENTER_CAPI(env->state());
    state->vm()->clear_waiter();

    return ret;
  }

  // More Experimental API support. This is the useful analog to the above
  // function, allowing you to selecting reaquire the GIL and do some work.

  void* rb_thread_call_with_gvl(void* (*func)(void*), void* data) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    GCTokenImpl gct;
    State* state = env->state();
    ENTER_CAPI(state);
    state->gc_dependent(gct, state->vm()->saved_call_frame());

    void* ret = (*func)(data);

    env->state()->gc_independent(gct, env->current_call_frame());
    LEAVE_CAPI(env->state());

    return ret;
  }

  Object* run_function(STATE) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Thread* self = state->vm()->thread.get();
    NativeMethod* nm = as<NativeMethod>(self->locals_aref(state, state->symbol("function")));
    Pointer* ptr = as<Pointer>(self->locals_aref(state, state->symbol("argument")));

    self->locals_remove(state, state->symbol("function"));
    self->locals_remove(state, state->symbol("argument"));

    NativeMethodFrame nmf(env, 0, nm);
    CallFrame cf;
    cf.previous = 0;
    cf.constant_scope_ = 0;
    cf.dispatch_data = (void*)&nmf;
    cf.compiled_code = 0;
    cf.flags = CallFrame::cNativeMethod;
    cf.optional_jit_data = 0;
    cf.top_scope_ = 0;
    cf.scope = 0;
    cf.arguments = 0;

    CallFrame* saved_frame = env->current_call_frame();
    env->set_current_call_frame(&cf);
    env->set_current_native_frame(&nmf);

    nmf.setup(
        env->get_handle(self),
        env->get_handle(cNil),
        env->get_handle(nm),
        env->get_handle(nm->module()));

    ENTER_CAPI(state);

    Object* ret = NULL;

    ExceptionPoint ep(env);

    PLACE_EXCEPTION_POINT(ep);

    if(unlikely(ep.jumped_to())) {
      // Setup exception in thread so it's raised when joining
      // Reload self because it might have been moved
      self = state->vm()->thread.get();
      Exception* exc = capi::c_as<Exception>(self->current_exception(state));
      self->exception(state, exc);
      return NULL;
    } else {
      ret = env->get_object(nm->func()(ptr->pointer));
    }

    LEAVE_CAPI(state);

    env->set_current_call_frame(saved_frame);
    env->set_current_native_frame(nmf.previous());
    ep.pop(env);

    return ret;
  }

  VALUE capi_thread_create(VALUE (*func)(ANYARGS), void* arg, const char* name, const char* file) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    State* state = env->state();

    NativeMethod* nm = NativeMethod::create(state,
                        String::create(state, file), G(thread),
                        state->symbol(name), (void*)func,
                        Fixnum::from(1), 0);

    Pointer* ptr = Pointer::create(state, arg);

    VM* vm = state->shared().new_vm();
    Thread* thr = Thread::create(env->state(), vm, G(thread), run_function);
    vm->thread.set(thr);

    thr->locals_store(state, state->symbol("function"), nm);
    thr->locals_store(state, state->symbol("argument"), ptr);

    VALUE thr_handle = env->get_handle(thr);
    thr->fork(state);
    return thr_handle;
  }

}
