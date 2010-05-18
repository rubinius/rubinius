#include "capi/capi.hpp"
#include "capi/ruby.h"
#include "builtin/thread.hpp"

using namespace rubinius;

extern "C" {
  void rb_thread_schedule() {
    rb_funcall2(rb_cThread, rb_intern("pass"), 0, NULL);
  }

  int rb_thread_select(int max, fd_set* read, fd_set* write, fd_set* except,
                       struct timeval *timeval) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    GlobalLock::UnlockGuard guard(env->state()->global_lock());

    return select(max, read, write, except, timeval);
  }

  VALUE rb_thread_current(void) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    GlobalLock::UnlockGuard guard(env->state()->global_lock());
    Thread* thread = env->state()->thread.get();
    
    return env->get_handle(thread);
  }
  

  int rb_thread_alone() {
    return 0;
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
    VALUE ret = Qnil;

    // ubf is ignored entirely.
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    {
      GlobalLock::UnlockGuard guard(env->state()->global_lock());
      ret = (*func)(data);
    }

    return ret;
  }
}
