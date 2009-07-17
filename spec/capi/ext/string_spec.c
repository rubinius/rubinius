#include "ruby.h"
#include <string.h>

VALUE string_spec_rb_str_new(VALUE self) {
  const char *str = "hello";
  return rb_str_new(str, strlen(str));
}

VALUE string_spec_rb_str_new2(VALUE self) {
  return rb_str_new2("hello\0invisible");
}

VALUE string_spec_rb_str_new2_with_null(VALUE self) {
  return rb_str_new2(NULL);
}

VALUE string_spec_rb_str_dup(VALUE self, VALUE str) {
  return rb_str_dup(str);
}

VALUE string_spec_rb_str_append(VALUE self, VALUE str, VALUE str2) {
  return rb_str_append(str, str2);
}

VALUE string_spec_rb_str_cat(VALUE self, VALUE str) {
  return rb_str_cat(str, "?", 1);
}

VALUE string_spec_rb_str_cat2(VALUE self, VALUE str) {
  return rb_str_cat2(str, "?");
}

VALUE string_spec_rb_str_plus(VALUE self, VALUE str1, VALUE str2) {
  return rb_str_plus(str1, str2);
}

VALUE string_spec_rb_str_buf_cat(VALUE self, VALUE str) {
  const char *question_mark = "?";
  rb_str_buf_cat(str, question_mark, strlen(question_mark));
  return str;
}

VALUE string_spec_rb_str_cmp(VALUE self, VALUE str1, VALUE str2) {
  return INT2NUM(rb_str_cmp(str1, str2));
}

VALUE string_spec_rb_str_split(VALUE self, VALUE str) {
  return rb_str_split(str, ",");
}

VALUE string_spec_rb_str2inum(VALUE self, VALUE str, VALUE inum) {
  int num = FIX2INT(inum);
  return rb_str2inum(str, num);
}

VALUE string_spec_rb_cstr2inum(VALUE self, VALUE str, VALUE inum) {
  int num = FIX2INT(inum);
  return rb_cstr2inum(RSTRING_PTR(str), num);
}

VALUE string_spec_rb_str_substr(VALUE self, VALUE str, VALUE beg, VALUE len) {
  return rb_str_substr(str, FIX2INT(beg), FIX2INT(len));
}

VALUE string_spec_rb_str_to_str(VALUE self, VALUE arg) {
  return rb_str_to_str(arg);
}

VALUE string_spec_RSTRING_PTR_iterate(VALUE self, VALUE str) {
  int i;
  char* ptr;

  ptr = RSTRING_PTR(str);
  for(i = 0; i < RSTRING_LEN(str); i++) {
    rb_yield(INT2FIX(ptr[i]));
  }
  return Qnil;
}

VALUE string_spec_RSTRING_PTR_assign(VALUE self, VALUE str, VALUE chr) {
  int i;
  char c;
  char* ptr;

  ptr = RSTRING_PTR(str);
  c = FIX2INT(chr);

  for(i = 0; i < RSTRING_LEN(str); i++) {
    ptr[i] = c;
  }
  return Qnil;
}

VALUE string_spec_RSTRING_LEN(VALUE self, VALUE str) {
  return INT2FIX(RSTRING_LEN(str));
}

VALUE string_spec_RSTRING_len(VALUE self, VALUE str) {
  return INT2FIX(RSTRING(str)->len);
}

VALUE string_spec_RSTRING_ptr_iterate(VALUE self, VALUE str) {
  int i;
  char* ptr;

  ptr = RSTRING(str)->ptr;
  for(i = 0; i < RSTRING_LEN(str); i++) {
    rb_yield(INT2FIX(ptr[i]));
  }
  return Qnil;
}

VALUE string_spec_RSTRING_ptr_assign(VALUE self, VALUE str, VALUE chr) {
  int i;
  char c;
  char* ptr;

  ptr = RSTRING(str)->ptr;
  c = FIX2INT(chr);

  for(i = 0; i < RSTRING_LEN(str); i++) {
    ptr[i] = c;
  }
  return Qnil;
}

VALUE string_spec_RSTRING_ptr_assign_call(VALUE self, VALUE str) {
  char *ptr = RSTRING(str)->ptr;

  ptr[1] = 'x';
  rb_str_concat(str, rb_str_new2("d"));
  return str;
}

VALUE string_spec_RSTRING_ptr_assign_funcall(VALUE self, VALUE str) {
  char *ptr = RSTRING(str)->ptr;

  ptr[1] = 'x';
  rb_funcall(str, rb_intern("<<"), 1, rb_str_new2("e"));
  return str;
}

VALUE string_spec_StringValue(VALUE self, VALUE str) {
  return StringValue(str);
}

VALUE string_spec_rb_str_resize(VALUE self, VALUE str, VALUE size) {
  return rb_str_resize(str, FIX2INT(size));
}

VALUE string_spec_rb_str_resize_RSTRING_LEN(VALUE self, VALUE str, VALUE size) {
  VALUE modified = rb_str_resize(str, FIX2INT(size));
  return INT2FIX(RSTRING_LEN(modified));
}

