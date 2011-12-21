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

#ifdef HAVE_RB_DEFAULT_INTERNAL_ENCODING
static VALUE encoding_spec_rb_default_internal_encoding(VALUE self) {
  rb_encoding* enc = rb_default_internal_encoding();
  if(enc == 0) return Qnil;
  return rb_str_new2(enc->name);
}
#endif

#ifdef HAVE_RB_ENC_COPY
static VALUE encoding_spec_rb_enc_copy(VALUE self, VALUE dest, VALUE src) {
  rb_enc_copy(dest, src);
  return dest;
}
#endif

#ifdef HAVE_RB_ENC_FIND_INDEX
static VALUE encoding_spec_rb_enc_find_index(VALUE self, VALUE name) {
  return INT2NUM(rb_enc_find_index(RSTRING_PTR(name)));
}
#endif

#ifdef HAVE_RB_ENC_FROM_INDEX
static VALUE encoding_spec_rb_enc_from_index(VALUE self, VALUE index) {
  return rb_str_new2(rb_enc_from_index(NUM2INT(index))->name);
}
#endif

#ifdef HAVE_RB_ENC_FROM_ENCODING
static VALUE encoding_spec_rb_enc_from_encoding(VALUE self, VALUE name) {
  return rb_enc_from_encoding(rb_enc_find(RSTRING_PTR(name)));
}
#endif

#ifdef HAVE_RB_TO_ENCODING
static VALUE encoding_spec_rb_to_encoding(VALUE self, VALUE obj) {
  return rb_str_new2(rb_to_encoding(obj)->name);
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

#ifdef HAVE_RB_DEFAULT_INTERNAL_ENCODING
  rb_define_method(cls, "rb_default_internal_encoding",
                   encoding_spec_rb_default_internal_encoding, 0);
#endif

#ifdef HAVE_RB_ENC_COPY
  rb_define_method(cls, "rb_enc_copy", encoding_spec_rb_enc_copy, 2);
#endif

#ifdef HAVE_RB_ENC_FIND_INDEX
  rb_define_method(cls, "rb_enc_find_index", encoding_spec_rb_enc_find_index, 1);
#endif

#ifdef HAVE_RB_ENC_FROM_INDEX
  rb_define_method(cls, "rb_enc_from_index", encoding_spec_rb_enc_from_index, 1);
#endif

#ifdef HAVE_RB_ENC_FROM_ENCODING
  rb_define_method(cls, "rb_enc_from_encoding", encoding_spec_rb_enc_from_encoding, 1);
#endif

#ifdef HAVE_RB_TO_ENCODING
  rb_define_method(cls, "rb_to_encoding", encoding_spec_rb_to_encoding, 1);
#endif
}

#ifdef __cplusplus
}
#endif
