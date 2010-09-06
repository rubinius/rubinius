#include "ruby.h"
#include "util.h"
#include "rubyspec.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_RUBY_SETENV
static VALUE util_spec_ruby_setenv(VALUE self, VALUE name, VALUE value) {
  ruby_setenv(RSTRING_PTR(name), RSTRING_PTR(value));
  return Qnil;
}
#endif

#ifdef HAVE_RUBY_STRDUP
static VALUE util_spec_ruby_strdup(VALUE self, VALUE str) {
  char *cstr = ruby_strdup(RSTRING_PTR(str));
  VALUE obj = rb_str_new2(cstr);
  xfree(cstr);
  return obj;
}
#endif

void Init_util_spec() {
  VALUE cls = rb_define_class("CApiUtilSpecs", rb_cObject);

#ifdef HAVE_RUBY_SETENV
  rb_define_method(cls, "ruby_setenv", util_spec_ruby_setenv, 2);
#endif

#ifdef HAVE_RUBY_STRDUP
  rb_define_method(cls, "ruby_strdup", util_spec_ruby_strdup, 1);
#endif
}

#ifdef __cplusplus
}
#endif
