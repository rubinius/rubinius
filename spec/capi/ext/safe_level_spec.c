#include "ruby.h"

static VALUE sb_safe_level(VALUE self) {
  int sl = rb_safe_level();
  return INT2FIX(sl);
}

static VALUE sb_secure(VALUE self, VALUE arg) {
  rb_secure( FIX2INT(arg) );
  return Qnil;
}

void Init_safe_level_spec() {
  VALUE cls;
  cls = rb_define_class("CApiSafeLevelSpecs", rb_cObject);

  rb_define_method(cls, "sb_safe_level", sb_safe_level, 0);
  rb_define_method(cls, "sb_secure", sb_secure, 1);
}
