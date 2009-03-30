#include <ruby.h>
#include <stdio.h>
#include <string.h>

VALUE exception_spec_rb_raise(VALUE self, VALUE hash) {
  rb_hash_aset(hash, ID2SYM(rb_intern("stage")), ID2SYM(rb_intern("before")));
  rb_raise(rb_eTypeError, "Wrong argument type %s (expected %s)", "Integer", "String");
  rb_hash_aset(hash, ID2SYM(rb_intern("stage")), ID2SYM(rb_intern("after")));
  return Qnil;
}

VALUE exception_spec_rb_exc_new(VALUE self, VALUE str) {
  char *cstr = StringValuePtr(str);
  return rb_exc_new(rb_eException, cstr, strlen(cstr));
}

VALUE exception_spec_rb_exc_new2(VALUE self, VALUE str) {
  char *cstr = StringValuePtr(str);
  return rb_exc_new2(rb_eException, cstr);
}

VALUE exception_spec_rb_exc_new3(VALUE self, VALUE str) {
  return rb_exc_new3(rb_eException, str);
}

void Init_exception_spec() {
  VALUE cls;
  cls = rb_define_class("CApiExceptionSpecs", rb_cObject);
  rb_define_method(cls, "rb_raise", exception_spec_rb_raise, 1);
  rb_define_method(cls, "rb_exc_new", exception_spec_rb_exc_new, 1);
  rb_define_method(cls, "rb_exc_new2", exception_spec_rb_exc_new2, 1);
  rb_define_method(cls, "rb_exc_new3", exception_spec_rb_exc_new3, 1);
}
