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
  const char *question_mark = "?";
  return rb_str_cat(str, "?", 1);
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
  return rb_str_cmp(str1, str2);
}

VALUE ss_str_split(VALUE self, VALUE str) {
  return rb_str_split(str, ",");
}

void Init_subtend_string() {
  VALUE cls;
  cls = rb_define_class("SubtendString", rb_cObject);
  rb_define_method(cls, "say_hello", ss_say_hello, 0);
  rb_define_method(cls, "say_hello2", ss_say_hello2, 0);
  rb_define_method(cls, "rb_str_new2_with_null", ss_rb_str_new2_with_null, 0);
  rb_define_method(cls, "dup_string", ss_dup_string, 1);
  rb_define_method(cls, "rb_str_append", ss_str_append, 2);
  rb_define_method(cls, "rb_str_plus", ss_str_plus, 2);
  rb_define_method(cls, "cat_as_question", ss_cat_as_question, 1);
  rb_define_method(cls, "phrase_as_question", ss_phrase_as_question, 1);
  rb_define_method(cls, "rb_str_cmp", ss_str_cmp, 2);
  rb_define_method(cls, "rb_str_split", ss_str_split, 1);
}
