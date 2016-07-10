#include "config.h"

#ifndef RBX_WINDOWS
#include <sys/select.h>
#endif

#include "capi/capi.hpp"
#include "capi/ruby.h"

#include "on_stack.hpp"
#include "call_frame.hpp"
#include "exception_point.hpp"
#include "thread_phase.hpp"

#include "builtin/exception.hpp"
#include "builtin/thread.hpp"
#include "builtin/native_method.hpp"
#include "builtin/ffi_pointer.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "builtin/thread.hpp"

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
        UnmanagedPhase unmanaged(env->state());
        ret = select(max, read, write, except, tvp);
      }

      bool ok = !env->state()->vm()->thread_interrupted_p(env->state());

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
    Thread* thread = env->state()->vm()->thread();

    return env->get_handle(thread);
  }


  int rb_thread_alone() {
    VALUE list = rb_funcall(rb_cThread, rb_intern("list"), 0);
    // This is int as bool, so only return 1 if the number of threads
    // is actually 1.
    return NUM2INT(rb_funcall(list, rb_intern("size"), 0)) == 1 ? 1 : 0;
  }

  VALUE rb_thread_local_aref(VALUE thread, ID id) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    Thread* thr = capi::c_as<Thread>(env->get_object(thread));
    return env->get_handle(
        thr->fiber_variable_get(env->state(), reinterpret_cast<Symbol*>(id)));
  }

  VALUE rb_thread_local_aset(VALUE thread, ID id, VALUE value) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    Thread* thr = capi::c_as<Thread>(env->get_object(thread));
    return env->get_handle(
        thr->fiber_variable_set(
          env->state(), reinterpret_cast<Symbol*>(id), env->get_object(value)));
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
      UnmanagedPhase unmanaged(env->state());
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
      UnmanagedPhase unmanaged(env->state());
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

    if(state->vm()->thread_interrupted_p(state)) {
      return ret;
    }
    if(ubf == RUBY_UBF_IO || ubf == RUBY_UBF_PROCESS) {
      state->vm()->interrupt_with_signal();
    } else {
      state->vm()->wait_on_custom_function(ubf, ubf_data);
    }
    LEAVE_CAPI(env->state());
    {
      UnmanagedPhase unmanaged(env->state());
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

    State* state = env->state();
    ENTER_CAPI(state);
    state->vm()->managed_phase(state);

    void* ret = (*func)(data);

    env->state()->vm()->unmanaged_phase(state);
    LEAVE_CAPI(env->state());

    return ret;
  }

  Object* run_function(STATE) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Thread* thread = state->vm()->thread();

    NativeMethod* nm = capi::c_as<NativeMethod>(
        thread->variable_get(state, state->symbol("function")));
    Pointer* ptr = capi::c_as<Pointer>(
        thread->variable_get(state, state->symbol("argument")));

    NativeMethodFrame nmf(env, 0, nm);
    CallFrame call_frame;
    call_frame.previous = NULL;
    call_frame.constant_scope_ = 0;
    call_frame.dispatch_data = (void*)&nmf;
    call_frame.compiled_code = 0;
    call_frame.flags = CallFrame::cNativeMethod;
    call_frame.top_scope_ = 0;
    call_frame.scope = 0;
    call_frame.arguments = 0;

    env->set_current_call_frame(&call_frame);
    env->set_current_native_frame(&nmf);

    state->vm()->set_call_frame(&call_frame);

    nmf.setup(
        env->get_handle(thread),
        env->get_handle(cNil),
        env->get_handle(nm),
        env->get_handle(nm->module()));

    ENTER_CAPI(state);

    Object* value = NULL;

    ExceptionPoint ep(env);

    PLACE_EXCEPTION_POINT(ep);

    if(unlikely(ep.jumped_to())) {
      LEAVE_CAPI(state);

      // Set exception in thread so it's raised when joining.
      state->vm()->thread()->exception(state,
          capi::c_as<Exception>(state->vm()->thread_state()->current_exception()));
    } else {
      value = env->get_object(nm->func()(ptr->pointer));
    }

    LEAVE_CAPI(state);

    env->set_current_call_frame(NULL);
    env->set_current_native_frame(NULL);
    ep.pop(env);

    return value;
  }

  VALUE capi_thread_create(VALUE (*func)(ANYARGS), void* arg, const char* name, const char* file) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    State* state = env->state();

    NativeMethod* nm = NativeMethod::create(state,
                        String::create(state, file), G(thread),
                        state->symbol(name), (void*)func,
                        Fixnum::from(1), 0);

    Pointer* ptr = Pointer::create(state, arg);

    Thread* thr = Thread::create(env->state(), G(thread), run_function);

    thr->variable_set(state, state->symbol("function"), nm);
    thr->variable_set(state, state->symbol("argument"), ptr);

    thr->group(state, state->vm()->thread()->group());

    VALUE thr_handle = env->get_handle(thr);
    thr->fork(state);

    return thr_handle;
  }
}
