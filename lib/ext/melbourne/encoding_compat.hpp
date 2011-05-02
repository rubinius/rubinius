#ifndef MEL_ENCODING_COMPAT_HPP
#define MEL_ENCODING_COMPAT_HPP

#ifdef HAVE_RUBY_ENCODING_H
#include "encoding.h"
#else

typedef struct {
  const char* name;
} rb_encoding;

#define ENC_CODERANGE_UNKNOWN	    0
#define ENC_CODERANGE_7BIT	      1

#define MBCLEN_CHARFOUND_P(ret)   0

#define rb_enc_isalnum(c, enc)              ISALNUM(c)
#define rb_enc_isspace(c, enc)              ISSPACE(c)
#define rb_enc_precise_mbclen(a, b, enc)    1
#define rb_enc_codelen(c, enc)              1
#define rb_enc_mbcput(c, l, enc)            (0)

#define rb_usascii_encoding()     0
#define rb_utf8_encoding()        0
#define rb_enc_name(enc)          "US-ASCII"

#endif

#endif
