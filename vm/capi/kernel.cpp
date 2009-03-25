#include "builtin/class.hpp"
#include "builtin/exception.hpp"

#include "exception.hpp"
#include "exception_point.hpp"

#include "capi/capi.hpp"
#include "capi/ruby.h"

using namespace rubinius;

extern "C" {
  VALUE rb_yield(VALUE argument_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    if (!rb_block_given_p()) {
      rb_raise(rb_eLocalJumpError, "no block given", 0);
    }

    VALUE block_handle = env->get_handle(env->block());

    return rb_funcall(block_handle, rb_intern("call"), 1, argument_handle);
  }

  int rb_block_given_p() {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    return RBX_RTEST(env->block());
  }

#define RB_RAISE_BUFSIZE   256

  void rb_raise(VALUE error_handle, const char* format_string, ...) {
    va_list args;
    char reason[RB_RAISE_BUFSIZE];

    va_start(args, format_string);
    vsnprintf(reason, RB_RAISE_BUFSIZE, format_string, args);
    va_end(args);

    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    Exception* exc = Exception::make_exception(
          env->state(), as<Class>(env->get_object(error_handle)), reason);
    env->state()->thread_state()->raise_exception(exc);

    env->current_ep()->return_to(env);
  }

  VALUE rb_require(const char* name) {
    return rb_funcall(rb_mKernel, rb_intern("require"), 1, rb_str_new2(name));
  }
}
