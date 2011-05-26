#ifndef MEL_ENCODING_COMPAT_HPP
#define MEL_ENCODING_COMPAT_HPP

#ifdef __cplusplus
extern "C" {
#endif

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

#define rb_enc_isalnum(c, enc)              ISALNUM(c)
#define rb_enc_isspace(c, enc)              ISSPACE(c)
#define rb_enc_precise_mbclen(a, b, enc)    1
#define rb_enc_codelen(c, enc)              1
#define rb_enc_mbcput(c, l, enc)            (0)
#define rb_enc_str_new(c, l, enc)           rb_str_new(c, l)
#define rb_enc_get(obj)                     rb_usascii_encoding()
#define rb_enc_name(enc)                    (enc)->name
#define rb_enc_compatible(s1, s2)           rb_enc_get(s1)
#define rb_enc_asciicompat(enc)             true
#define rb_enc_str_coderange(str)           ENC_CODERANGE_7BIT
#define rb_enc_associate(str, enc)          (0)

rb_encoding* rb_utf8_encoding();
rb_encoding* rb_usascii_encoding();
rb_encoding* rb_ascii8bit_encoding();

#endif

#ifdef __cplusplus
}  /* extern "C" { */
#endif

#endif
