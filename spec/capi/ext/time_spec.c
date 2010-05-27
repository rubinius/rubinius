#include "ruby.h"
#include "rubyspec.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_RB_TIME_NEW
static VALUE time_spec_rb_time_new(VALUE self, VALUE sec, VALUE usec) {
  return rb_time_new(NUM2INT(sec), NUM2INT(usec));
}
#endif

void Init_time_spec() {
  VALUE cls;
  cls = rb_define_class("CApiTimeSpecs", rb_cObject);

#ifdef HAVE_RB_TIME_NEW
  rb_define_method(cls, "rb_time_new", time_spec_rb_time_new, 2);
#endif
}

#ifdef __cplusplus
}
#endif
