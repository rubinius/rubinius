#include <ruby.h>

void sm_define_const(VALUE self, VALUE klass, VALUE val) {
  rb_define_const(klass, "FOO", val);
}

VALUE sm_const_defined(VALUE self, VALUE klass, VALUE id) {
  return rb_const_defined(klass, SYM2ID(id)) == 1 ? Qtrue : Qfalse;
}

void Init_subtend_module() {
  VALUE cls;
  cls = rb_define_class("SubtendModule", rb_cObject);
  rb_define_method(cls, "rb_define_const", sm_define_const, 2);
  rb_define_method(cls, "rb_const_defined", sm_const_defined, 2);  
}