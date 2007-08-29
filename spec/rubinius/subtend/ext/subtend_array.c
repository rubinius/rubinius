#include <ruby.h>
#include <stdio.h>

static VALUE sa_new_array(VALUE self) {
  VALUE ret;
  ret = rb_ary_new();
  return ret;
}

static VALUE sa_array_push(VALUE self, VALUE array, VALUE item) {
  rb_ary_push(array, item);
  return array;
}

static VALUE sa_array_pop(VALUE self, VALUE array) {
  return rb_ary_pop(array);
}

static VALUE sa_array_push2(VALUE self, VALUE array, VALUE item, VALUE i2) {
  rb_ary_push(array, item);
  rb_ary_push(array, i2);
  return array;
}

static VALUE sa_array_entry(VALUE self, VALUE array, VALUE offset) {
  return rb_ary_entry(array, FIX2INT(offset));
}

static VALUE sa_array_clear(VALUE self, VALUE array) {
  return rb_ary_clear(array);
}

static VALUE sa_array_dup(VALUE self, VALUE array) {
  return rb_ary_dup(array);
}

static VALUE sa_array_unshift(VALUE self, VALUE array, VALUE val) {
  return rb_ary_unshift(array, val);
}

static VALUE sa_array_shift(VALUE self, VALUE array) {
  return rb_ary_shift(array);
}

static VALUE sa_array_store(VALUE self, VALUE array, VALUE offset, VALUE value) {
  rb_ary_store(array, FIX2INT(offset), value);

  return Qnil;
}

void Init_subtend_array() {
  VALUE cls;
  cls = rb_define_class("SubtendArray", rb_cObject);
  rb_define_method(cls, "new_array", sa_new_array, 0);
  rb_define_method(cls, "rb_ary_push", sa_array_push, 2);
  rb_define_method(cls, "rb_ary_push2", sa_array_push2, 3);
  rb_define_method(cls, "rb_ary_entry", sa_array_entry, 2);
  rb_define_method(cls, "rb_ary_clear", sa_array_clear, 1);
  rb_define_method(cls, "rb_ary_dup", sa_array_dup, 1);
  rb_define_method(cls, "rb_ary_unshift", sa_array_unshift, 2);
  rb_define_method(cls, "rb_ary_shift", sa_array_shift, 1);
  rb_define_method(cls, "rb_ary_store", sa_array_store, 3);
  rb_define_method(cls, "rb_ary_pop", sa_array_pop, 1);
}
