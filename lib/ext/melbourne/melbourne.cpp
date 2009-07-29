#include "ruby.h"
#include "grammar.hpp"
#include "symbols.hpp"

#ifdef __cplusplus
extern "C" {
#endif

VALUE melbourne_string_to_ast(VALUE self, VALUE source, VALUE name, VALUE line) {
  bstring b_str = blk2bstr(RSTRING_PTR(source), RSTRING_LEN(source));
  VALUE result = melbourne::string_to_ast(self, RSTRING_PTR(name), b_str, FIX2INT(line));
  bdestroy(b_str);

  return result;
}

VALUE melbourne_file_to_ast(VALUE self, VALUE fname, VALUE start) {
  FILE *file = fopen(RSTRING_PTR(fname), "r");
  VALUE result = melbourne::file_to_ast(self, RSTRING_PTR(fname), file, FIX2INT(start));
  fclose(file);

  return result;
}

void Init_melbourne(void) {
  VALUE rb_cMelbourne;

  melbourne::init_symbols();

#ifndef RUBINIUS
  VALUE rb_mRubinius = rb_const_get(rb_cObject, rb_intern("Rubinius"));
#endif

  rb_cMelbourne = rb_define_class_under(rb_mRubinius, "Melbourne", rb_cObject);
  rb_define_method(rb_cMelbourne, "string_to_ast", melbourne_string_to_ast, 3);
  rb_define_method(rb_cMelbourne, "file_to_ast", melbourne_file_to_ast, 2);
}

#ifdef __cplusplus
}  /* extern "C" { */
#endif
