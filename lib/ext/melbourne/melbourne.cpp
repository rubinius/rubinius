/* We are well-behaved citizens of the C-extension State
 * so we let the C-API know this so it will not penalize
 * us with unnecessary caching.
 */
#define RSTRING_NOT_MODIFIED 1

#include "ruby.h"
#include "grammar18.hpp"
#include "grammar19.hpp"
#include "symbols.hpp"

#include <unistd.h>
#include <fcntl.h>

#ifdef __cplusplus
extern "C" {
#endif

VALUE melbourne18_string_to_ast(VALUE self, VALUE source, VALUE name, VALUE line) {
  StringValue(source);
  StringValue(name);
  bstring b_str = blk2bstr(RSTRING_PTR(source), RSTRING_LEN(source));
  VALUE result = melbourne::grammar18::string_to_ast(self,
      RSTRING_PTR(name), b_str, FIX2INT(line));
  bdestroy(b_str);

  return result;
}

VALUE melbourne18_file_to_ast(VALUE self, VALUE fname, VALUE start) {
  StringValue(fname);

  FILE *file = fopen(RSTRING_PTR(fname), "r");
  if(file) {
    VALUE result = melbourne::grammar18::file_to_ast(self,
        RSTRING_PTR(fname), file, FIX2INT(start));
    fclose(file);

    return result;
  } else {
    rb_raise(rb_eLoadError, "no such file to load -- %s", RSTRING_PTR(fname));
  }
}

VALUE melbourne19_string_to_ast(VALUE self, VALUE source, VALUE name, VALUE line) {
  StringValue(source);
  StringValue(name);
  VALUE result = melbourne::grammar19::string_to_ast(self, name, source, line);

  return result;
}

VALUE melbourne19_file_to_ast(VALUE self, VALUE fname, VALUE start) {
  StringValue(fname);

  int fd = open(RSTRING_PTR(fname), O_RDONLY);
  if(fd) {
    VALUE result = melbourne::grammar19::file_to_ast(self,
        RSTRING_PTR(fname), fd, FIX2INT(start));
    close(fd);

    return result;
  } else {
    rb_raise(rb_eLoadError, "no such file to load -- %s", RSTRING_PTR(fname));
  }
}

void Init_melbourne(void) {
  VALUE rb_cMelbourne;

  melbourne::init_symbols();

#ifndef RUBINIUS
  VALUE rb_mRubinius = rb_const_get(rb_cObject, rb_intern("Rubinius"));
#endif

  rb_cMelbourne = rb_define_class_under(rb_mRubinius, "Melbourne", rb_cObject);
  rb_define_method(rb_cMelbourne, "string_to_ast_18",
      RUBY_METHOD_FUNC(melbourne18_string_to_ast), 3);
  rb_define_method(rb_cMelbourne, "file_to_ast_18",
      RUBY_METHOD_FUNC(melbourne18_file_to_ast), 2);
  rb_define_method(rb_cMelbourne, "string_to_ast_19",
      RUBY_METHOD_FUNC(melbourne19_string_to_ast), 3);
  rb_define_method(rb_cMelbourne, "file_to_ast_19",
      RUBY_METHOD_FUNC(melbourne19_file_to_ast), 2);
}

#ifdef __cplusplus
}  /* extern "C" { */
#endif
