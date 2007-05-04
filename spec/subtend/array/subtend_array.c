#include <ruby.h>

VALUE sa_new_array(VALUE self) {
  return rb_ary_new();
}

VALUE sa_array_push(VALUE self, VALUE array, VALUE item) {
  rb_ary_push(array, item);
  return array;
}

void Init_subtend_array() {
  VALUE cls;
  cls = rb_define_class("SubtendArray", rb_cObject);
  rb_define_method(cls, "new_array", sa_new_array, 0);
  rb_define_method(cls, "rb_ary_push", sa_array_push, 2);
}
