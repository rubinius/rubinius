// TODO: resolve after importing Oniguruma from MRI
#include <stddef.h>
#include "capi/19/include/ruby/oniguruma.h"

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
}
