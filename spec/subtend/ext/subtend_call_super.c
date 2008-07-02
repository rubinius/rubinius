#include <ruby.h>

static VALUE new_method(VALUE self) {
  VALUE retval;
  retval = rb_call_super(0, 0);
  retval = INT2NUM(NUM2INT(retval) * 2);
  return retval;
}

static VALUE override_method(VALUE self, VALUE obj) {
  rb_define_method(obj, "a_method", new_method, 0);
  return Qnil;
}

void Init_subtend_call_super() {
  VALUE cls;
  cls = rb_define_class("SubtendCallSuper", rb_cObject);
  rb_define_method(cls, "override_method", override_method, 1);
}
