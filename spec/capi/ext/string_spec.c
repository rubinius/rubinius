#include <ruby.h>
#include <string.h>

VALUE ss_say_hello(VALUE self) {
  const char *str = "hello";
  return rb_str_new(str, strlen(str));
}

VALUE ss_say_hello2(VALUE self) {
  return rb_str_new2("hello\0invisible");
}

VALUE ss_dup_string(VALUE self, VALUE str) {
  return rb_str_dup(str);
}

VALUE ss_str_append(VALUE self, VALUE str, VALUE str2) {
  return rb_str_append(str, str2);
}

VALUE ss_cat_as_question(VALUE self, VALUE str) {
  return rb_str_cat(str, "?", 1);
}

VALUE ss_cat2_as_question(VALUE self, VALUE str) {
  return rb_str_cat2(str, "?");
}

VALUE ss_str_plus(VALUE self, VALUE str1, VALUE str2) {
  return rb_str_plus(str1, str2);
}

VALUE ss_phrase_as_question(VALUE self, VALUE str) {
  const char *question_mark = "?";
  rb_str_buf_cat(str, question_mark, strlen(question_mark));
  return str;
}

VALUE ss_rb_str_new2_with_null(VALUE self) {
  return rb_str_new2(NULL);
}

VALUE ss_str_cmp(VALUE self, VALUE str1, VALUE str2) {
  return INT2NUM(rb_str_cmp(str1, str2));
}

VALUE ss_str_split(VALUE self, VALUE str) {
  return rb_str_split(str, ",");
}

VALUE ss_str2inum(VALUE self, VALUE str, VALUE inum) {
  int num = FIX2INT(inum);
  return rb_str2inum(str, num);
}

VALUE ss_cstr2inum(VALUE self, VALUE inum) {
  int num = FIX2INT(inum);
  return rb_cstr2inum("10", num);
}

VALUE ss_str_substr(VALUE self, VALUE str, VALUE beg, VALUE len) {
  return rb_str_substr(str, FIX2INT(beg), FIX2INT(len));
}

VALUE ss_rstring_see(VALUE self, VALUE str) {
  return rb_str_new2(RSTRING(str)->ptr);
}

VALUE ss_rstring_assign_foo(VALUE self, VALUE str) {
  RSTRING(str)->len = 3;
  RSTRING(str)->ptr = ALLOC_N(char, 4);
  memcpy(RSTRING(str)->ptr, "foo", 4);
  return Qnil;
}

VALUE ss_rstring_assign_global_foobar(VALUE self) {
  VALUE var = rb_gv_get("$global_rstring_test");
  RSTRING(var)->len = 6;
  RSTRING(var)->ptr = ALLOC_N(char, 7);
  memcpy(RSTRING(var)->ptr, "foobar", 7);
  return Qnil;
}

VALUE ss_rstring_set_len(VALUE self, VALUE str, VALUE len) {
  RSTRING(str)->len = NUM2INT(len);
  return Qnil;
}

VALUE ss_rstring_assign_foo_and_upcase(VALUE self, VALUE str) {
  RSTRING(str)->len = 3;
  RSTRING(str)->ptr = ALLOC_N(char, 4);
  memcpy(RSTRING(str)->ptr, "foo", 4);
  rb_funcall(str, rb_intern("upcase!"), 0, 0);
  return Qnil;
}

VALUE ss_str_to_str(VALUE self, VALUE arg) {
  return rb_str_to_str(arg);
}

void Init_string_spec() {
  VALUE cls;
  cls = rb_define_class("CApiStringSpecs", rb_cObject);
  rb_define_method(cls, "say_hello", ss_say_hello, 0);
  rb_define_method(cls, "say_hello2", ss_say_hello2, 0);
  rb_define_method(cls, "rb_str_new2_with_null", ss_rb_str_new2_with_null, 0);
  rb_define_method(cls, "dup_string", ss_dup_string, 1);
  rb_define_method(cls, "rb_str_append", ss_str_append, 2);
  rb_define_method(cls, "rb_str_plus", ss_str_plus, 2);
  rb_define_method(cls, "cat_as_question", ss_cat_as_question, 1);
  rb_define_method(cls, "cat2_as_question", ss_cat2_as_question, 1);
  rb_define_method(cls, "phrase_as_question", ss_phrase_as_question, 1);
  rb_define_method(cls, "rb_str_cmp", ss_str_cmp, 2);
  rb_define_method(cls, "rb_str_split", ss_str_split, 1);
  rb_define_method(cls, "rb_str2inum", ss_str2inum, 2);
  rb_define_method(cls, "rb_cstr2inum", ss_cstr2inum, 1);
  rb_define_method(cls, "rb_str_substr", ss_str_substr, 3);
  rb_define_method(cls, "rb_rstring_see", ss_rstring_see, 1);
  rb_define_method(cls, "rb_rstring_assign_foo", ss_rstring_assign_foo, 1);
  rb_define_method(cls, "rb_rstring_assign_global_foobar", ss_rstring_assign_global_foobar, 0);
  rb_define_method(cls, "rb_rstring_set_len", ss_rstring_set_len, 2);
  rb_define_method(cls, "rb_rstring_assign_foo_and_upcase", ss_rstring_assign_foo_and_upcase, 1);
  rb_define_method(cls, "rb_str_to_str", ss_str_to_str, 1);
}