VALUE string_spec_rb_str_intern(VALUE self, VALUE str) {
  return rb_str_intern(str);
}

VALUE string_spec_rb_str2cstr(VALUE self, VALUE str, VALUE return_length) {
  if(return_length == Qtrue) {
    long len = 0;
    char* ptr = rb_str2cstr(str, &len);
    VALUE ary = rb_ary_new();
    rb_ary_push(ary, rb_str_new2(ptr));
    rb_ary_push(ary, INT2FIX(len));
    return ary;
  } else {
    return rb_str_new2(rb_str2cstr(str, NULL));
  }
}

VALUE string_spec_rb_str2cstr_replace(VALUE self, VALUE str) {
  char* ptr = rb_str2cstr(str, NULL);
  ptr[0] = 'f'; ptr[1] = 'o'; ptr[2] = 'o'; ptr[3] = 0;
  return Qnil;
}

VALUE string_spec_STR2CSTR(VALUE self, VALUE str) {
  return rb_str_new2(STR2CSTR(str));
}

VALUE string_spec_STR2CSTR_replace(VALUE self, VALUE str) {
  char* ptr = STR2CSTR(str);
  ptr[0] = 'f'; ptr[1] = 'o'; ptr[2] = 'o'; ptr[3] = 0;
  return Qnil;
}

#ifdef RUBINIUS
VALUE string_spec_rb_str_ptr_iterate(VALUE self, VALUE str) {
  int i;
  char* ptr;

  ptr = rb_str_ptr(str);
  for(i = 0; i < RSTRING_LEN(str); i++) {
    rb_yield(INT2FIX(ptr[i]));
  }
  return Qnil;
}

VALUE string_spec_rb_str_ptr_assign(VALUE self, VALUE str, VALUE chr) {
  int i;
  char c;
  char* ptr;

  ptr = rb_str_ptr(str);
  c = FIX2INT(chr);

  for(i = 0; i < RSTRING_LEN(str); i++) {
    ptr[i] = c;
  }
  return Qnil;
}

VALUE string_spec_rb_str_ptr_convert(VALUE self, VALUE str, VALUE more) {
  char* ptr1 = rb_str_ptr_readonly(str);
  char* ptr2 = rb_str_ptr(str);

  rb_str_concat(str, more);

  return rb_str_new2(ptr2);
}

VALUE string_spec_rb_str_ptr_assign_call(VALUE self, VALUE str) {
  char *ptr = rb_str_ptr(str);

  ptr[1] = 'x';
  rb_str_concat(str, rb_str_new2("d"));
  return str;
}

VALUE string_spec_rb_str_ptr_assign_funcall(VALUE self, VALUE str) {
  char *ptr = rb_str_ptr(str);

  ptr[1] = 'x';
  rb_str_flush(str);
  rb_funcall(str, rb_intern("<<"), 1, rb_str_new2("e"));
  return str;
}

VALUE string_spec_rb_str_ptr_readonly_iterate(VALUE self, VALUE str) {
  int i;
  char* ptr;

  ptr = rb_str_ptr_readonly(str);
  for(i = 0; i < RSTRING_LEN(str); i++) {
    rb_yield(INT2FIX(ptr[i]));
  }
  return Qnil;
}

VALUE string_spec_rb_str_ptr_readonly_assign(VALUE self, VALUE str, VALUE chr) {
  int i;
  char c;
  char* ptr;

  ptr = rb_str_ptr_readonly(str);
  c = FIX2INT(chr);

  for(i = 0; i < RSTRING_LEN(str); i++) {
    ptr[i] = c;
  }
  return Qnil;
}

VALUE string_spec_rb_str_ptr_readonly_append(VALUE self, VALUE str, VALUE more) {
  char *ptr = rb_str_ptr_readonly(str);

  rb_str_concat(str, more);

  return rb_str_new2(ptr);
}

VALUE string_spec_rb_str_flush_writable(VALUE self, VALUE str) {
  char *ptr = rb_str_ptr(str);

  ptr[1] = 'B';
  rb_str_flush(str);

  return Qnil;
}

VALUE string_spec_rb_str_flush_readonly(VALUE self, VALUE str) {
  char *ptr = rb_str_ptr_readonly(str);

  ptr[1] = 'b';
  rb_str_flush(str);

  return Qnil;
}

VALUE string_spec_rb_str_update_writable(VALUE self, VALUE str, VALUE more) {
  char *ptr = rb_str_ptr(str);

  rb_str_concat(str, more);
  rb_str_update(str);

  return rb_str_new2(ptr);
}

VALUE string_spec_rb_str_update_readonly(VALUE self, VALUE str) {
  char *ptr = rb_str_ptr_readonly(str);

  ptr[0] = 'q';
  ptr[1] = 'r';
  ptr[2] = 's';
  rb_str_update(str);

  return rb_str_new2(ptr);
}

