#include "ruby.h"

VALUE symbol_spec_rb_is_const_id(VALUE self, VALUE sym) {
  return rb_is_const_id(SYM2ID(sym));
}

VALUE symbol_spec_rb_is_instance_id(VALUE self, VALUE sym) {
  return rb_is_instance_id(SYM2ID(sym));
}

VALUE symbol_spec_rb_is_class_id(VALUE self, VALUE sym) {
  return rb_is_class_id(SYM2ID(sym));
}

void Init_symbol_spec() {
  VALUE cls;
  cls = rb_define_class("CApiSymbolSpecs", rb_cObject);

  rb_define_method(cls, "rb_is_const_id", symbol_spec_rb_is_const_id, 1);
  rb_define_method(cls, "rb_is_instance_id", symbol_spec_rb_is_instance_id, 1);
  rb_define_method(cls, "rb_is_class_id", symbol_spec_rb_is_class_id, 1);
}
