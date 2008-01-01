#include <ruby.h>

VALUE so_instance_of(VALUE self, VALUE obj, VALUE klass) {
  return rb_obj_is_instance_of(obj, klass);
}

VALUE so_kind_of(VALUE self, VALUE obj, VALUE klass) {
  return rb_obj_is_kind_of(obj, klass);
}

VALUE so_respond_to(VALUE self, VALUE obj, VALUE sym) {
  return rb_respond_to(obj, SYM2ID(sym)) == 1 ? Qtrue : Qfalse;
}

VALUE so_to_id(VALUE self, VALUE obj) {
  return ID2SYM(rb_to_id(obj));
}

VALUE so_require(VALUE self) {
  rb_require("fixtures/foo");
  return Qnil;
}

VALUE so_attr_get(VALUE self, VALUE obj, VALUE attr) {
  return rb_attr_get(obj, SYM2ID(attr));
}

VALUE so_check_array_type(VALUE self, VALUE ary) {
  return rb_check_array_type(ary);
}

VALUE so_check_string_type(VALUE self, VALUE str) {
  return rb_check_string_type(str);
}

VALUE so_check_convert_type(VALUE self, VALUE obj) {
  return rb_check_convert_type(obj, 9, "Array", "to_ary");
}

void Init_subtend_object() {
  VALUE cls;
  cls = rb_define_class("SubtendObject", rb_cObject);
  rb_define_method(cls, "rb_obj_is_instance_of", so_instance_of, 2);  
  rb_define_method(cls, "rb_obj_is_kind_of", so_kind_of, 2);  
  rb_define_method(cls, "rb_respond_to", so_respond_to, 2);    
  rb_define_method(cls, "rb_to_id", so_to_id, 1);
  rb_define_method(cls, "rb_require", so_require, 0);
  rb_define_method(cls, "rb_attr_get", so_attr_get, 2);
  rb_define_method(cls, "rb_check_array_type", so_check_array_type, 1);
  rb_define_method(cls, "rb_check_string_type", so_check_string_type, 1);
  rb_define_method(cls, "rb_check_convert_type", so_check_convert_type, 1);  
}