#include <ruby.h>

static VALUE smv_test(VALUE self) {
  return rb_str_new2("test");
}

VALUE smv_undef_method(VALUE self, VALUE klass, VALUE name) {
  rb_undef_method(klass, STR2CSTR(name));
  return Qnil;
}

void Init_subtend_define_method_vis() {
  VALUE cls, mod;
  cls = rb_define_class("SubtendMethodVis", rb_cObject);
  mod = rb_define_module("SubtendMethodVisModule");

  rb_define_method(cls, "smv_test_public", smv_test, 0);
  rb_define_protected_method(cls, "smv_test_protected", smv_test, 0);
  rb_define_private_method(cls, "smv_test_private", smv_test, 0);

  rb_define_singleton_method(mod, "smv_test_singleton", smv_test, 0);
  rb_define_module_function(mod, "smv_test_module_function", smv_test, 0);

  rb_define_global_function("smv_test_global_function", smv_test, 0);

  rb_define_method(cls, "rb_undef_method", smv_undef_method, 2);
}
