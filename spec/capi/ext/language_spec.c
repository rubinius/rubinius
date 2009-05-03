#include "ruby.h"

// @todo cleanup cleanup cleanup

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

static VALUE sb_test(VALUE self) {
  if(rb_block_given_p()) return Qtrue;
  return Qfalse;
}

static VALUE sb_yield(VALUE self) {
  return rb_yield(INT2FIX(5));
}

static VALUE language_spec_rb_define_class_under(VALUE self,
    VALUE outer, VALUE name, VALUE super) {
  const char* class_name = StringValuePtr(name);
  if(NIL_P(super)) super = 0;
  return rb_define_class_under(outer, class_name, super);
}

// @todo: fix all these specs to follow the style guide
void Init_language_spec() {
  VALUE cls;
  cls = rb_define_class("CApiCallSuperSpecs", rb_cObject);
  rb_define_method(cls, "override_method", override_method, 1);
  rb_define_method(cls, "test_subclass_and_instantiation",
    test_subclass_and_instantiation, 0);

  cls = rb_define_class("CApiBlockSpecs", rb_cObject);
  rb_define_method(cls, "block_given?", sb_test, 0);
  rb_define_method(cls, "do_yield", sb_yield, 0);

  cls = rb_define_class("CApiLanguageSpecs", rb_cObject);
  rb_define_method(cls, "rb_define_class_under",
      language_spec_rb_define_class_under, 3);
}
