#include "builtin/class.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/exception.hpp"
#include "builtin/staticscope.hpp"
#include "builtin/system.hpp"
#include "builtin/array.hpp"

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

  VALUE rb_apply(VALUE recv, ID mid, VALUE args) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    env->flush_cached_data(false);

    Array* ary = capi::c_as<Array>(env->get_object(args));

    Object* obj = env->get_object(recv);
    Object* ret = obj->send(env->state(), env->current_call_frame(),
        reinterpret_cast<Symbol*>(mid), ary, RBX_Qnil);
    env->update_cached_data();

    // An exception occurred
    if(!ret) env->current_ep()->return_to(env);

    return env->get_handle(ret);
  }

#define RB_EXC_BUFSIZE   256

  void rb_raise(VALUE error_handle, const char* format_string, ...) {
    va_list args;
    char reason[RB_EXC_BUFSIZE];

    va_start(args, format_string);
    vsnprintf(reason, RB_EXC_BUFSIZE, format_string, args);
    va_end(args);

    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    Exception* exc = Exception::make_exception(
          env->state(), as<Class>(env->get_object(error_handle)), reason);
    exc->locations(env->state(), System::vm_backtrace(env->state(),
          Fixnum::from(0), env->current_call_frame()));
    env->state()->thread_state()->raise_exception(exc);

    env->current_ep()->return_to(env);
  }

  /* @note  It is of dubious correctness that any C extension uses
   * rb_bug, but some do. We basically mimic MRI behavior here.
   */
  void rb_bug(const char *fmt, ...) {
    va_list args;

    fputs("[BUG]", stderr);
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    rubinius::abort();
  }

  void rb_fatal(const char *fmt, ...) {
    va_list args;
    char msg[RB_EXC_BUFSIZE];

    va_start(args, fmt);
    vsnprintf(msg, RB_EXC_BUFSIZE, fmt, args);
    va_end(args);

    rb_raise(rb_eFatal, msg);
  }

  /* rb_warn and rb_warning don't factor out creating the string for the
   * message because it is just as much work preparing the va_list to
   * pass to another variadic function.
   */
  void rb_warn(const char *fmt, ...) {
    va_list args;
    char msg[RB_EXC_BUFSIZE];

    va_start(args, fmt);
    vsnprintf(msg, RB_EXC_BUFSIZE, fmt, args);
    va_end(args);

    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    String* string = String::create(env->state(), msg);
    rb_funcall(rb_mKernel, rb_intern("warn"), 1, env->get_handle(string));
  }

  void rb_warning(const char* fmt, ...) {
    va_list args;
    char msg[RB_EXC_BUFSIZE];

    va_start(args, fmt);
    vsnprintf(msg, RB_EXC_BUFSIZE, fmt, args);
    va_end(args);

    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    String* string = String::create(env->state(), msg);
    rb_funcall(rb_mKernel, rb_intern("warning"), 1, env->get_handle(string));
  }

  VALUE rb_require(const char* name) {
    return rb_funcall(rb_mKernel, rb_intern("require"), 1, rb_str_new2(name));
  }

  void rb_sys_fail(const char* mesg) {
    rb_funcall(rb_mErrno, rb_intern("handle"), 1, rb_str_new2(mesg));
  }
}
