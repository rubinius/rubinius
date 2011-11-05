#ifndef RBX_CAPI_ENCODING_H
#define RBX_CAPI_ENCODING_H

#ifdef __cplusplus
extern "C" {
#endif

#include "oniguruma.h"

typedef OnigEncodingType rb_encoding;

#define ENC_CODERANGE_UNKNOWN	      0
#define ENC_CODERANGE_7BIT	        1

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

#define rb_enc_name(enc)            (enc)->name
#define rb_enc_mbminlen(enc)        (enc)->min_enc_len
#define rb_enc_mbmaxlen(enc)        (enc)->max_enc_len

int rb_enc_precise_mbclen(const char *p, const char *e, rb_encoding *enc);
int rb_enc_codelen(int code, rb_encoding *enc);

rb_encoding *rb_utf8_encoding(void);
rb_encoding *rb_usascii_encoding(void);
rb_encoding *rb_ascii8bit_encoding(void);

rb_encoding* rb_enc_get(VALUE obj);
rb_encoding* rb_enc_compatible(VALUE str1, VALUE str2);
rb_encoding* rb_locale_encoding(void);

int rb_enc_dummy_p(rb_encoding *enc);
VALUE rb_enc_associate(VALUE, rb_encoding*);

VALUE rb_enc_str_new(const char*, long, rb_encoding*);
int rb_enc_str_coderange(VALUE);

VALUE rb_str_conv_enc(VALUE str, rb_encoding *from, rb_encoding *to);

#ifdef __cplusplus
}
#endif

#endif
