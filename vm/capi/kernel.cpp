#include "builtin/exception.hpp"
#include "builtin/array.hpp"

#include "exception_point.hpp"

#include "capi/capi.hpp"
#include "capi/include/ruby.h"

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
    env->flush_cached_data();

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
    capi::capi_raise_backend(exc);

    printf("rb_raise broken!\n");
    exit(1);
  }

  VALUE rb_rescue2(VALUE (*func)(ANYARGS), VALUE arg1,
                   VALUE (*raise_func)(ANYARGS), VALUE arg2, ...)
  {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    VALUE ret = Qnil;
    ExceptionPoint ep(env);
    va_list exc_classes;

    PLACE_EXCEPTION_POINT(ep);
    if(unlikely(ep.jumped_to())) {
      ep.pop(env);

      if(env->state()->thread_state()->raise_reason() != cException) {
        //Something bad happened, we shouldn't be here.
        return Qnil;
      }

      VALUE exc_handle = env->get_handle(env->state()->thread_state()->current_exception());
      bool handle_exc = false;

      va_start(exc_classes, arg2);
      while(VALUE eclass = va_arg(exc_classes, VALUE)) {
        if(rb_obj_is_kind_of(exc_handle, eclass) == Qtrue) {
          handle_exc = true;
          break;
        }
      }
      va_end(exc_classes);

      if(handle_exc) {
        ret = (*raise_func)(arg2, exc_handle);
        env->state()->thread_state()->clear_raise();
      } else {
        env->current_ep()->return_to(env);
      }

    } else {
      ret = (*func)(arg1);
      ep.pop(env);
    }

    return ret;
  }

  VALUE rb_rescue(VALUE (*func)(ANYARGS), VALUE arg1,
                  VALUE (*raise_func)(ANYARGS), VALUE arg2)
  {
    // Sending 0 as the last argument is an ugly hack, but we have to mimic MRI, so...
    return rb_rescue2(func, arg1, raise_func, arg2, rb_eStandardError,  0);
  }

  VALUE rb_protect(VALUE (*func)(VALUE), VALUE data, int* status) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    VALUE ret = Qnil;

    ExceptionPoint ep(env);
    PLACE_EXCEPTION_POINT(ep);

    if(unlikely(ep.jumped_to())) {
      if(status) *status = 1;
    } else {
      ret = (*func)(data);
      if(status) *status = 0;
    }

    ep.pop(env);

    return ret;
  }

  void rb_jump_tag(int status) {
    if(!status) return;

    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    // TODO should check if there is an ep?
    env->current_ep()->return_to(env);
  }

  VALUE rb_ensure(VALUE (*func)(ANYARGS), VALUE arg1,
                  VALUE (*ensure_func)(ANYARGS), VALUE arg2)
  {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    VALUE ret = Qnil;
    bool exc_raised = false;

    ExceptionPoint ep(env);
    PLACE_EXCEPTION_POINT(ep);

    if(unlikely(ep.jumped_to())) {
      exc_raised = true;
    } else {
      ret = (*func)(arg1);
    }

    ep.pop(env);
    (*ensure_func)(arg2);

    if(exc_raised) {
      env->current_ep()->return_to(env);
    }

    return ret;
  }

  void rb_p(VALUE obj) {
    rb_funcall(rb_mKernel, rb_intern("p"), 1, obj);
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

  void rb_notimplement() {
    rb_fatal("not implemented");
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

  VALUE rb_eval_string(const char* str) {
    return rb_funcall(rb_mKernel, rb_intern("eval"), 1, rb_str_new2(str));
  }

  VALUE rb_block_proc() {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    return rb_funcall(rb_cProc, rb_intern("__from_block__"), 1, env->get_handle(env->block()));
  }
}
