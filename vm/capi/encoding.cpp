#include "capi/include/ruby.h"
#include "capi/include/encoding.h"

extern "C" {
  rb_encoding* rb_utf8_encoding(void) {
    /* TODO
    if(!enc_table.list) {
      rb_enc_init();
    }
    return enc_table.list[ENCINDEX_UTF_8].enc;
    */
    return 0;
  }

  rb_encoding* rb_usascii_encoding(void) {
    /* TODO
    if (!enc_table.list) {
      rb_enc_init();
    }
    return enc_table.list[ENCINDEX_US_ASCII].enc;
    */
    return 0;
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
