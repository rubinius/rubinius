/* We are well-behaved citizens of the C-extension State
 * so we let the C-API know this so it will not penalize
 * us with unnecessary caching.
 */

#include "melbourne.hpp"
#include "grammar18.hpp"
#include "grammar19.hpp"
#include "symbols.hpp"

#ifdef __cplusplus
extern "C" {
#endif

VALUE melbourne18_string_to_ast(VALUE self, VALUE source, VALUE name, VALUE line) {
  StringValue(source);
  StringValue(name);
  bstring b_str = blk2bstr(RSTRING_PTR(source), (int)RSTRING_LEN(source));
  VALUE result = melbourne::grammar18::string_to_ast(self,
      RSTRING_PTR(name), b_str, FIX2INT(line));
  bdestroy(b_str);

  return result;
}

VALUE melbourne18_file_to_ast(VALUE self, VALUE fname, VALUE start) {
  StringValue(fname);

  char* c_name = RSTRING_PTR(fname);

  FILE *file = fopen(c_name, "r");
  if(file) {
    VALUE result = melbourne::grammar18::file_to_ast(self,
        RSTRING_PTR(fname), file, FIX2INT(start));
    fclose(file);

    return result;
  } else {
    rb_raise(rb_eLoadError, "%s -- %s", strerror(errno), c_name);
    return Qnil;
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
  if(fd != -1) {
    VALUE result = melbourne::grammar19::file_to_ast(self,
        RSTRING_PTR(fname), fd, FIX2INT(start));
    close(fd);

    return result;
  } else {
    rb_raise(rb_eLoadError, "no such file to load -- %s", RSTRING_PTR(fname));
    return Qnil;
  }
}

void Init_melbourne20(void) {
  VALUE rb_cMelbourne;

  melbourne::init_symbols20();

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
