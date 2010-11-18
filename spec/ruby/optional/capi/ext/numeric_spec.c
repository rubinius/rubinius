#include "ruby.h"
#include "rubyspec.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_NUM2CHR
static VALUE numeric_spec_NUM2CHR(VALUE self, VALUE value) {
  return INT2FIX(NUM2CHR(value));
}
#endif

#ifdef HAVE_RB_INT2INUM
static VALUE numeric_spec_rb_int2inum_14(VALUE self) {
  return rb_int2inum(14);
}
#endif

#ifdef HAVE_RB_INTEGER
static VALUE numeric_spec_rb_Integer(VALUE self, VALUE str) {
  return rb_Integer(str);
}
#endif

#ifdef HAVE_RB_LL2INUM
static VALUE numeric_spec_rb_ll2inum_14(VALUE self) {
  return rb_ll2inum(14);
}
#endif

#ifdef HAVE_RB_NUM2DBL
static VALUE numeric_spec_rb_num2dbl(VALUE self, VALUE num) {
  return rb_float_new(rb_num2dbl(num));
}
#endif

#ifdef HAVE_RB_NUM2LONG
static VALUE numeric_spec_rb_num2long(VALUE self, VALUE num) {
  return LONG2NUM(rb_num2long(num));
}
#endif

#ifdef HAVE_RB_NUM2ULONG
static VALUE numeric_spec_rb_num2ulong(VALUE self, VALUE num) {
  return ULONG2NUM(rb_num2ulong(num));
}
#endif

#ifdef HAVE_RB_NUM_ZERODIV
static VALUE numeric_spec_rb_num_zerodiv(VALUE self) {
  rb_num_zerodiv();
}
#endif

#ifdef HAVE_RB_CMPINT
static VALUE numeric_spec_rb_cmpint(VALUE self, VALUE val, VALUE b) {
  return INT2FIX(rb_cmpint(val, val, b));
}
#endif

void Init_numeric_spec() {
  VALUE cls;
  cls = rb_define_class("CApiNumericSpecs", rb_cObject);

#ifdef HAVE_NUM2CHR
  rb_define_method(cls, "NUM2CHR", numeric_spec_NUM2CHR, 1);
#endif

#ifdef HAVE_RB_INT2INUM
  rb_define_method(cls, "rb_int2inum_14", numeric_spec_rb_int2inum_14, 0);
#endif

#ifdef HAVE_RB_INTEGER
  rb_define_method(cls, "rb_Integer", numeric_spec_rb_Integer, 1);
#endif

#ifdef HAVE_RB_LL2INUM
  rb_define_method(cls, "rb_ll2inum_14", numeric_spec_rb_ll2inum_14, 0);
#endif

#ifdef HAVE_RB_NUM2DBL
  rb_define_method(cls, "rb_num2dbl", numeric_spec_rb_num2dbl, 1);
#endif

#ifdef HAVE_RB_NUM2LONG
  rb_define_method(cls, "rb_num2long", numeric_spec_rb_num2long, 1);
#endif

#ifdef HAVE_RB_NUM2ULONG
  rb_define_method(cls, "rb_num2ulong", numeric_spec_rb_num2ulong, 1);
#endif

#ifdef HAVE_RB_NUM_ZERODIV
  rb_define_method(cls, "rb_num_zerodiv", numeric_spec_rb_num_zerodiv, 0);
#endif

#ifdef HAVE_RB_CMPINT
  rb_define_method(cls, "rb_cmpint", numeric_spec_rb_cmpint, 2);
#endif
}

#ifdef __cplusplus
}
#endif
