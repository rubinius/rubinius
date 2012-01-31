#ifndef MEL_ENCODING_COMPAT_HPP
#define MEL_ENCODING_COMPAT_HPP

#ifdef __cplusplus
extern "C" {
#endif
#include "melbourne.hpp"

#ifdef HAVE_RUBY_ENCODING_H
#include "ruby/encoding.h"
#else

#define ENCODING_NAMELEN_MAX 63

typedef struct {
  const char name[ENCODING_NAMELEN_MAX];
} rb_encoding;

#define ENC_CODERANGE_UNKNOWN               0
#define ENC_CODERANGE_7BIT                  1

#define MBCLEN_CHARFOUND_P(ret)             1

#endif

int parser_enc_isalnum(int c, rb_encoding* enc);
int parser_enc_isascii(int c, rb_encoding* enc);
int parser_enc_isspace(int c, rb_encoding* enc);
int parser_enc_ispunct(int c, rb_encoding* enc);
int parser_enc_isupper(int c, rb_encoding* enc);

int parser_enc_precise_mbclen(const char* p, const char* e, rb_encoding* enc);
int parser_enc_codelen(int c, rb_encoding* enc);
int parser_enc_mbclen(const char* p, const char* e, rb_encoding* enc);
int parser_enc_asciicompat(rb_encoding* enc);
int parser_enc_str_coderange(VALUE str);
int parser_enc_find_index(const char* name);

void parser_enc_mbcput(int c, const char* ptr, rb_encoding* enc);

const char* parser_enc_name(rb_encoding* enc);

VALUE parser_enc_str_new(const char* ptr, long len, rb_encoding* enc);
VALUE parser_enc_associate(VALUE obj, rb_encoding* enc);

rb_encoding* parser_enc_get(VALUE obj);
rb_encoding* parser_enc_compatible(VALUE str1, VALUE str2);
rb_encoding* parser_enc_from_index(int index);
rb_encoding* parser_utf8_encoding();
rb_encoding* parser_usascii_encoding();
rb_encoding* parser_ascii8bit_encoding();

#ifdef __cplusplus
}  /* extern "C" { */
#endif

#endif
