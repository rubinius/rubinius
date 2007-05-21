#include <ruby.h>
#include <stdio.h>

#define ID2SYM(in) ((VALUE)in)

VALUE se_raise(VALUE self) {
  rb_raise(rb_eTypeError, "Wrong argument type %s (expected %s)", "Integer", "String");
  return Qnil;
}

VALUE se_raise_early(VALUE self, VALUE obj) {
  rb_hash_aset(obj, ID2SYM(rb_intern("screwed")), Qfalse);
  rb_raise(rb_eTypeError, "Wrong argument type %s (expected %s)", "Integer", "String");
  rb_hash_aset(obj, ID2SYM(rb_intern("screwed")), Qtrue);
  return Qnil;
}

void Init_subtend_exception() {
  VALUE cls;
  cls = rb_define_class("SubtendRaiser", rb_cObject);
  rb_define_method(cls, "raise!", se_raise, 0);
  rb_define_method(cls, "raise_early", se_raise_early, 1);
}
