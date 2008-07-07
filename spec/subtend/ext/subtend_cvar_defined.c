#include <ruby.h>
#include <intern.h>

static VALUE sa_cvar_defined(VALUE self, VALUE klass, VALUE id) {
  ID as_id = rb_intern(StringValuePtr(id));
  return rb_cvar_defined(klass, as_id);
}

void Init_subtend_cvar_defined() {
  VALUE cls;
  cls = rb_define_class("SubtendCVarDefined", rb_cObject);
  rb_define_method(cls, "rb_cvar_defined", sa_cvar_defined, 2);
}
