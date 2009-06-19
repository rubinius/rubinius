#include "ruby.h"

VALUE io_spec_rb_io_write(VALUE self, VALUE io, VALUE str) {
  return rb_io_write(io, str);
}

void Init_io_spec() {
  VALUE cls = rb_define_class("CApiIOSpecs", rb_cObject);

  rb_define_method(cls, "rb_io_write", io_spec_rb_io_write, 2);
}
