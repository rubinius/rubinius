#include "encoding_compat.hpp"

extern "C" {

#ifdef HAVE_RUBY_ENCODING_H

  int parser_enc_isalnum(int c, rb_encoding* enc) {
    return rb_enc_isalnum(c, enc);
  }

  int parser_enc_isascii(int c, rb_encoding* enc) {
    return rb_enc_isascii(c, enc);
  }

  int parser_enc_isspace(int c, rb_encoding* enc) {
    return rb_enc_isspace(c, enc);
  }

  int parser_enc_ispunct(int c, rb_encoding* enc) {
    return rb_enc_ispunct(c, enc);
  }

  int parser_enc_isupper(int c, rb_encoding* enc) {
    return rb_enc_isupper(c, enc);
  }

  int parser_enc_precise_mbclen(const char* p, const char* e, rb_encoding* enc) {
    return rb_enc_precise_mbclen(p, e, enc);
  }

  int parser_enc_codelen(int c, rb_encoding* enc) {
    return rb_enc_codelen(c, enc);
  }

  int parser_enc_mbclen(const char* p, const char* e, rb_encoding* enc) {
    return rb_enc_mbclen(p, e, enc);
  }

  int parser_enc_asciicompat(rb_encoding* enc) {
    return rb_enc_asciicompat(enc);
  }

  int parser_enc_str_coderange(VALUE str) {
    return rb_enc_str_coderange(str);
  }

  int parser_enc_find_index(const char* name) {
    return rb_enc_find_index(name);
  }

  void parser_enc_mbcput(int c, const char* ptr, rb_encoding* enc) {
    rb_enc_mbcput(c, ptr, enc);
  }

  const char* parser_enc_name(rb_encoding* enc) {
    return rb_enc_name(enc);
  }

  VALUE parser_enc_str_new(const char* ptr, long len, rb_encoding* enc) {
    return rb_enc_str_new(ptr, len, enc);
  }

  VALUE parser_enc_associate(VALUE obj, rb_encoding* enc) {
    return rb_enc_associate(obj, enc);
  }

  rb_encoding* parser_enc_get(VALUE obj) {
    return rb_enc_get(obj);
  }

  rb_encoding* parser_enc_compatible(VALUE str1, VALUE str2) {
    return rb_enc_compatible(str1, str2);
  }

  rb_encoding* parser_enc_from_index(int index) {
    return rb_enc_from_index(index);
  }

  rb_encoding* parser_utf8_encoding() {
    return rb_utf8_encoding();
  }

  rb_encoding* parser_usascii_encoding() {
    return rb_usascii_encoding();
  }

  rb_encoding* parser_ascii8bit_encoding() {
    return rb_ascii8bit_encoding();
  }

#else

  int parser_enc_isalnum(int c, rb_encoding* enc) {
    return isalnum(c);
  }

  int parser_enc_isascii(int c, rb_encoding* enc) {
    return isascii(c);
  }

  int parser_enc_isspace(int c, rb_encoding* enc) {
    return isspace(c);
  }

  int parser_enc_ispunct(int c, rb_encoding* enc) {
    return ispunct(c);
  }

  int parser_enc_isupper(int c, rb_encoding* enc) {
    return isupper(c);
  }

  int parser_enc_precise_mbclen(const char* p, const char* e, rb_encoding* enc) {
    return 1;
  }

  int parser_enc_codelen(int c, rb_encoding* enc) {
    return 1;
  }

  int parser_enc_mbclen(const char* p, const char* e, rb_encoding* enc) {
    return 1;
  }

  int parser_enc_asciicompat(rb_encoding* enc) {
    return 1;
  }

  int parser_enc_str_coderange(VALUE str) {
    return ENC_CODERANGE_7BIT;
  }

  int parser_enc_find_index(const char* name) {
    return 1;
  }

  void parser_enc_mbcput(int c, const char* ptr, rb_encoding* enc) {
    // Do nothing
  }

  const char* parser_enc_name(rb_encoding* enc) {
    return enc->name;
  }

  VALUE parser_enc_str_new(const char* ptr, long len, rb_encoding* enc) {
    return rb_str_new(ptr, len);
  }

  VALUE parser_enc_associate(VALUE obj, rb_encoding* enc) {
    return obj;
  }

  rb_encoding* parser_enc_get(VALUE obj) {
    return parser_usascii_encoding();
  }

  rb_encoding* parser_enc_compatible(VALUE str1, VALUE str2) {
    return parser_enc_get(str1);
  }

  rb_encoding* parser_enc_from_index(int index) {
    return parser_usascii_encoding();
  }

  rb_encoding* parser_utf8_encoding() {
    static rb_encoding enc = { "UTF-8" };

    return &enc;
  }

  rb_encoding* parser_usascii_encoding() {
    static rb_encoding enc = { "US-ASCII" };

    return &enc;
  }

  rb_encoding* parser_ascii8bit_encoding() {
    static rb_encoding enc = { "ASCII-8BIT" };

    return &enc;
  }

  ID rb_intern_str(VALUE str) {
    return SYM2ID(rb_funcall(str, rb_intern("to_sym"), 0));
  }

#endif
}
