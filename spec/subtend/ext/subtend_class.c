#include <ruby.h>
#include <stdio.h>
#include <string.h>

static VALUE sc_rb_obj_alloc(VALUE self, VALUE klass) {
  return rb_obj_alloc(klass);
}

static VALUE sc_rb_obj_call_init(VALUE self, VALUE object,
                                 VALUE nargs, VALUE args) {
  int c_nargs = FIX2INT(nargs);
  VALUE c_args[c_nargs];
  int i;

  for (i = 0; i < c_nargs; i++)
    c_args[i] = rb_ary_entry(args, i);

  rb_obj_call_init(object, c_nargs, c_args);

  return Qnil;
}

static VALUE sc_rb_class_new_instance(VALUE self,
                                      VALUE nargs, VALUE args,
                                      VALUE klass) {
  int c_nargs = FIX2INT(nargs);
  VALUE c_args[c_nargs];
  int i;

  for (i = 0; i < c_nargs; i++)
    c_args[i] = rb_ary_entry(args, i);

  return rb_class_new_instance(c_nargs, c_args, klass);
}

static VALUE sc_include_module(VALUE self, VALUE klass, VALUE module) {
  rb_include_module(klass, module);
  return klass;
}

void sc_define_attr(VALUE self, VALUE klass, VALUE sym, VALUE read, VALUE write) {
  int int_read, int_write;
  int_read = read == Qtrue ? 1 : 0;
  int_write = write == Qtrue ? 1 : 0;
  rb_define_attr(klass, SYM2ID(sym), int_read, int_write);
}

static VALUE sc_rbclass2name(VALUE self, VALUE klass) {
  return rb_str_new2( rb_class2name(klass) );
}

void Init_subtend_class() {
  VALUE cls;
  cls = rb_define_class("SubtendClass", rb_cObject);
  rb_define_method(cls, "rb_obj_alloc", sc_rb_obj_alloc, 1);
  rb_define_method(cls, "rb_obj_call_init", sc_rb_obj_call_init, 3);
  rb_define_method(cls, "rb_class_new_instance", sc_rb_class_new_instance, 3);
  rb_define_method(cls, "rb_include_module", sc_include_module, 2);
  rb_define_method(cls, "rb_define_attr", sc_define_attr, 4);
  rb_define_method(cls, "rb_class2name", sc_rbclass2name, 1);
}
