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
  return rb_to_id(obj);
}

void Init_subtend_object() {
  VALUE cls;
  cls = rb_define_class("SubtendObject", rb_cObject);
  rb_define_method(cls, "rb_obj_is_instance_of", so_instance_of, 2);  
  rb_define_method(cls, "rb_obj_is_kind_of", so_kind_of, 2);  
  rb_define_method(cls, "rb_respond_to", so_respond_to, 2);    
  rb_define_method(cls, "rb_to_id", so_to_id, 1);      
}