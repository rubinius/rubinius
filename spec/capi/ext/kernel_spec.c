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

#ifdef HAVE_RB_BLOCK_PROC
VALUE kernel_spec_rb_block_proc(VALUE self) {
  return rb_block_proc();
}
#endif

#ifdef HAVE_RB_ENSURE
VALUE kernel_spec_rb_ensure(VALUE self, VALUE main_proc, VALUE arg,
                            VALUE ensure_proc, VALUE arg2) {
  VALUE main_array = rb_ary_new();
  rb_ary_push(main_array, main_proc);
  rb_ary_push(main_array, arg);

  VALUE ensure_array = rb_ary_new();
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

#ifdef HAVE_RB_RESCUE
VALUE kernel_spec_rb_rescue(VALUE self, VALUE main_proc, VALUE arg,
                            VALUE raise_proc, VALUE arg2) {
  VALUE main_array = rb_ary_new();
  rb_ary_push(main_array, main_proc);
  rb_ary_push(main_array, arg);

  VALUE raise_array = rb_ary_new();
  rb_ary_push(raise_array, raise_proc);
  rb_ary_push(raise_array, arg2);

  return rb_rescue(kernel_spec_call_proc, main_array,
      kernel_spec_call_proc, raise_array);
}
#endif

#ifdef HAVE_RB_RESCUE2
VALUE kernel_spec_rb_rescue2(int argc, VALUE *args, VALUE self) {
  VALUE main_array = rb_ary_new();
  rb_ary_push(main_array, args[0]);
  rb_ary_push(main_array, args[1]);

  VALUE raise_array = rb_ary_new();
  rb_ary_push(raise_array, args[2]);
  rb_ary_push(raise_array, args[3]);

  return rb_rescue2(kernel_spec_call_proc, main_array,
      kernel_spec_call_proc, raise_array, args[4], args[5], (VALUE)0);
}
#endif

#ifdef HAVE_RB_SYS_FAIL
VALUE kernel_spec_rb_sys_fail(VALUE self, VALUE msg) {
  errno = 1;
  rb_sys_fail(StringValuePtr(msg));
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

void Init_kernel_spec() {
  VALUE cls;
  cls = rb_define_class("CApiKernelSpecs", rb_cObject);

#ifdef HAVE_RB_BLOCK_GIVEN_P
  rb_define_method(cls, "rb_block_given_p", kernel_spec_rb_block_given_p, 0);
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
}

#ifdef __cplusplus
}
#endif
