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
}
