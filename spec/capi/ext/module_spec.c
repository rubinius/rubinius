#include "ruby.h"

VALUE sm_define_const(VALUE self, VALUE klass, VALUE val) {
  rb_define_const(klass, "FOO", val);
  return Qnil;
}

VALUE sm_const_set(VALUE self, VALUE klass, VALUE val) {
  rb_const_set(klass, rb_intern("FOO"), val);
  return Qnil;
}

VALUE sm_const_get(VALUE self, VALUE klass, VALUE val) {
  return rb_const_get(klass, val);
}

VALUE sm_const_defined(VALUE self, VALUE klass, VALUE id) {
  return rb_const_defined(klass, SYM2ID(id)) ? Qtrue : Qfalse;
}

static VALUE sa_define_alias(VALUE self, VALUE obj,
  VALUE new_name, VALUE old_name) {

  rb_define_alias(obj, StringValuePtr(new_name), StringValuePtr(old_name));
  return Qnil;
}

static VALUE smv_test(VALUE self) {
  return rb_str_new2("test");
}

VALUE smv_undef_method(VALUE self, VALUE klass, VALUE name) {
  rb_undef_method(klass, StringValuePtr(name));
  return Qnil;
}

void Init_module_spec() {
  VALUE cls, mod;

  cls = rb_define_class("CApiModuleSpecs", rb_cObject);
  rb_define_method(cls, "rb_const_set", sm_const_set, 2);
  rb_define_method(cls, "rb_const_get", sm_const_get, 2);
  rb_define_method(cls, "rb_define_const", sm_define_const, 2);
  rb_define_method(cls, "rb_const_defined", sm_const_defined, 2);

  cls = rb_define_class("CApiDefineAliasSpecs", rb_cObject);
  rb_define_method(cls, "rb_define_alias", sa_define_alias, 3);

  cls = rb_define_class("CApiMethodVisSpecs", rb_cObject);
  mod = rb_define_module("CApiMethodVisModule");

  rb_define_method(cls, "smv_test_public", smv_test, 0);
  rb_define_protected_method(cls, "smv_test_protected", smv_test, 0);
  rb_define_private_method(cls, "smv_test_private", smv_test, 0);

  rb_define_singleton_method(mod, "smv_test_singleton", smv_test, 0);
  rb_define_module_function(mod, "smv_test_module_function", smv_test, 0);

  rb_define_global_function("smv_test_global_function", smv_test, 0);

  rb_define_method(cls, "rb_undef_method", smv_undef_method, 2);
}
