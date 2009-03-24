#include <ruby.h>
#include <stdio.h>
#include <string.h>

static VALUE sc_rb_obj_alloc(VALUE self, VALUE klass) {
  return rb_obj_alloc(klass);
}

static VALUE sc_rb_obj_call_init(VALUE self, VALUE object,
                                 VALUE nargs, VALUE args) {
  int c_nargs = FIX2INT(nargs);
  VALUE c_args[c_nargs];
  int i;

  for (i = 0; i < c_nargs; i++)
    c_args[i] = rb_ary_entry(args, i);

  rb_obj_call_init(object, c_nargs, c_args);

  return Qnil;
}

static VALUE sc_rb_class_new_instance(VALUE self,
                                      VALUE nargs, VALUE args,
                                      VALUE klass) {
  int c_nargs = FIX2INT(nargs);
  VALUE c_args[c_nargs];
  int i;

  for (i = 0; i < c_nargs; i++)
    c_args[i] = rb_ary_entry(args, i);

  return rb_class_new_instance(c_nargs, c_args, klass);
}

static VALUE sc_include_module(VALUE self, VALUE klass, VALUE module) {
  rb_include_module(klass, module);
  return klass;
}

VALUE sc_define_attr(VALUE self, VALUE klass, VALUE sym, VALUE read, VALUE write) {
  int int_read, int_write;
  int_read = read == Qtrue ? 1 : 0;
  int_write = write == Qtrue ? 1 : 0;
  rb_define_attr(klass, rb_id2name(SYM2ID(sym)), int_read, int_write);
  return Qnil;
}

static VALUE sc_rbclass2name(VALUE self, VALUE klass) {
  return rb_str_new2( rb_class2name(klass) );
}

static VALUE sa_cv_set(VALUE self, VALUE klass, VALUE name, VALUE val) {
	return rb_cv_set(klass, StringValuePtr(name), val);
}

static VALUE sa_cvar_set(VALUE self, VALUE klass, VALUE name, VALUE val) {
	return rb_cvar_set(klass, rb_intern(StringValuePtr(name)), val);
}

static VALUE sa_cv_get(VALUE self, VALUE klass, VALUE name) {
	return rb_cv_get(klass, StringValuePtr(name));
}

static VALUE sa_cvar_get(VALUE self, VALUE klass, VALUE name) {
	return rb_cvar_get(klass, rb_intern(StringValuePtr(name)));
}

static VALUE sa_cvar_defined(VALUE self, VALUE klass, VALUE id) {
  ID as_id = rb_intern(StringValuePtr(id));
  return rb_cvar_defined(klass, as_id);
}

void Init_class_spec() {
  VALUE cls;
  cls = rb_define_class("CApiClassSpecs", rb_cObject);
  rb_define_method(cls, "rb_obj_alloc", sc_rb_obj_alloc, 1);
  rb_define_method(cls, "rb_obj_call_init", sc_rb_obj_call_init, 3);
  rb_define_method(cls, "rb_class_new_instance", sc_rb_class_new_instance, 3);
  rb_define_method(cls, "rb_include_module", sc_include_module, 2);
  rb_define_method(cls, "rb_define_attr", sc_define_attr, 4);
  rb_define_method(cls, "rb_class2name", sc_rbclass2name, 1);

  cls = rb_define_class("CApiCVGetSetSpecs", rb_cObject);
  rb_define_method(cls, "rb_cv_set", sa_cv_set, 3);
  rb_define_method(cls, "rb_cvar_set", sa_cvar_set, 3);
  rb_define_method(cls, "rb_cv_get", sa_cv_get, 2);
  rb_define_method(cls, "rb_cvar_get", sa_cvar_get, 2);

  cls = rb_define_class("CApiCVarDefinedSpecs", rb_cObject);
  rb_define_method(cls, "rb_cvar_defined", sa_cvar_defined, 2);
}
