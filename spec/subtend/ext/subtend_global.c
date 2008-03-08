#include <ruby.h>

static VALUE sb_gv_get(VALUE self, VALUE var) {
  return rb_gv_get( rb_str_get_char_ptr(var) );
}

static VALUE sb_gv_set(VALUE self, VALUE var, VALUE val) {
  return rb_gv_set( rb_str_get_char_ptr(var), val );
}

void Init_subtend_global() {
  VALUE cls;
  cls = rb_define_class("SubtendGlobal", rb_cObject);

  rb_define_method(cls, "sb_gv_get", sb_gv_get, 1);
  rb_define_method(cls, "sb_gv_set", sb_gv_set, 2);
}