VALUE string_spec_rb_str_len(VALUE self, VALUE str) {
  return INT2FIX(rb_str_len(str));
}
#endif

void Init_string_spec() {
  VALUE cls;
  cls = rb_define_class("CApiStringSpecs", rb_cObject);

  rb_define_method(cls, "rb_str_new", string_spec_rb_str_new, 0);
  rb_define_method(cls, "rb_str_new2", string_spec_rb_str_new2, 0);
  rb_define_method(cls, "rb_str_new2_with_null", string_spec_rb_str_new2_with_null, 0);
  rb_define_method(cls, "rb_str_dup", string_spec_rb_str_dup, 1);
  rb_define_method(cls, "rb_str_append", string_spec_rb_str_append, 2);
  rb_define_method(cls, "rb_str_plus", string_spec_rb_str_plus, 2);
  rb_define_method(cls, "rb_str_cat", string_spec_rb_str_cat, 1);
  rb_define_method(cls, "rb_str_cat2", string_spec_rb_str_cat2, 1);
  rb_define_method(cls, "rb_str_buf_cat", string_spec_rb_str_buf_cat, 1);
  rb_define_method(cls, "rb_str_cmp", string_spec_rb_str_cmp, 2);
  rb_define_method(cls, "rb_str_split", string_spec_rb_str_split, 1);
  rb_define_method(cls, "rb_str2inum", string_spec_rb_str2inum, 2);
  rb_define_method(cls, "rb_cstr2inum", string_spec_rb_cstr2inum, 2);
  rb_define_method(cls, "rb_str_substr", string_spec_rb_str_substr, 3);
  rb_define_method(cls, "rb_str_to_str", string_spec_rb_str_to_str, 1);
  rb_define_method(cls, "rb_str_resize", string_spec_rb_str_resize, 2);
  rb_define_method(cls, "rb_str_resize_RSTRING_LEN",
      string_spec_rb_str_resize_RSTRING_LEN, 2);
  rb_define_method(cls, "RSTRING_PTR_iterate", string_spec_RSTRING_PTR_iterate, 1);
  rb_define_method(cls, "RSTRING_PTR_assign", string_spec_RSTRING_PTR_assign, 2);
  rb_define_method(cls, "RSTRING_LEN", string_spec_RSTRING_LEN, 1);
  rb_define_method(cls, "RSTRING_ptr_iterate", string_spec_RSTRING_ptr_iterate, 1);
  rb_define_method(cls, "RSTRING_ptr_assign", string_spec_RSTRING_ptr_assign, 2);
  rb_define_method(cls, "RSTRING_ptr_assign_call", string_spec_RSTRING_ptr_assign_call, 1);
  rb_define_method(cls, "RSTRING_ptr_assign_funcall",
      string_spec_RSTRING_ptr_assign_funcall, 1);
  rb_define_method(cls, "RSTRING_len", string_spec_RSTRING_len, 1);
  rb_define_method(cls, "StringValue", string_spec_StringValue, 1);
  rb_define_method(cls, "rb_str_intern", string_spec_rb_str_intern, 1);
  rb_define_method(cls, "rb_str2cstr", string_spec_rb_str2cstr, 2);
  rb_define_method(cls, "rb_str2cstr_replace", string_spec_rb_str2cstr_replace, 1);
  rb_define_method(cls, "STR2CSTR", string_spec_STR2CSTR, 1);
  rb_define_method(cls, "STR2CSTR_replace", string_spec_STR2CSTR_replace, 1);

#ifdef RUBINIUS
  rb_define_method(cls, "rb_str_ptr_iterate", string_spec_rb_str_ptr_iterate, 1);
  rb_define_method(cls, "rb_str_ptr_assign", string_spec_rb_str_ptr_assign, 2);
  rb_define_method(cls, "rb_str_ptr_assign_call", string_spec_rb_str_ptr_assign_call, 1);
  rb_define_method(cls, "rb_str_ptr_convert", string_spec_rb_str_ptr_convert, 2);
  rb_define_method(cls, "rb_str_ptr_assign_funcall",
      string_spec_rb_str_ptr_assign_funcall, 1);
  rb_define_method(cls, "rb_str_len", string_spec_rb_str_len, 1);
  rb_define_method(cls, "rb_str_ptr_readonly_iterate",
      string_spec_rb_str_ptr_readonly_iterate, 1);
  rb_define_method(cls, "rb_str_ptr_readonly_assign",
      string_spec_rb_str_ptr_readonly_assign, 2);
  rb_define_method(cls, "rb_str_ptr_readonly_append",
      string_spec_rb_str_ptr_readonly_append, 2);
  rb_define_method(cls, "rb_str_flush_writable", string_spec_rb_str_flush_writable, 1);
  rb_define_method(cls, "rb_str_flush_readonly", string_spec_rb_str_flush_readonly, 1);
  rb_define_method(cls, "rb_str_update_writable", string_spec_rb_str_update_writable, 2);
  rb_define_method(cls, "rb_str_update_readonly", string_spec_rb_str_update_readonly, 1);
#endif
}
