#include "ruby.h"
#include <math.h>

static VALUE sf_new_zero(VALUE self) {
  double flt = 0;
  return rb_float_new(flt);
}

static VALUE sf_new_point_five(VALUE self) {
  double flt = 0.555;
  return rb_float_new(flt);
}

static VALUE float_spec_RFLOAT_value(VALUE self, VALUE float_h) {
  return rb_float_new(RFLOAT(float_h)->value);
}

static VALUE float_spec_RFLOAT_value_set(VALUE self, VALUE float_h, VALUE new_value) {
  RFLOAT(float_h)->value = (double)NUM2INT(new_value);
  return Qnil;
}

void Init_float_spec() {
  VALUE cls;
  cls = rb_define_class("CApiFloatSpecs", rb_cObject);
  rb_define_method(cls, "sf_new_zero", sf_new_zero, 0);
  rb_define_method(cls, "sf_new_point_five", sf_new_point_five, 0);
  rb_define_method(cls, "RFLOAT_value", float_spec_RFLOAT_value, 1);
  rb_define_method(cls, "RFLOAT_value_set", float_spec_RFLOAT_value_set, 2);
}
