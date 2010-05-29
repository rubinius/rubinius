#include "ruby.h"
#include "rubyspec.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_RB_DEFINE_HOOKED_VARIABLE
VALUE g_hooked_var = Qnil;

void var_2x_setter(VALUE val, ID id, VALUE *var) {
    *var = INT2NUM(NUM2INT(val) * 2);
}

static VALUE sb_define_hooked_variable(VALUE self, VALUE var_name) {
  rb_define_hooked_variable(StringValuePtr(var_name), &g_hooked_var, 0, var_2x_setter);
  return Qnil;
}
#endif

#ifdef HAVE_RB_DEFINE_READONLY_VARIABLE
VALUE g_ro_var = Qnil;

static VALUE sb_define_readonly_variable(VALUE self, VALUE var_name, VALUE val) {
  g_ro_var = val;
  rb_define_readonly_variable(StringValuePtr(var_name), &g_ro_var);
  return Qnil;
}
#endif

#ifdef HAVE_RB_DEFINE_VARIABLE
VALUE g_var = Qnil;

static VALUE sb_get_global_value(VALUE self) {
  return g_var;
}

static VALUE sb_define_variable(VALUE self, VALUE var_name, VALUE val) {
  g_var = val;
  rb_define_variable(StringValuePtr(var_name), &g_var);
  return Qnil;
}
#endif

#ifdef HAVE_RB_F_GLOBAL_VARIABLES
static VALUE sb_f_global_variables(VALUE self) {
  return rb_f_global_variables();
}
#endif

#ifdef HAVE_RB_GV_GET
static VALUE sb_gv_get(VALUE self, VALUE var) {
  return rb_gv_get(StringValuePtr(var));
}
#endif

#ifdef HAVE_RB_GV_SET
static VALUE sb_gv_set(VALUE self, VALUE var, VALUE val) {
  return rb_gv_set(StringValuePtr(var), val);
}
#endif

void Init_globals_spec() {
  VALUE cls;
  cls = rb_define_class("CApiGlobalSpecs", rb_cObject);

#ifdef HAVE_RB_DEFINE_HOOKED_VARIABLE
  rb_define_method(cls, "rb_define_hooked_variable_2x", sb_define_hooked_variable, 1);
#endif

#ifdef HAVE_RB_DEFINE_READONLY_VARIABLE
  rb_define_method(cls, "rb_define_readonly_variable", sb_define_readonly_variable, 2);
#endif

#ifdef HAVE_RB_DEFINE_VARIABLE
  rb_define_method(cls, "rb_define_variable", sb_define_variable, 2);
  rb_define_method(cls, "sb_get_global_value", sb_get_global_value, 0);
#endif

#ifdef HAVE_RB_F_GLOBAL_VARIABLES
  rb_define_method(cls, "rb_f_global_variables", sb_f_global_variables, 0);
#endif

#ifdef HAVE_RB_GV_GET
  rb_define_method(cls, "sb_gv_get", sb_gv_get, 1);
#endif

#ifdef HAVE_RB_GV_SET
  rb_define_method(cls, "sb_gv_set", sb_gv_set, 2);
#endif
}

#ifdef __cplusplus
}
#endif
