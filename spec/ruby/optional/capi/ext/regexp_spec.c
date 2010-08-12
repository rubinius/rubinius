#include <string.h>

#include "ruby.h"
#include "rubyspec.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_RB_REG_NEW
VALUE regexp_spec_re(VALUE self) {
  return rb_reg_new("a", 1, 0);
}
#endif

#ifdef HAVE_RB_REG_NTH_MATCH
VALUE regexp_spec_reg_1st_match(VALUE self, VALUE md) {
  return rb_reg_nth_match(1, md);
}
#endif

void Init_regexp_spec() {
  VALUE cls;
  cls = rb_define_class("CApiRegexpSpecs", rb_cObject);

#ifdef HAVE_RB_REG_NEW
  rb_define_method(cls, "a_re", regexp_spec_re, 0);
#endif

#ifdef HAVE_RB_REG_NTH_MATCH
  rb_define_method(cls, "a_re_1st_match", regexp_spec_reg_1st_match, 1);
#endif
}

#ifdef __cplusplus
}
#endif
