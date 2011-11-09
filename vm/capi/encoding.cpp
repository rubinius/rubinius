// TODO: resolve after importing Oniguruma from MRI
#include <stddef.h>
#include "capi/19/include/ruby/oniguruma.h"
#include "capi/19/include/ruby/regenc.h"

#include "builtin/encoding.hpp"
#include "builtin/nativemethod.hpp"

#include "capi/19/include/ruby.h"
#include "capi/19/include/ruby/encoding.h"

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {
  rb_encoding* rb_utf8_encoding() {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    return Encoding::utf8_encoding(env->state())->get_encoding();
  }

  rb_encoding* rb_usascii_encoding() {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    return Encoding::ascii_encoding(env->state())->get_encoding();
  }

  rb_encoding* rb_ascii8bit_encoding() {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    // TODO
    return Encoding::ascii_encoding(env->state())->get_encoding();
  }

  rb_encoding* rb_locale_encoding(void)
  {
    // TODO
    return rb_usascii_encoding();
  }

  rb_encoding* rb_enc_get(VALUE obj) {
    // TODO
    return rb_usascii_encoding();
  }

  rb_encoding* rb_enc_compatible(VALUE str1, VALUE str2) {
    // TODO
    return rb_enc_get(str1);
  }

  int rb_enc_dummy_p(rb_encoding *enc) {
    // TODO
    return 0;
  }

  VALUE rb_enc_associate(VALUE obj, rb_encoding *enc) {
    // TODO
    return obj;
  }

  int rb_enc_precise_mbclen(const char* p, const char* e, rb_encoding *enc) {
    int n;
    if(e <= p) {
      return ONIGENC_CONSTRUCT_MBCLEN_NEEDMORE(1);
    }

    n = ONIGENC_PRECISE_MBC_ENC_LEN(enc, (UChar*)p, (UChar*)e);
    if(e-p < n) {
      return ONIGENC_CONSTRUCT_MBCLEN_NEEDMORE(n-(int)(e-p));
    }

    return n;
  }

  int rb_enc_codelen(int c, rb_encoding* enc)
  {
    int n = ONIGENC_CODE_TO_MBCLEN(enc, c);
    if (n == 0) {
      rb_raise(rb_eArgError, "invalid codepoint 0x%x in %s", c, rb_enc_name(enc));
    }
    return n;
  }

#define ctype_test(c, ctype)  (rb_isascii(c) && ONIGENC_IS_ASCII_CODE_CTYPE((c), ctype))

  int rb_isalnum(int c) {
    return ctype_test(c, ONIGENC_CTYPE_ALNUM);
  }

  int rb_isalpha(int c) {
    return ctype_test(c, ONIGENC_CTYPE_ALPHA);
  }

  int rb_isblank(int c) {
    return ctype_test(c, ONIGENC_CTYPE_BLANK);
  }

  int rb_iscntrl(int c) {
    return ctype_test(c, ONIGENC_CTYPE_CNTRL);
  }

  int rb_isdigit(int c) {
    return ctype_test(c, ONIGENC_CTYPE_DIGIT);
  }

  int rb_isgraph(int c) {
    return ctype_test(c, ONIGENC_CTYPE_GRAPH);
  }

  int rb_islower(int c) {
    return ctype_test(c, ONIGENC_CTYPE_LOWER);
  }

  int rb_isprint(int c) {
    return ctype_test(c, ONIGENC_CTYPE_PRINT);
  }

  int rb_ispunct(int c) {
    return ctype_test(c, ONIGENC_CTYPE_PUNCT);
  }

  int rb_isspace(int c) {
    return ctype_test(c, ONIGENC_CTYPE_SPACE);
  }

  int rb_isupper(int c) {
    return ctype_test(c, ONIGENC_CTYPE_UPPER);
  }

  int rb_isxdigit(int c) {
    return ctype_test(c, ONIGENC_CTYPE_XDIGIT);
  }

  int rb_tolower(int c) {
    return rb_isascii(c) ? ONIGENC_ASCII_CODE_TO_LOWER_CASE(c) : c;
  }

  int rb_toupper(int c) {
    return rb_isascii(c) ? ONIGENC_ASCII_CODE_TO_UPPER_CASE(c) : c;
  }
}
