#include <ruby.h>

VALUE so_instance_of(VALUE self, VALUE obj, VALUE klass) {
  return rb_obj_is_instance_of(obj, klass);
}

VALUE so_kind_of(VALUE self, VALUE obj, VALUE klass) {
  return rb_obj_is_kind_of(obj, klass);
}

VALUE so_respond_to(VALUE self, VALUE obj, VALUE sym) {
  return (VALUE)rb_respond_to(obj, SYM2ID(sym));
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
  return rb_check_convert_type(obj, T_ARRAY, "Array", "to_ary");
}

VALUE so_convert_type(VALUE self, VALUE obj) {
  return rb_convert_type(obj, T_ARRAY, "Array", "to_ary");
}

VALUE so_rbobjclassname(VALUE self, VALUE obj) {
  return rb_str_new2( rb_obj_classname(obj) );
}

VALUE so_rbclassof(VALUE self, VALUE obj) {
  return rb_class_of(obj);
}

VALUE so_inspect(VALUE self, VALUE obj) {
  return rb_inspect(obj);
}

VALUE so_is_type_nil(VALUE self, VALUE obj) {
  if(TYPE(obj) == T_NIL) {
    return Qtrue;
  }
  return Qfalse;
}

VALUE so_is_type_object(VALUE self, VALUE obj) {
  if(TYPE(obj) == T_OBJECT) {
    return Qtrue;
  }
  return Qfalse;
}

VALUE so_is_type_array(VALUE self, VALUE obj) {
  if(TYPE(obj) == T_ARRAY) {
    return Qtrue;
  }
  return Qfalse;
}

VALUE so_is_type_module(VALUE self, VALUE obj) {
  if(TYPE(obj) == T_MODULE) {
    return Qtrue;
  }
  return Qfalse;
}

VALUE so_is_type_class(VALUE self, VALUE obj) {
  if(TYPE(obj) == T_CLASS) {
    return Qtrue;
  }
  return Qfalse;
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
  rb_define_method(cls, "rb_convert_type", so_convert_type, 1);
  rb_define_method(cls, "rb_inspect", so_inspect, 1);
  rb_define_method(cls, "rb_obj_classname", so_rbobjclassname, 1);
  rb_define_method(cls, "rb_class_of", so_rbclassof, 1);
  rb_define_method(cls, "rb_is_type_nil", so_is_type_nil, 1);
  rb_define_method(cls, "rb_is_type_object", so_is_type_object, 1);
  rb_define_method(cls, "rb_is_type_array", so_is_type_array, 1);
  rb_define_method(cls, "rb_is_type_module", so_is_type_module, 1);
  rb_define_method(cls, "rb_is_type_class", so_is_type_class, 1);
}
