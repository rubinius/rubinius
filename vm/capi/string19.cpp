#include "vm.hpp"
#include "objectmemory.hpp"

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
#include "util/vsnprintf.h"

#include <string.h>

using namespace rubinius;
using namespace rubinius::capi;

namespace rubinius {
  namespace capi {
  }
}

extern "C" {

  void rb_str_free(VALUE str) {}

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

    return rb_str_conv_enc(env->get_handle(str), enc->get_encoding(),
                           rb_default_internal_encoding());
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
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    String *str = c_as<String>(env->get_object(string));

    bool valid = CBOOL(str->valid_encoding_p(env->state()));
    bool ascii = CBOOL(str->ascii_only_p(env->state()));

    if(valid && ascii) {
      return ENC_CODERANGE_7BIT;
    } else if(valid) {
      return ENC_CODERANGE_VALID;
    } else {
      return ENC_CODERANGE_BROKEN;
    }
  }

  VALUE rb_locale_str_new_cstr(const char *ptr) {
    return rb_external_str_new_with_enc(ptr, strlen(ptr), rb_locale_encoding());
  }

  VALUE rb_locale_str_new(const char* ptr, long len) {
    return rb_external_str_new_with_enc(ptr, len, rb_locale_encoding());
  }

  VALUE rb_str_conv_enc_opts(VALUE str, rb_encoding* from, rb_encoding* to,
                             int ecflags, VALUE ecopts)
  {
    VALUE f = rb_enc_from_encoding(from);
    VALUE t = to ? rb_enc_from_encoding(to) : Qnil;

    return rb_funcall(rb_mCAPI, rb_intern("rb_str_conv_enc_opts"), 5,
                      str, f, t, INT2FIX(ecflags), ecopts);
  }

  VALUE rb_str_conv_enc(VALUE str, rb_encoding *from, rb_encoding *to) {
    return rb_str_conv_enc_opts(str, from, to, 0, Qnil);
  }

  VALUE rb_str_export_to_enc(VALUE str, rb_encoding *enc) {
    return rb_str_conv_enc(str, rb_enc_get(str), enc);
  }

  VALUE rb_str_export(VALUE str) {
    return rb_str_conv_enc(str, rb_enc_get(str), rb_default_external_encoding());
  }

  VALUE rb_str_export_locale(VALUE str) {
    return rb_str_conv_enc(str, rb_enc_get(str), rb_locale_encoding());
  }

  VALUE rb_sprintf(const char* format, ...) {
    va_list varargs;

    ssize_t length = strlen(format) << 1;
    ssize_t err = 0;
    VALUE result = Qnil;

    do {
      result = rb_str_buf_new(length);
      char* buf = RSTRING_PTR(result);
      va_start(varargs, format);
      err = ruby_vsnprintf(buf, length, format, varargs);
      va_end(varargs);
      if(err >= length) {
        length = err + 1;
        err = -1;
      } else if(err < 0) {
        length <<= 1;
      }
    } while(err < 0);

    rb_str_set_len(result, err);
    return result;
  }

}
