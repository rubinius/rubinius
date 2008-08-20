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

static VALUE initialize_super(VALUE self) {
  return self;
}

static VALUE initialize_sub(VALUE self) {
  return rb_call_super(0, 0);
}

static VALUE test_subclass_and_instantiation(VALUE self) {
  VALUE module = rb_define_module("ModuleTest");
  VALUE class_super = rb_define_class_under(module, "ClassSuper", rb_cObject);
  VALUE class_sub;
    
  rb_define_method(class_super, "initialize", initialize_super, 0);
  class_sub = rb_define_class_under(module, "ClassSub", class_super);
  rb_define_method(class_sub, "initialize", initialize_sub, 0);

  return Qtrue;
}

void Init_subtend_call_super() {
  VALUE cls;
  cls = rb_define_class("SubtendCallSuper", rb_cObject);
  rb_define_method(cls, "override_method", override_method, 1);
  rb_define_method(cls, "test_subclass_and_instantiation",
    test_subclass_and_instantiation, 0);
}
