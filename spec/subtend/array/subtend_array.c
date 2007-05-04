#include <ruby.h>

VALUE sa_new_array(VALUE self) {
  return rb_ary_new();
}

void Init_subtend_array() {
  VALUE cls;
  cls = rb_define_class("SubtendArray", rb_cObject);
  rb_define_method(cls, "rb_ary_new", sa_new_array, 0);
}
