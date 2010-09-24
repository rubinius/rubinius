#include "capi/capi.hpp"
#include "capi/include/ruby.h"
#include "builtin/thread.hpp"

/*
namespace rubinius {
  class UnblockFuncWaiter : public Waiter {
    rb_unblock_function_t* ubf_;
    void* ubf_data_;

  public:
    UnblockFuncWaiter(rb_unblock_function_t* ubf, void* ubf_data)
      : ubf_(ubf)
      , ubf_data_(ubf_data)
    {}

    void wakeup() {
      if (ubf_ != NULL) {
        (*ubf_)(ubf_data_);
      }
    }
  };
}
*/

using namespace rubinius;

extern "C" {

  int rb_thread_critical = 0;

  void rb_thread_schedule() {
    rb_funcall2(rb_cThread, rb_intern("pass"), 0, NULL);
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
      env->state()->shared.leave_capi(env->state());
      {
        GCIndependent guard(env);
        ret = select(max, read, write, except, tvp);
      }
      env->state()->shared.enter_capi(env->state());

      if(!env->state()->check_async(env->current_call_frame())) {
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
    Thread* thread = env->state()->thread.get();

    return env->get_handle(thread);
  }


  int rb_thread_alone() {
    return 0;
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
  // remain vigilant and disiplined! If she should ever find a VALUE for
  // a reference in her travels: Look away! For these VALUEs are pure
  // death! Our hero must steel herself and continue on her quest, returning
  // as soon as possible to the castle of the managed.
  VALUE rb_thread_blocking_region(rb_blocking_function_t func, void* data,
                                  rb_unblock_function_t ubf, void* ubf_data) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    VM* state = env->state();
    // UnblockFuncWaiter waiter(ubf, ubf_data);
    VALUE ret = Qnil;

    if (ubf == RUBY_UBF_IO || ubf == RUBY_UBF_PROCESS) {
      state->interrupt_with_signal();
    } else {
      //state->install_waiter(waiter);
    }
    env->state()->shared.leave_capi(env->state());
    {
      GCIndependent guard(env);
      ret = (*func)(data);
    }
    env->state()->shared.enter_capi(env->state());
    // state->clear_waiter();

    return ret;
  }
}
