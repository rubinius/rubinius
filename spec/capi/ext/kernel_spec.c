#include <errno.h>
#include "ruby.h"

VALUE kernel_spec_rb_raise(VALUE self, VALUE hash) {
  rb_hash_aset(hash, ID2SYM(rb_intern("stage")), ID2SYM(rb_intern("before")));
  rb_raise(rb_eTypeError, "Wrong argument type %s (expected %s)", "Integer", "String");
  rb_hash_aset(hash, ID2SYM(rb_intern("stage")), ID2SYM(rb_intern("after")));
  return Qnil;
}

VALUE kernel_spec_rb_warn(VALUE self, VALUE msg) {
  rb_warn("%s", StringValuePtr(msg));
  return Qnil;
}

VALUE kernel_spec_rb_sys_fail(VALUE self, VALUE msg) {
  errno = 1;
  rb_sys_fail(StringValuePtr(msg));
  return Qnil;
}

void Init_kernel_spec() {
  VALUE cls;
  cls = rb_define_class("CApiKernelSpecs", rb_cObject);
  rb_define_method(cls, "rb_raise", kernel_spec_rb_raise, 1);
  rb_define_method(cls, "rb_warn", kernel_spec_rb_warn, 1);
  rb_define_method(cls, "rb_sys_fail", kernel_spec_rb_sys_fail, 1);
}
