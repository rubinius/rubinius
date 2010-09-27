#include "ruby.h"
#include "rubyspec.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_RB_IO_WRITE
VALUE io_spec_rb_io_write(VALUE self, VALUE io, VALUE str) {
  return rb_io_write(io, str);
}
#endif

#ifdef HAVE_RB_IO_CLOSE
VALUE io_spec_rb_io_close(VALUE self, VALUE io) {
  return rb_io_close(io);
}
#endif

void Init_io_spec() {
  VALUE cls = rb_define_class("CApiIOSpecs", rb_cObject);

#ifdef HAVE_RB_IO_WRITE
  rb_define_method(cls, "rb_io_write", io_spec_rb_io_write, 2);
#endif
#ifdef HAVE_RB_IO_CLOSE
  rb_define_method(cls, "rb_io_close", io_spec_rb_io_close, 1);
#endif
}

#ifdef __cplusplus
}
#endif
