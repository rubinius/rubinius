#include "ruby.h"

static VALUE numeric_spec_rb_num2long(VALUE self, VALUE num) {
  return LONG2NUM(rb_num2long(num));
}

static VALUE numeric_spec_rb_num2ulong(VALUE self, VALUE num) {
  return ULONG2NUM(rb_num2ulong(num));
}

static VALUE numeric_spec_rb_Integer(VALUE self, VALUE str) {
  return rb_Integer(str);
}

static VALUE numeric_spec_rb_ll2inum_14(VALUE self) {
  return rb_ll2inum(14);
}

static VALUE numeric_spec_rb_int2inum_14(VALUE self) {
  return rb_int2inum(14);
}

void Init_numeric_spec() {
  VALUE cls;
  cls = rb_define_class("CApiNumericSpecs", rb_cObject);
  rb_define_method(cls, "rb_num2long", numeric_spec_rb_num2long, 1);
  rb_define_method(cls, "rb_num2ulong", numeric_spec_rb_num2ulong, 1);
  rb_define_method(cls, "rb_Integer", numeric_spec_rb_Integer, 1);
  rb_define_method(cls, "rb_ll2inum_14", numeric_spec_rb_ll2inum_14, 0);
  rb_define_method(cls, "rb_int2inum_14", numeric_spec_rb_int2inum_14, 0);
}
