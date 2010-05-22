#include "ruby.h"

static VALUE module_specs_define_const(VALUE self, VALUE klass, VALUE str_name, VALUE val) {
  rb_define_const(klass, RSTRING_PTR(str_name), val);
  return Qnil;
}

static VALUE module_specs_define_global_const(VALUE self, VALUE str_name, VALUE obj) {
  rb_define_global_const(RSTRING_PTR(str_name), obj);
  return Qnil;
}

static VALUE module_specs_const_set(VALUE self, VALUE klass, VALUE name, VALUE val) {
  rb_const_set(klass, SYM2ID(name), val);
  return Qnil;
}

static VALUE module_specs_const_get(VALUE self, VALUE klass, VALUE val) {
  return rb_const_get(klass, SYM2ID(val));
}

static VALUE module_specs_const_get_from(VALUE self, VALUE klass, VALUE val) {
  return rb_const_get_from(klass, SYM2ID(val));
}

static VALUE module_specs_const_get_at(VALUE self, VALUE klass, VALUE val) {
  return rb_const_get_at(klass, SYM2ID(val));
}

static VALUE module_specs_const_defined(VALUE self, VALUE klass, VALUE id) {
  return rb_const_defined(klass, SYM2ID(id)) ? Qtrue : Qfalse;
}

static VALUE module_specs_const_defined_at(VALUE self, VALUE klass, VALUE id) {
  return rb_const_defined_at(klass, SYM2ID(id)) ? Qtrue : Qfalse;
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
  rb_define_method(cls, "rb_const_set", module_specs_const_set, 3);
  rb_define_method(cls, "rb_const_get", module_specs_const_get, 2);
  rb_define_method(cls, "rb_const_get_from", module_specs_const_get_from, 2);
  rb_define_method(cls, "rb_const_get_at", module_specs_const_get_at, 2);
  rb_define_method(cls, "rb_define_const", module_specs_define_const, 3);
  rb_define_method(cls, "rb_define_global_const", module_specs_define_global_const, 2);
  rb_define_method(cls, "rb_const_defined", module_specs_const_defined, 2);
  rb_define_method(cls, "rb_const_defined_at", module_specs_const_defined_at, 2);

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
