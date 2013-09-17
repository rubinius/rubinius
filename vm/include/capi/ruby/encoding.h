#ifndef RBX_CAPI_ENCODING_H
#define RBX_CAPI_ENCODING_H

#ifdef __cplusplus
extern "C" {
#endif

#include "oniguruma.h"
#include "transcoder.h"

typedef OnigEncodingType rb_encoding;

#define ENCODING_INLINE_MAX 1023
#define ENCODING_GET_INLINED(obj)   rb_enc_get_index(obj)

#define ENC_CODERANGE_UNKNOWN       0
#define ENC_CODERANGE_7BIT          1
#define ENC_CODERANGE_VALID         2
#define ENC_CODERANGE_BROKEN        4

#define MBCLEN_CHARFOUND_P(ret)     ONIGENC_MBCLEN_CHARFOUND_P(ret)

#define rb_enc_isctype(c, t, enc)   ONIGENC_IS_CODE_CTYPE(enc, c, t)
#define rb_enc_isascii(c, enc)      ONIGENC_IS_CODE_ASCII(c)
#define rb_enc_isalpha(c, enc)      ONIGENC_IS_CODE_ALPHA(enc, c)
#define rb_enc_islower(c, enc)      ONIGENC_IS_CODE_LOWER(enc, c)
#define rb_enc_isupper(c, enc)      ONIGENC_IS_CODE_UPPER(enc, c)
#define rb_enc_ispunct(c, enc)      ONIGENC_IS_CODE_PUNCT(enc, c)
#define rb_enc_isalnum(c, enc)      ONIGENC_IS_CODE_ALNUM(enc, c)
#define rb_enc_isprint(c, enc)      ONIGENC_IS_CODE_PRINT(enc, c)
#define rb_enc_isspace(c, enc)      ONIGENC_IS_CODE_SPACE(enc, c)
#define rb_enc_isdigit(c, enc)      ONIGENC_IS_CODE_DIGIT(enc, c)

#define rb_enc_mbcput(c, buf, enc)  ONIGENC_CODE_TO_MBC(enc,c,(UChar*)(buf))
#define rb_enc_asciicompat(enc)     (rb_enc_mbminlen(enc)==1 && !rb_enc_dummy_p(enc))

/* start, ptr, end, encoding -> prev_char */
#define rb_enc_prev_char(s,p,e,enc) \
    ((char *)onigenc_get_prev_char_head((enc),(UChar*)(s),(UChar*)(p),(UChar*)(e)))
/* start, ptr, end, encoding -> next_char */
#define rb_enc_left_char_head(s,p,e,enc) \
    ((char *)onigenc_get_left_adjust_char_head((enc),(UChar*)(s),(UChar*)(p),(UChar*)(e)))
#define rb_enc_right_char_head(s,p,e,enc) \
    ((char *)onigenc_get_right_adjust_char_head((enc),(UChar*)(s),(UChar*)(p),(UChar*)(e)))
#define rb_enc_step_back(s,p,e,n,enc) \
    ((char *)onigenc_step_back((enc),(UChar*)(s),(UChar*)(p),(UChar*)(e),(int)(n)))

#define rb_enc_name(enc)            (enc)->name
#define rb_enc_mbminlen(enc)        (enc)->min_enc_len
#define rb_enc_mbmaxlen(enc)        (enc)->max_enc_len

#define rb_usascii_str_new2 rb_usascii_str_new_cstr

#define ENCODING_GET(obj) rb_enc_get_index(obj)
#define ENCODING_SET(obj, i) rb_enc_set_index(obj, i)

int rb_encdb_alias(const char *alias, const char *orig);

int rb_enc_mbclen(const char *p, const char *e, rb_encoding *enc);
int rb_enc_precise_mbclen(const char *p, const char *e, rb_encoding *enc);
int rb_enc_codelen(int code, rb_encoding *enc);

rb_encoding* rb_utf8_encoding(void);
rb_encoding* rb_usascii_encoding(void);
rb_encoding* rb_ascii8bit_encoding(void);
rb_encoding* rb_locale_encoding(void);
rb_encoding* rb_filesystem_encoding(void);

int rb_enc_coderange_asciionly_p(VALUE obj);
#define ENC_CODERANGE_ASCIIONLY     rb_enc_coderange_asciionly_p

int rb_utf8_encindex(void);
int rb_usascii_encindex(void);
int rb_ascii8bit_encindex(void);
int rb_locale_encindex(void);
int rb_filesystem_encindex(void);
int rb_enc_get_index(VALUE obj);
VALUE rb_obj_encoding(VALUE obj);
void rb_enc_set_index(VALUE obj, int index);
int rb_to_encoding_index(VALUE obj);

rb_encoding* rb_enc_find(const char* name);
rb_encoding* rb_enc_get(VALUE obj);
rb_encoding* rb_enc_compatible(VALUE str1, VALUE str2);
rb_encoding* rb_enc_from_index(int idx);
rb_encoding* rb_default_internal_encoding(void);
rb_encoding* rb_default_external_encoding(void);
rb_encoding* rb_to_encoding(VALUE obj);

VALUE rb_enc_from_encoding(rb_encoding *enc);

unsigned int rb_enc_codepoint_len(const char *p, const char *e, int *len_p, rb_encoding *enc);

int rb_define_dummy_encoding(const char *);
int rb_enc_find_index(const char *name);
int rb_enc_to_index(rb_encoding* enc);
char* rb_enc_nth(const char *p, const char *e, long nth, rb_encoding *enc);

int rb_enc_dummy_p(rb_encoding *enc);
VALUE rb_enc_associate(VALUE, rb_encoding*);
VALUE rb_enc_associate_index(VALUE, int);
void rb_enc_copy(VALUE dest, VALUE src);

VALUE rb_enc_str_new(const char*, long, rb_encoding*);
VALUE rb_usascii_str_new(const char* ptr, long len);
VALUE rb_usascii_str_new2(const char* ptr);
int rb_enc_str_coderange(VALUE);

VALUE rb_external_str_new_with_enc(const char *ptr, long len, rb_encoding *);
VALUE rb_str_export_to_enc(VALUE, rb_encoding *);
VALUE rb_str_conv_enc(VALUE str, rb_encoding *from, rb_encoding *to);
VALUE rb_str_conv_enc_opts(VALUE str, rb_encoding *from, rb_encoding *to, int ecflags, VALUE ecopts);

VALUE rb_str_encode(VALUE str, VALUE to, int ecflags, VALUE ecopts);

ID rb_intern3(const char* string, long len, rb_encoding* enc);

void rb_declare_transcoder(const char* from, const char* to, const char* lib);
void rb_register_transcoder(const rb_transcoder* trans);

#ifdef __cplusplus
}
#endif

#endif
