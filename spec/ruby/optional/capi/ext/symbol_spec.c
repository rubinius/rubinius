#include "ruby.h"
#include "rubyspec.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_RB_IS_CLASS_ID
VALUE symbol_spec_rb_is_class_id(VALUE self, VALUE sym) {
  return rb_is_class_id(SYM2ID(sym));
}
#endif

#ifdef HAVE_RB_IS_CONST_ID
VALUE symbol_spec_rb_is_const_id(VALUE self, VALUE sym) {
  return rb_is_const_id(SYM2ID(sym));
}
#endif

#ifdef HAVE_RB_IS_INSTANCE_ID
VALUE symbol_spec_rb_is_instance_id(VALUE self, VALUE sym) {
  return rb_is_instance_id(SYM2ID(sym));
}
#endif

void Init_symbol_spec() {
  VALUE cls;
  cls = rb_define_class("CApiSymbolSpecs", rb_cObject);

#ifdef HAVE_RB_IS_CLASS_ID
  rb_define_method(cls, "rb_is_class_id", symbol_spec_rb_is_class_id, 1);
#endif

#ifdef HAVE_RB_IS_CONST_ID
  rb_define_method(cls, "rb_is_const_id", symbol_spec_rb_is_const_id, 1);
#endif

#ifdef HAVE_RB_IS_INSTANCE_ID
  rb_define_method(cls, "rb_is_instance_id", symbol_spec_rb_is_instance_id, 1);
#endif
}

#ifdef __cplusplus
}
#endif
