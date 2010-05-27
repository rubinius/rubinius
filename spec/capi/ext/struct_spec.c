#include "ruby.h"
#include "intern.h"
#include "rubyspec.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_RB_STRUCT_DEFINE
/* Only allow setting three attributes, should be sufficient for testing. */
static VALUE sa_struct_define(VALUE self, VALUE name,
  VALUE attr1, VALUE attr2, VALUE attr3) {

  const char *a1 = StringValuePtr(attr1);
  const char *a2 = StringValuePtr(attr2);
  const char *a3 = StringValuePtr(attr3);
  char *nm = NULL;

  if (name != Qnil) nm = StringValuePtr(name);

  return rb_struct_define(nm, a1, a2, a3, NULL);
}
#endif

void Init_struct_spec() {
  VALUE cls;
  cls = rb_define_class("CApiStructSpecs", rb_cObject);

#ifdef HAVE_RB_STRUCT_DEFINE
  rb_define_method(cls, "rb_struct_define", sa_struct_define, 4);
#endif
}

#ifdef __cplusplus
}
#endif
