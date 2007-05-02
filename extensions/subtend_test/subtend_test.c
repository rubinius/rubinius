#include <ruby.h>
#include <stdio.h>
#include <string.h>

VALUE st_new_array(VALUE self) {
  return rb_ary_new();
}

VALUE st_new_array_of_size_two(VALUE self) {
  return rb_ary_new2(2);
}

VALUE st_push_on_new_array(VALUE self, VALUE item) {
  VALUE ary;
  ary = rb_ary_new();
  rb_ary_push(ary, item);
  return ary;
}

VALUE st_push_on_array(VALUE self, VALUE array, VALUE item) {  
  rb_ary_push(array, item);
  return array;
}

VALUE st_dup_string(VALUE self, VALUE str) {  
  return rb_str_dup(str);
}

VALUE st_make_urgent(VALUE self, VALUE str) {
  char* urgent_str;
  urgent_str = "!?";
  return rb_str_buf_cat(str, urgent_str, strlen(urgent_str));
}



void Init_subtend_test() {
  VALUE cls;

  cls = rb_define_class("SubtendTest", rb_cObject);
  rb_define_method(cls, "new_array", st_new_array, 0);
  rb_define_method(cls, "new_array_of_size_two", st_new_array_of_size_two, 0);
  rb_define_method(cls, "push_on_array", st_push_on_array, 2);
  rb_define_method(cls, "push_on_new_array", st_push_on_new_array, 1);
  rb_define_method(cls, "dup_string", st_dup_string, 1);
  rb_define_method(cls, "make_urgent!", st_make_urgent, 1);
}
