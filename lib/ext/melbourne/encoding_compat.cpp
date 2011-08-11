#include "encoding_compat.hpp"

extern "C" {
  rb_encoding* rb_utf8_encoding() {
    static rb_encoding enc = { "UTF-8" };

    return &enc;
  }

  rb_encoding* rb_usascii_encoding() {
    static rb_encoding enc = { "US-ASCII" };

    return &enc;
  }

  rb_encoding* rb_ascii8bit_encoding() {
    static rb_encoding enc = { "ASCII-8BIT" };

    return &enc;
  }

  ID rb_intern_str(VALUE str) {
    return SYM2ID(rb_funcall(str, rb_intern("to_sym"), 0));
  }
}
