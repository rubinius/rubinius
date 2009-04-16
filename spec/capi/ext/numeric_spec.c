#include "ruby.h"

static VALUE numeric_spec_rb_num2long(VALUE self, VALUE num) {
  return LONG2NUM(rb_num2long(num));
}

static VALUE numeric_spec_rb_num2ulong(VALUE self, VALUE num) {
  return ULONG2NUM(rb_num2ulong(num));
}

void Init_numeric_spec() {
  VALUE cls;
  cls = rb_define_class("CApiNumericSpecs", rb_cObject);
  rb_define_method(cls, "rb_num2long", numeric_spec_rb_num2long, 1);
  rb_define_method(cls, "rb_num2ulong", numeric_spec_rb_num2ulong, 1);
}
