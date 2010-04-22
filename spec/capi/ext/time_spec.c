#include "ruby.h"

static VALUE time_spec_rb_time_new(VALUE self, VALUE sec, VALUE usec) {
  return rb_time_new(NUM2INT(sec), NUM2INT(usec));
}

void Init_time_spec() {
  VALUE cls;
  cls = rb_define_class("CApiTimeSpecs", rb_cObject);
  rb_define_method(cls, "rb_time_new", time_spec_rb_time_new, 2);
}
