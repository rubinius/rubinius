#include <ruby.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

static VALUE array_spec_rb_ary_new(VALUE self) {
  VALUE ret;
  ret = rb_ary_new();
  return ret;
}

static VALUE array_spec_rb_ary_push(VALUE self, VALUE array, VALUE item) {
  rb_ary_push(array, item);
  return array;
}

static VALUE array_spec_rb_ary_pop(VALUE self, VALUE array) {
  return rb_ary_pop(array);
}

static VALUE array_spec_rb_ary_entry(VALUE self, VALUE array, VALUE offset) {
  return rb_ary_entry(array, FIX2INT(offset));
}

static VALUE array_spec_rb_ary_clear(VALUE self, VALUE array) {
  return rb_ary_clear(array);
}

static VALUE array_spec_rb_ary_dup(VALUE self, VALUE array) {
  return rb_ary_dup(array);
}

static VALUE array_spec_rb_ary_join(VALUE self, VALUE array1, VALUE array2) {
  return rb_ary_join(array1, array2);
}

static VALUE array_spec_rb_ary_reverse(VALUE self, VALUE array) {
  return rb_ary_reverse(array);
}

static VALUE array_spec_rb_ary_unshift(VALUE self, VALUE array, VALUE val) {
  return rb_ary_unshift(array, val);
}

static VALUE array_spec_rb_ary_shift(VALUE self, VALUE array) {
  return rb_ary_shift(array);
}

static VALUE array_spec_rb_ary_store(VALUE self, VALUE array, VALUE offset, VALUE value) {
  rb_ary_store(array, FIX2INT(offset), value);

  return Qnil;
}

static VALUE array_spec_iterate(VALUE self, VALUE array) {
  int i;
  for(i = 0; i < RARRAY_LEN(array); i++) {
    rb_yield(RARRAY_PTR(array)[i]);
  }
  return Qnil;
}

static VALUE array_spec_RARRAY_LEN(VALUE self, VALUE array) {
  return INT2FIX(RARRAY_LEN(array));
}

/* NOTE: RARRAY will not be supported in Rubinius, only
 * RARRAY_PTR and RARRAY_LEN will be provided.
 */
void Init_array_spec() {
  VALUE cls;
  cls = rb_define_class("CApiArraySpecs", rb_cObject);
  rb_define_method(cls, "rb_ary_new", array_spec_rb_ary_new, 0);
  rb_define_method(cls, "rb_ary_push", array_spec_rb_ary_push, 2);
  rb_define_method(cls, "rb_ary_entry", array_spec_rb_ary_entry, 2);
  rb_define_method(cls, "rb_ary_clear", array_spec_rb_ary_clear, 1);
  rb_define_method(cls, "rb_ary_dup", array_spec_rb_ary_dup, 1);
  rb_define_method(cls, "rb_ary_join", array_spec_rb_ary_join, 2);
  rb_define_method(cls, "rb_ary_reverse", array_spec_rb_ary_reverse, 1);
  rb_define_method(cls, "rb_ary_unshift", array_spec_rb_ary_unshift, 2);
  rb_define_method(cls, "rb_ary_shift", array_spec_rb_ary_shift, 1);
  rb_define_method(cls, "rb_ary_store", array_spec_rb_ary_store, 3);
  rb_define_method(cls, "rb_ary_pop", array_spec_rb_ary_pop, 1);
  rb_define_method(cls, "rb_ary_iterate", array_spec_iterate, 1);
  rb_define_method(cls, "RARRAY_LEN", array_spec_RARRAY_LEN, 1);
}

#ifdef __cplusplus
}
#endif
