#include <errno.h>

#include "ruby.h"
#include "rubyspec.h"

#ifdef __cplusplus
extern "C" {
#endif

VALUE kernel_spec_call_proc(VALUE arg_array) {
  VALUE arg = rb_ary_pop(arg_array);
  VALUE proc = rb_ary_pop(arg_array);
  return rb_funcall(proc, rb_intern("call"), 1, arg);
}

#ifdef HAVE_RB_BLOCK_GIVEN_P
static VALUE kernel_spec_rb_block_given_p(VALUE self) {
  return rb_block_given_p() ? Qtrue : Qfalse;
}
#endif

#ifdef HAVE_RB_NEED_BLOCK
VALUE kernel_spec_rb_need_block(VALUE self) {
  rb_need_block();
  return Qnil;
}
#endif

#ifdef HAVE_RB_BLOCK_PROC
VALUE kernel_spec_rb_block_proc(VALUE self) {
  return rb_block_proc();
}
#endif

#ifdef HAVE_RB_ENSURE
VALUE kernel_spec_rb_ensure(VALUE self, VALUE main_proc, VALUE arg,
                            VALUE ensure_proc, VALUE arg2) {
  VALUE main_array, ensure_array;

  main_array = rb_ary_new();
  rb_ary_push(main_array, main_proc);
  rb_ary_push(main_array, arg);

  ensure_array = rb_ary_new();
  rb_ary_push(ensure_array, ensure_proc);
  rb_ary_push(ensure_array, arg2);

  return rb_ensure(kernel_spec_call_proc, main_array,
      kernel_spec_call_proc, ensure_array);
}
#endif

#ifdef HAVE_RB_EVAL_STRING
VALUE kernel_spec_rb_eval_string(VALUE self, VALUE str) {
  return rb_eval_string(RSTRING_PTR(str));
}
#endif

#ifdef HAVE_RB_RAISE
VALUE kernel_spec_rb_raise(VALUE self, VALUE hash) {
  rb_hash_aset(hash, ID2SYM(rb_intern("stage")), ID2SYM(rb_intern("before")));
  rb_raise(rb_eTypeError, "Wrong argument type %s (expected %s)", "Integer", "String");
  rb_hash_aset(hash, ID2SYM(rb_intern("stage")), ID2SYM(rb_intern("after")));
  return Qnil;
}
#endif

#ifdef HAVE_RB_THROW
VALUE kernel_spec_rb_throw(VALUE self, VALUE result) {
  rb_throw("foo", result);
  return ID2SYM(rb_intern("rb_throw_failed"));
}
#endif

#ifdef HAVE_RB_RESCUE
VALUE kernel_spec_call_proc_with_raised_exc(VALUE arg_array, VALUE raised_exc) {
  VALUE argv[2];
  int argc;

  VALUE arg = rb_ary_pop(arg_array);
  VALUE proc = rb_ary_pop(arg_array);

  argv[0] = arg;
  argv[1] = raised_exc;

  argc = 2;

  return rb_funcall2(proc, rb_intern("call"), argc, argv);
}

VALUE kernel_spec_rb_rescue(VALUE self, VALUE main_proc, VALUE arg,
                            VALUE raise_proc, VALUE arg2) {
  VALUE main_array, raise_array;

  main_array = rb_ary_new();
  rb_ary_push(main_array, main_proc);
  rb_ary_push(main_array, arg);

  raise_array = rb_ary_new();
  rb_ary_push(raise_array, raise_proc);
  rb_ary_push(raise_array, arg2);

  return rb_rescue(kernel_spec_call_proc, main_array,
      kernel_spec_call_proc_with_raised_exc, raise_array);
}
#endif

#ifdef HAVE_RB_RESCUE2
VALUE kernel_spec_rb_rescue2(int argc, VALUE *args, VALUE self) {
  VALUE main_array, raise_array;

  main_array = rb_ary_new();
  rb_ary_push(main_array, args[0]);
  rb_ary_push(main_array, args[1]);

  raise_array = rb_ary_new();
  rb_ary_push(raise_array, args[2]);
  rb_ary_push(raise_array, args[3]);

  return rb_rescue2(kernel_spec_call_proc, main_array,
      kernel_spec_call_proc, raise_array, args[4], args[5], (VALUE)0);
}
#endif

#ifdef HAVE_RB_SYS_FAIL
VALUE kernel_spec_rb_sys_fail(VALUE self, VALUE msg) {
  errno = 1;
  if(msg == Qnil) {
    rb_sys_fail(0);
  } else {
    rb_sys_fail(StringValuePtr(msg));
  }
  return Qnil;
}
#endif

#ifdef HAVE_RB_WARN
VALUE kernel_spec_rb_warn(VALUE self, VALUE msg) {
  rb_warn("%s", StringValuePtr(msg));
  return Qnil;
}
#endif

#ifdef HAVE_RB_YIELD
static VALUE kernel_spec_rb_yield(VALUE self, VALUE obj) {
  return rb_yield(obj);
}
#endif

#ifdef HAVE_RB_YIELD_SPLAT
static VALUE kernel_spec_rb_yield_splat(VALUE self, VALUE ary) {
  return rb_yield_splat(ary);
}
#endif

#ifdef HAVE_RB_YIELD_VALUES
static VALUE kernel_spec_rb_yield_values(VALUE self, VALUE obj1, VALUE obj2) {
  return rb_yield_values(2, obj1, obj2);
}
#endif

#ifdef HAVE_RB_EXEC_RECURSIVE
static VALUE do_rec(VALUE obj, VALUE arg, int is_rec) {
  if(is_rec) {
    return obj;
  } else if(arg == Qtrue) {
    return rb_exec_recursive(do_rec, obj, Qnil);
  } else {
    return Qnil;
  }
}

static VALUE kernel_spec_rb_exec_recursive(VALUE self, VALUE obj) {
  return rb_exec_recursive(do_rec, obj, Qtrue);
}
#endif

#ifdef HAVE_RB_SET_END_PROC
static void write_io(VALUE io) {
  rb_funcall(io, rb_intern("write"), 1, rb_str_new2("e"));
}

static VALUE kernel_spec_rb_set_end_proc(VALUE self, VALUE io) {
  rb_set_end_proc(write_io, io);
  return Qnil;
}
#endif

#ifdef HAVE_RB_F_SPRINTF
static VALUE kernel_spec_rb_f_sprintf(VALUE self, VALUE ary) {
  return rb_f_sprintf(RARRAY_LEN(ary), RARRAY_PTR(ary));
}
#endif

void Init_kernel_spec() {
  VALUE cls;
  cls = rb_define_class("CApiKernelSpecs", rb_cObject);

#ifdef HAVE_RB_BLOCK_GIVEN_P
  rb_define_method(cls, "rb_block_given_p", kernel_spec_rb_block_given_p, 0);
#endif

#ifdef HAVE_RB_NEED_BLOCK
  rb_define_method(cls, "rb_need_block", kernel_spec_rb_need_block, 0);
#endif

#ifdef HAVE_RB_BLOCK_PROC
  rb_define_method(cls, "rb_block_proc", kernel_spec_rb_block_proc, 0);
#endif

#ifdef HAVE_RB_ENSURE
  rb_define_method(cls, "rb_ensure", kernel_spec_rb_ensure, 4);
#endif

#ifdef HAVE_RB_EVAL_STRING
  rb_define_method(cls, "rb_eval_string", kernel_spec_rb_eval_string, 1);
#endif

#ifdef HAVE_RB_RAISE
  rb_define_method(cls, "rb_raise", kernel_spec_rb_raise, 1);
#endif

#ifdef HAVE_RB_THROW
  rb_define_method(cls, "rb_throw", kernel_spec_rb_throw, 1);
#endif

#ifdef HAVE_RB_RESCUE
  rb_define_method(cls, "rb_rescue", kernel_spec_rb_rescue, 4);
#endif

#ifdef HAVE_RB_RESCUE2
  rb_define_method(cls, "rb_rescue2", kernel_spec_rb_rescue2, -1);
#endif

#ifdef HAVE_RB_SYS_FAIL
  rb_define_method(cls, "rb_sys_fail", kernel_spec_rb_sys_fail, 1);
#endif

#ifdef HAVE_RB_WARN
  rb_define_method(cls, "rb_warn", kernel_spec_rb_warn, 1);
#endif

#ifdef HAVE_RB_YIELD
  rb_define_method(cls, "rb_yield", kernel_spec_rb_yield, 1);
#endif

#ifdef HAVE_RB_YIELD_VALUES
  rb_define_method(cls, "rb_yield_values", kernel_spec_rb_yield_values, 2);
#endif

#ifdef HAVE_RB_YIELD_SPLAT
  rb_define_method(cls, "rb_yield_splat", kernel_spec_rb_yield_splat, 1);
#endif

#ifdef HAVE_RB_EXEC_RECURSIVE
  rb_define_method(cls, "rb_exec_recursive", kernel_spec_rb_exec_recursive, 1);
#endif

#ifdef HAVE_RB_SET_END_PROC
  rb_define_method(cls, "rb_set_end_proc", kernel_spec_rb_set_end_proc, 1);
#endif

#ifdef HAVE_RB_F_SPRINTF
  rb_define_method(cls, "rb_f_sprintf", kernel_spec_rb_f_sprintf, 1);
#endif
}

#ifdef __cplusplus
}
#endif
