#include <ruby.h>
#include <stdio.h>
#include <string.h>

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

VALUE se_exc_new(VALUE self, VALUE str) {
  char *cstr = STR2CSTR(str);
  return rb_exc_new(rb_eException, cstr, strlen(cstr));
}

VALUE se_exc_new2(VALUE self, VALUE str) {
  char *cstr = STR2CSTR(str);
  return rb_exc_new2(rb_eException, cstr);
}

VALUE se_exc_new3(VALUE self, VALUE str) {
  return rb_exc_new3(rb_eException, str);
}

void Init_subtend_exception() {
  VALUE cls;
  cls = rb_define_class("SubtendRaiser", rb_cObject);
  rb_define_method(cls, "raise!", se_raise, 0);
  rb_define_method(cls, "raise_early", se_raise_early, 1);
  rb_define_method(cls, "rb_exc_new", se_exc_new, 1);
  rb_define_method(cls, "rb_exc_new2", se_exc_new2, 1);
  rb_define_method(cls, "rb_exc_new3", se_exc_new3, 1);
}
