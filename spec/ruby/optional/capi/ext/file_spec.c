#include "ruby.h"
#include "rubyspec.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_RB_FILE_OPEN
VALUE file_spec_rb_file_open(VALUE self, VALUE name, VALUE mode) {
  return rb_file_open(RSTRING_PTR(name), RSTRING_PTR(mode));
}
#endif

void Init_file_spec() {
  VALUE cls = rb_define_class("CApiFileSpecs", rb_cObject);

#ifdef HAVE_RB_FILE_OPEN
  rb_define_method(cls, "rb_file_open", file_spec_rb_file_open, 2);
#endif
}

#ifdef __cplusplus
}
#endif
