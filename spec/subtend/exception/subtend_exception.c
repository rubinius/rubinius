#include <ruby.h>
#include <stdio.h>

VALUE se_raise(VALUE self) {
  rb_raise(rb_eTypeError, "Wrong argument type %s (expected %s)", "Integer", "String");
  return Qnil;
}

void Init_subtend_exception() {
  VALUE cls;
  cls = rb_define_class("SubtendRaiser", rb_cObject);
  rb_define_method(cls, "raise!", se_raise, 0);
}
