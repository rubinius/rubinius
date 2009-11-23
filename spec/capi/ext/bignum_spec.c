#include "ruby.h"

static VALUE bignum_spec_rb_big2long(VALUE self, VALUE num) {
  return LONG2NUM(rb_big2long(num));
}

static VALUE bignum_spec_rb_big2ulong(VALUE self, VALUE num) {
  return ULONG2NUM(rb_big2ulong(num));
}

static VALUE bignum_spec_rb_big2dbl(VALUE self, VALUE num) {
  return rb_float_new(rb_big2dbl(num));
}

static VALUE bignum_spec_rb_big2str(VALUE self, VALUE num, VALUE base) {
  return rb_big2str(num, FIX2INT(base));
}

void Init_bignum_spec() {
  VALUE cls;
  cls = rb_define_class("CApiBignumSpecs", rb_cObject);
  rb_define_method(cls, "rb_big2long", bignum_spec_rb_big2long, 1);
  rb_define_method(cls, "rb_big2ulong", bignum_spec_rb_big2ulong, 1);
  rb_define_method(cls, "rb_big2dbl", bignum_spec_rb_big2dbl, 1);
  rb_define_method(cls, "rb_big2str", bignum_spec_rb_big2str, 2);
}
