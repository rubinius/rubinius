#include <ruby.h>
#include <string.h>

VALUE ss_say_hello(VALUE self) {
  const char *str = "hello";
  return rb_str_new(str, strlen(str));
}

VALUE ss_dup_string(VALUE self, VALUE str) {
  return rb_str_dup(str);
}

VALUE ss_phrase_as_question(VALUE self, VALUE str) {
  const char *question_mark = "?";
  rb_str_buf_cat(str, question_mark, strlen(question_mark));
  return str;
}


void Init_subtend_string() {
  VALUE cls;
  cls = rb_define_class("SubtendString", rb_cObject);
  rb_define_method(cls, "say_hello", ss_say_hello, 0);
  rb_define_method(cls, "dup_string", ss_dup_string, 1);
  rb_define_method(cls, "phrase_as_question", ss_phrase_as_question, 1);
}
