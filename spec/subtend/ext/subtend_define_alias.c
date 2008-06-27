#include <ruby.h>

static VALUE sa_define_alias(VALUE self, VALUE obj,
  VALUE new_name, VALUE old_name) {
    
  rb_define_alias(obj, StringValuePtr(new_name), StringValuePtr(old_name));
  return Qnil;
}

void Init_subtend_define_alias() {
  VALUE cls;
  cls = rb_define_class("SubtendDefineAlias", rb_cObject);
  rb_define_method(cls, "rb_define_alias", sa_define_alias, 3);
}
