#include "builtin/class.hpp"
#include "builtin/exception.hpp"
#include "builtin/proc.hpp"
#include "builtin/string.hpp"

#include "exception_point.hpp"

#include "capi/capi.hpp"
#include "capi/ruby.h"

using namespace rubinius;

extern "C" {
#define RB_EXC_BUFSIZE   256

  void rb_raise(VALUE error_handle, const char* format_string, ...) {
    va_list args;
    char reason[RB_EXC_BUFSIZE];

    va_start(args, format_string);
    vsnprintf(reason, RB_EXC_BUFSIZE, format_string, args);
    va_end(args);

    capi::capi_raise_backend(error_handle, reason);

    rubinius::bug("rb_raise failed");
    exit(1);  // compiler snack.
  }

  void rb_throw_obj(VALUE obj, VALUE result) {
    rb_funcall(rb_mKernel, rb_intern("throw"), 2, obj, result);
    rubinius::bug("rb_throw failed");
    exit(1);  // compiler snack.
  }

  void rb_throw(const char* symbol, VALUE result) {
    rb_throw_obj(ID2SYM(rb_intern(symbol)), result);
    exit(1);  // compiler snack.
  }

  VALUE rb_catch_obj(VALUE tag, VALUE (*func)(ANYARGS), VALUE data) {
    VALUE proc = rb_proc_new(func, data);
    return rb_funcall2b(rb_mKernel, rb_intern("catch"), 1, &tag, proc);
  }

  VALUE rb_catch(const char* tag, VALUE(*func)(ANYARGS), VALUE data) {
    return rb_catch_obj(ID2SYM(rb_intern(tag)), func, data);
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
        if(RTEST(rb_obj_is_kind_of(exc_handle, eclass))) {
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
    exit(1);  // compiler snack.
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

  // TODO: Since in 1.9 #respond_to? returns false if the MRI version of this
  // method is in a method table, we'll probably need to get the Rubinius
  // special version of this method and call it rather than just raising here.
  VALUE rb_f_notimplement(int argc, VALUE *argv, VALUE obj) {
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
    if(!mesg) mesg = "system failure";
    rb_funcall(rb_mErrno, rb_intern("handle"), 1, rb_str_new2(mesg));

    rubinius::bug("rb_sys_fail failed");
    exit(1);  // compiler snack.
  }

  void rb_syserr_fail(int err, const char* mesg) {
    if(!mesg) mesg = "system failure";

    VALUE exc = rb_funcall(rb_eSystemCallError, rb_intern("new"), 2, rb_str_new2(mesg), INT2NUM(err));
    rb_exc_raise(exc);
    rubinius::bug("rb_syserr_fail failed");
    exit(1);  // compiler snack.
  }

  VALUE rb_eval_string(const char* str) {
    return rb_funcall(rb_mKernel, rb_intern("eval"), 1, rb_str_new2(str));
  }

  VALUE rb_block_proc() {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    return rb_funcall(rb_mCAPI, rb_intern("rb_block_proc"), 1,
        env->get_handle(env->block()));
  }

  // Hoisted from 1.8.7

  static VALUE recursive_check(VALUE hash, VALUE obj) {
    if(NIL_P(hash) || TYPE(hash) != T_HASH) {
      return Qfalse;
    } else {
      VALUE list = rb_hash_aref(hash, ID2SYM(rb_frame_last_func()));

      if(NIL_P(list) || TYPE(list) != T_HASH) return Qfalse;
      if(NIL_P(rb_hash_lookup(list, obj))) return Qfalse;
      return Qtrue;
    }
  }

  static VALUE recursive_push(VALUE hash, VALUE obj) {
    VALUE list, sym;

    sym = ID2SYM(rb_frame_last_func());

    if(NIL_P(hash) || TYPE(hash) != T_HASH) {
      hash = rb_hash_new();
      rb_thread_local_aset(rb_thread_current(),
                           rb_intern("__recursive_key"), hash);
      list = Qnil;
    } else {
      list = rb_hash_aref(hash, sym);
    }

    if(NIL_P(list) || TYPE(list) != T_HASH) {
      list = rb_hash_new();
      rb_hash_aset(hash, sym, list);
    }

    rb_hash_aset(list, obj, Qtrue);
    return hash;
  }

  static void recursive_pop(VALUE hash, VALUE obj) {
    VALUE list, sym;

    sym = ID2SYM(rb_frame_last_func());

    if(NIL_P(hash) || TYPE(hash) != T_HASH) {
      VALUE symname;
      VALUE thrname;
      symname = rb_inspect(sym);
      thrname = rb_inspect(rb_thread_current());

      rb_raise(rb_eTypeError, "invalid inspect_tbl hash for %s in %s",
          StringValuePtr(symname), StringValuePtr(thrname));
    }

    list = rb_hash_aref(hash, sym);

    if(NIL_P(list) || TYPE(list) != T_HASH) {
      VALUE symname = rb_inspect(sym);
      VALUE thrname = rb_inspect(rb_thread_current());
      rb_raise(rb_eTypeError, "invalid inspect_tbl list for %s in %s",
          StringValuePtr(symname), StringValuePtr(thrname));
    }
    rb_hash_delete(list, obj);
  }

  VALUE rb_exec_recursive(VALUE (*func)(VALUE, VALUE, int),
                          VALUE obj, VALUE arg)
  {
    VALUE hash = rb_thread_local_aref(rb_thread_current(),
                                      rb_intern("__recursive_key"));
    VALUE objid = rb_obj_id(obj);

    if(recursive_check(hash, objid)) {
      return (*func)(obj, arg, 1);
    } else {
      NativeMethodEnvironment* env = NativeMethodEnvironment::get();
      VALUE ret = Qnil;

      ExceptionPoint ep(env);
      PLACE_EXCEPTION_POINT(ep);

      bool unwinding = false;
      hash = recursive_push(hash, objid);

      if(unlikely(ep.jumped_to())) {
        unwinding = true;
      } else {
        ret = (*func)(obj, arg, 0);
      }

      ep.pop(env);

      recursive_pop(hash, objid);
      if(unwinding) env->current_ep()->return_to(env);
      return ret;
    }
  }

  void rb_set_end_proc(void* cb, VALUE cb_data) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    VALUE prc = env->get_handle(
        capi::wrap_c_function(cb, cb_data, C_CALLBACK));
    rb_funcall(rb_mKernel, rb_intern("at_exit"), 1, prc);
  }

  VALUE rb_f_sprintf(int argc, const VALUE* argv) {
    VALUE ary = rb_ary_new4(argc-1, argv+1);
    return rb_funcall(rb_mCAPI, rb_intern("sprintf"), 2, argv[0], ary);
  }

  VALUE rb_make_backtrace() {
    return rb_funcall(rb_mKernel, rb_intern("caller"), 0);
  }

  VALUE rb_obj_method(VALUE self, VALUE method) {
    return rb_funcall(self, rb_intern("method"), 1, method);
  }

}
