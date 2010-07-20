#include "ruby.h"
#include "rubyspec.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_RB_MARSHAL_DUMP
VALUE marshal_spec_rb_marshal_dump(VALUE self, VALUE obj, VALUE port) {
  return rb_marshal_dump(obj, port);
}
#endif

void Init_marshal_spec() {
  VALUE cls;
  cls = rb_define_class("CApiMarshalSpecs", rb_cObject);

#ifdef HAVE_RB_MARSHAL_DUMP
  rb_define_method(cls, "rb_marshal_dump", marshal_spec_rb_marshal_dump, 2);
#endif
}

#ifdef __cplusplus
}
#endif
