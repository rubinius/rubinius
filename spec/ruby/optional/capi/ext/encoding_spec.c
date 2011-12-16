#include "ruby.h"
#include "rubyspec.h"

#include "ruby/encoding.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_RB_USASCII_ENCODING
static VALUE encoding_spec_rb_usascii_encoding(VALUE self) {
  return rb_str_new2(rb_usascii_encoding()->name);
}
#endif

#ifdef HAVE_RB_ASCII8BIT_ENCODING
static VALUE encoding_spec_rb_ascii8bit_encoding(VALUE self) {
  return rb_str_new2(rb_ascii8bit_encoding()->name);
}
#endif

#ifdef HAVE_RB_UTF8_ENCODING
static VALUE encoding_spec_rb_utf8_encoding(VALUE self) {
  return rb_str_new2(rb_utf8_encoding()->name);
}
#endif

void Init_encoding_spec() {
  VALUE cls;
  cls = rb_define_class("CApiEncodingSpecs", rb_cObject);

#ifdef HAVE_RB_USASCII_ENCODING
  rb_define_method(cls, "rb_usascii_encoding", encoding_spec_rb_usascii_encoding, 0);
#endif

#ifdef HAVE_RB_ASCII8BIT_ENCODING
  rb_define_method(cls, "rb_ascii8bit_encoding", encoding_spec_rb_ascii8bit_encoding, 0);
#endif

#ifdef HAVE_RB_UTF8_ENCODING
  rb_define_method(cls, "rb_utf8_encoding", encoding_spec_rb_utf8_encoding, 0);
#endif

}

#ifdef __cplusplus
}
#endif
