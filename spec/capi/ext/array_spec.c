#include "ruby.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

static VALUE array_spec_rb_ary_new(VALUE self) {
  VALUE ret;
  ret = rb_ary_new();
  return ret;
}

static VALUE array_spec_rb_ary_new2(VALUE self, VALUE length) {
  return rb_ary_new2(NUM2LONG(length));
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

static VALUE array_spec_RARRAY_PTR_iterate(VALUE self, VALUE array) {
  int i;
  VALUE* ptr;

  ptr = RARRAY_PTR(array);
  for(i = 0; i < RARRAY_LEN(array); i++) {
    rb_yield(ptr[i]);
  }
  return Qnil;
}

static VALUE array_spec_RARRAY_PTR_assign(VALUE self, VALUE array, VALUE value) {
  int i;
  VALUE* ptr;

  ptr = RARRAY_PTR(array);
  for(i = 0; i < RARRAY_LEN(array); i++) {
    ptr[i] = value;
  }
  return Qnil;
}

static VALUE array_spec_RARRAY_LEN(VALUE self, VALUE array) {
  return INT2FIX(RARRAY_LEN(array));
}

static VALUE array_spec_RARRAY_ptr_iterate(VALUE self, VALUE array) {
  int i;
  VALUE* ptr;

  ptr = RARRAY(array)->ptr;
  for(i = 0; i < RARRAY_LEN(array); i++) {
    rb_yield(ptr[i]);
  }
  return Qnil;
}

static VALUE array_spec_RARRAY_ptr_assign(VALUE self, VALUE array, VALUE value) {
  int i;
  VALUE* ptr;

  ptr = RARRAY(array)->ptr;
  for(i = 0; i < RARRAY_LEN(array); i++) {
    ptr[i] = value;
  }
  return array;
}

static VALUE array_spec_RARRAY_ptr_assign_call(VALUE self, VALUE array) {
  VALUE* ptr;

  ptr = RARRAY(array)->ptr;
  ptr[1] = INT2FIX(5);
  ptr[2] = INT2FIX(7);
  rb_ary_push(array, INT2FIX(9));

  return array;
}

static VALUE array_spec_RARRAY_ptr_assign_funcall(VALUE self, VALUE array) {
  VALUE* ptr;

  ptr = RARRAY(array)->ptr;
  ptr[1] = INT2FIX(1);
  ptr[2] = INT2FIX(2);
  rb_funcall(array, rb_intern("<<"), 1, INT2FIX(3));

  return array;
}

static VALUE array_spec_RARRAY_len(VALUE self, VALUE array) {
  return INT2FIX(RARRAY(array)->len);
}

static VALUE array_spec_rb_assoc_new(VALUE self, VALUE first, VALUE second) {
  return rb_assoc_new(first, second);
}

static VALUE array_spec_rb_ary_includes(VALUE self, VALUE ary, VALUE item) {
  return rb_ary_includes(ary, item);
}

static VALUE array_spec_rb_ary_aref(int argc, VALUE *argv, VALUE self) {
  VALUE ary, args;
  rb_scan_args(argc, argv, "1*", &ary, &args);
  return rb_ary_aref(RARRAY(args)->len, RARRAY_PTR(args), ary);
}

void Init_array_spec() {
  VALUE cls;
  cls = rb_define_class("CApiArraySpecs", rb_cObject);
  rb_define_method(cls, "rb_ary_new", array_spec_rb_ary_new, 0);
  rb_define_method(cls, "rb_ary_new2", array_spec_rb_ary_new2, 1);
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
  rb_define_method(cls, "RARRAY_PTR_iterate", array_spec_RARRAY_PTR_iterate, 1);
  rb_define_method(cls, "RARRAY_PTR_assign", array_spec_RARRAY_PTR_assign, 2);
  rb_define_method(cls, "RARRAY_LEN", array_spec_RARRAY_LEN, 1);
  rb_define_method(cls, "RARRAY_ptr_iterate", array_spec_RARRAY_ptr_iterate, 1);
  rb_define_method(cls, "RARRAY_ptr_assign", array_spec_RARRAY_ptr_assign, 2);
  rb_define_method(cls, "RARRAY_ptr_assign_call",
      array_spec_RARRAY_ptr_assign_call, 1);
  rb_define_method(cls, "RARRAY_ptr_assign_funcall",
      array_spec_RARRAY_ptr_assign_funcall, 1);
  rb_define_method(cls, "RARRAY_len", array_spec_RARRAY_len, 1);
  rb_define_method(cls, "rb_assoc_new", array_spec_rb_assoc_new, 2);
  rb_define_method(cls, "rb_ary_includes", array_spec_rb_ary_includes, 2);
  rb_define_method(cls, "rb_ary_aref", array_spec_rb_ary_aref, -1);
}

#ifdef __cplusplus
}
#endif
