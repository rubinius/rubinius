#include <ruby.h>

void sm_define_const(VALUE self, VALUE klass, VALUE val) {
  rb_define_const(klass, "FOO", val);
}

void Init_subtend_module() {
  VALUE cls;
  cls = rb_define_class("SubtendModule", rb_cObject);
  rb_define_method(cls, "rb_define_const", sm_define_const, 2);
}