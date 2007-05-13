#include <ruby.h>
#include <stdio.h>

VALUE sa_new_array(VALUE self) {
  VALUE ret;
  ret = rb_ary_new();
  return ret;
}

VALUE sa_array_push(VALUE self, VALUE array, VALUE item) {
  rb_ary_push(array, item);
  return array;
}

VALUE sa_array_push2(VALUE self, VALUE array, VALUE item, VALUE i2) {
  rb_ary_push(array, item);
  rb_ary_push(array, i2);
  return array;
}

void Init_subtend_array() {
  VALUE cls;
  cls = rb_define_class("SubtendArray", rb_cObject);
  rb_define_method(cls, "new_array", sa_new_array, 0);
  rb_define_method(cls, "rb_ary_push", sa_array_push, 2);
  rb_define_method(cls, "rb_ary_push2", sa_array_push2, 3);
}
