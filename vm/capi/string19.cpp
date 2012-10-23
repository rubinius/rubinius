#include "vm/vm.hpp"

#include "capi/19/include/ruby/ruby.h"
#include "capi/19/include/ruby/encoding.h"

#include "builtin/bytearray.hpp"
#include "builtin/encoding.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/integer.hpp"
#include "builtin/nativemethod.hpp"
#include "builtin/object.hpp"
#include "builtin/string.hpp"
#include "capi/capi.hpp"

#include <string.h>

using namespace rubinius;
using namespace rubinius::capi;

namespace rubinius {
  namespace capi {
  }
}

extern "C" {
  VALUE rb_enc_str_new(const char *ptr, long len, rb_encoding *enc)
  {
    VALUE str = rb_str_new(ptr, len);
    rb_enc_associate(str, enc);
    return str;
  }

  VALUE rb_usascii_str_new(const char* ptr, long len) {
    return rb_enc_str_new(ptr, len, rb_usascii_encoding());
  }

  VALUE rb_usascii_str_new2(const char* ptr) {
    return rb_enc_str_new(ptr, strlen(ptr), rb_usascii_encoding());
  }

  VALUE rb_external_str_new_with_enc(const char* string, long size, rb_encoding* encoding) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String* str = String::create(env->state(), string, size);
    str->taint(env->state());

    Encoding* enc = Encoding::find(env->state(), encoding->name);
    if(enc == Encoding::usascii_encoding(env->state())
       && !CBOOL(str->ascii_only_p(env->state()))) {
      str->encoding(env->state(), Encoding::ascii8bit_encoding(env->state()));
    } else {
      str->encoding(env->state(), enc);
    }

    // TODO: handle transcoding if necessary
    return env->get_handle(str);
  }

  VALUE rb_external_str_new(const char* string, long size) {
    return rb_external_str_new_with_enc(string, size, rb_default_external_encoding());
  }

  VALUE rb_external_str_new_cstr(const char* string) {
    return rb_external_str_new_with_enc(string, strlen(string), rb_default_external_encoding());
  }

  VALUE rb_str_encode(VALUE str, VALUE to, int ecflags, VALUE ecopts) {
    return rb_funcall(rb_mCAPI, rb_intern("rb_str_encode"), 4,
                      str, to, INT2FIX(ecflags), ecopts);
  }

  int rb_enc_str_coderange(VALUE string) {
    // TODO
    return ENC_CODERANGE_7BIT;
  }

  VALUE rb_locale_str_new_cstr(const char *ptr) {
    // TODO
    return rb_str_new2(ptr);
  }

  VALUE rb_locale_str_new(const char* ptr, long len) {
    // TODO
    return rb_str_new(ptr, len);
  }

  VALUE rb_str_conv_enc(VALUE str, rb_encoding *from, rb_encoding *to) {
    // TODO
    return str;
  }

  VALUE rb_str_export_to_enc(VALUE str, rb_encoding *enc) {
    // TODO
    return rb_str_conv_enc(str, rb_enc_get(str), enc);
  }
}
