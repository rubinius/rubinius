#include <string.h>

#include "ruby.h"
#include "rubyspec.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_RB_REG_NEW
VALUE sp_a_re(VALUE self) {
  return rb_reg_new("a", 1, 0);
}
#endif

void Init_regexp_spec() {
  VALUE cls;
  cls = rb_define_class("CApiRegexpSpecs", rb_cObject);

#ifdef HAVE_RB_REG_NEW
  rb_define_method(cls, "a_re", sp_a_re, 0);
#endif
}

#ifdef __cplusplus
}
#endif
