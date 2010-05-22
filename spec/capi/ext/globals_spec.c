#include "ruby.h"

static VALUE sb_gv_get(VALUE self, VALUE var) {
  return rb_gv_get( StringValuePtr(var) );
}

static VALUE sb_gv_set(VALUE self, VALUE var, VALUE val) {
  return rb_gv_set( StringValuePtr(var), val );
}

VALUE g_var = Qnil;
VALUE g_ro_var = Qnil;
VALUE g_hooked_var = Qnil;

static VALUE sb_define_variable(VALUE self, VALUE var_name, VALUE val) {
  g_var = val;
  rb_define_variable(StringValuePtr(var_name), &g_var);
  return Qnil;
}

static VALUE sb_f_global_variables(VALUE self) {
  return rb_f_global_variables();
}

static VALUE sb_get_global_value(VALUE self) {
  return g_var;
}

static VALUE sb_define_readonly_variable(VALUE self, VALUE var_name, VALUE val) {
  g_ro_var = val;
  rb_define_readonly_variable(StringValuePtr(var_name), &g_ro_var);
  return Qnil;
}

void var_2x_setter(VALUE val, ID id, VALUE *var) {
    *var = INT2NUM(NUM2INT(val) * 2);
}

static VALUE sb_define_hooked_variable(VALUE self, VALUE var_name) {
  rb_define_hooked_variable(StringValuePtr(var_name), &g_hooked_var, 0, var_2x_setter);
  return Qnil;
}

void Init_globals_spec() {
  VALUE cls;
  cls = rb_define_class("CApiGlobalSpecs", rb_cObject);

  rb_define_method(cls, "sb_gv_get", sb_gv_get, 1);
  rb_define_method(cls, "sb_gv_set", sb_gv_set, 2);
  rb_define_method(cls, "rb_define_variable", sb_define_variable, 2);
  rb_define_method(cls, "rb_f_global_variables", sb_f_global_variables, 0);
  rb_define_method(cls, "sb_get_global_value", sb_get_global_value, 0);
  rb_define_method(cls, "rb_define_readonly_variable", sb_define_readonly_variable, 2);
  rb_define_method(cls, "rb_define_hooked_variable_2x", sb_define_hooked_variable, 1);
}
