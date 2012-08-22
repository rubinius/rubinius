// TODO: resolve after importing Oniguruma from MRI
#include <stddef.h>
#include "capi/19/include/ruby/oniguruma.h"
#include "capi/19/include/ruby/transcoder.h"
#include "capi/19/include/ruby/regenc.h"

#include "builtin/array.hpp"
#include "builtin/encoding.hpp"
#include "builtin/nativemethod.hpp"
#include "builtin/regexp.hpp"

#include "capi/capi.hpp"

#include "capi/19/include/ruby/ruby.h"
#include "capi/19/include/ruby/encoding.h"

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {
  int rb_enc_coderange_asciionly_p(VALUE obj) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* val = env->get_object(obj);

    if(String* str = try_as<String>(val)) {
      if(CBOOL(str->ascii_only_p(env->state()))) return Qtrue;
    } else {
      rb_raise(rb_eArgError, "ENC_CODERANGE_ASCIIONLY is only defined for String");
    }

    return Qfalse;
  }

  int rb_encdb_alias(const char *alias, const char *orig) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Encoding::alias(env->state(), alias, orig);

    return Encoding::find_index(env->state(), alias);
  }

  rb_encoding* rb_utf8_encoding() {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    return Encoding::utf8_encoding(env->state())->get_encoding();
  }

  int rb_utf8_encindex(void) {
    return Encoding::eUtf8;
  }

  rb_encoding* rb_usascii_encoding() {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    return Encoding::usascii_encoding(env->state())->get_encoding();
  }

  int rb_usascii_encindex(void) {
    return Encoding::eAscii;
  }

  rb_encoding* rb_ascii8bit_encoding() {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    return Encoding::ascii8bit_encoding(env->state())->get_encoding();
  }

  int rb_ascii8bit_encindex(void) {
    return Encoding::eBinary;
  }

  rb_encoding* rb_locale_encoding(void) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    Encoding* enc = Encoding::find(env->state(), "locale");
    if(enc->nil_p()) {
      return rb_usascii_encoding();
    } else {
      return enc->get_encoding();
    }
  }

  int rb_locale_encindex(void) {
    return rb_enc_find_index(rb_locale_encoding()->name);
  }

  rb_encoding* rb_filesystem_encoding(void) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    Encoding* enc = Encoding::find(env->state(), "filesystem");
    if(enc->nil_p()) {
      return rb_ascii8bit_encoding();
    } else {
      return enc->get_encoding();
    }
  }

  int rb_filesystem_encindex(void) {
    return rb_enc_find_index(rb_filesystem_encoding()->name);
  }

  rb_encoding *rb_default_internal_encoding(void) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    Encoding* enc = Encoding::find(env->state(), "internal");
    if(enc->nil_p()) {
      return 0;
    } else {
      return enc->get_encoding();
    }
  }

  rb_encoding *rb_default_external_encoding(void) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    Encoding* enc = Encoding::find(env->state(), "external");
    if(enc->nil_p()) {
      return 0;
    } else {
      return enc->get_encoding();
    }
  }

  rb_encoding* rb_enc_get(VALUE obj) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    int index = rb_enc_get_index(obj);
    if(index < 0) return 0;

    Encoding* enc = try_as<Encoding>(
        Encoding::encoding_list(env->state())->get(env->state(), index));

    if(!enc) return 0;

    return enc->get_encoding();
  }

  VALUE rb_obj_encoding(VALUE obj) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    Object* val = env->get_object(obj);
    Encoding* enc = Encoding::get_object_encoding(env->state(), val);
    return env->get_handle(enc);
  }

  int rb_enc_get_index(VALUE obj) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* val = env->get_object(obj);
    Encoding* enc = Encoding::get_object_encoding(env->state(), val);

    if(enc->nil_p()) return 0;

    return Encoding::find_index(env->state(), enc->get_encoding()->name);
  }

  void rb_enc_set_index(VALUE obj, int index) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Encoding* enc = Encoding::from_index(env->state(), index);
    Object* val = env->get_object(obj);

    Encoding::set_object_encoding(env->state(), val, enc);
  }

  rb_encoding* rb_enc_compatible(VALUE str1, VALUE str2) {
    // TODO
    return rb_enc_get(str1);
  }

  rb_encoding* rb_to_encoding(VALUE obj) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    int index = rb_to_encoding_index(obj);
    if(index < 0) return 0;

    Encoding* enc = try_as<Encoding>(
        Encoding::encoding_list(env->state())->get(env->state(), index));

    if(!enc) return 0;
    return enc->get_encoding();
  }

  int rb_to_encoding_index(VALUE obj) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    Encoding* enc = nil<Encoding>();

    switch(TYPE(obj)) {
    case T_ENCODING:
      enc = c_as<Encoding>(env->get_object(obj));
      break;
    case T_STRING:
      enc = Encoding::find(env->state(), RSTRING_PTR(obj));
      break;
    default:
      obj = rb_funcall(obj, rb_intern("to_str"), 0);
      enc = Encoding::find(env->state(), RSTRING_PTR(obj));
    }

    if(enc->nil_p()) return -1;

    return Encoding::find_index(env->state(), enc->get_encoding()->name);
  }

  int rb_enc_dummy_p(rb_encoding *enc) {
    // TODO
    return 0;
  }

  VALUE rb_enc_associate(VALUE obj, rb_encoding *enc) {
    return rb_enc_associate_index(obj, rb_enc_to_index(enc));
  }

  VALUE rb_enc_associate_index(VALUE obj, int index) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Encoding* enc = try_as<Encoding>(
        Encoding::encoding_list(env->state())->get(env->state(), index));

    if(!enc) return obj;

    Object* val = env->get_object(obj);

    if(String* str = try_as<String>(val)) {
      str->encoding(env->state(), enc);
    } else if(Regexp* reg = try_as<Regexp>(val)) {
      reg->encoding(env->state(), enc);
    } else if(Symbol* sym = try_as<Symbol>(val)) {
      sym->encoding(env->state(), enc);
    } else {
      rb_raise(rb_eArgError, "object does not have an associated Encoding");
    }

    return obj;
  }

  void rb_enc_copy(VALUE dest, VALUE src) {
    rb_enc_associate(dest, rb_enc_get(src));
  }

  int rb_define_dummy_encoding(const char *) {
    // TODO
    return 1;
  }

  rb_encoding* rb_enc_find(const char* name) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Encoding* enc = Encoding::find(env->state(), name);
    if(enc->nil_p()) return 0;
    return enc->get_encoding();
  }

  int rb_enc_find_index(const char *name) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    return Encoding::find_index(env->state(), name);
  }

  rb_encoding* rb_enc_from_index(int index) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Encoding* enc = Encoding::from_index(env->state(), index);
    if(enc->nil_p()) return 0;
    return enc->get_encoding();
  }

  int rb_enc_to_index(rb_encoding* enc) {
    if(enc) {
      return rb_enc_find_index(rb_enc_name(enc));
    } else {
      return 0;
    }
  }

  VALUE rb_enc_from_encoding(rb_encoding *enc) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    return env->get_handle(Encoding::find(env->state(), enc->name));
  }

  int rb_enc_mbclen(const char *p, const char *e, rb_encoding *enc) {
    int n = ONIGENC_PRECISE_MBC_ENC_LEN(enc, (UChar*)p, (UChar*)e);
    if (ONIGENC_MBCLEN_CHARFOUND_P(n) && ONIGENC_MBCLEN_CHARFOUND_LEN(n) <= e-p)
      return ONIGENC_MBCLEN_CHARFOUND_LEN(n);
    else {
      int min = rb_enc_mbminlen(enc);
      return min <= e-p ? min : (int)(e-p);
    }
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

  void rb_declare_transcoder(const char* from, const char* to, const char* lib) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Transcoding::declare(env->state(), from, to, lib);
  }

  void rb_register_transcoder(const rb_transcoder* trans) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Transcoding::define(env->state(), (OnigTranscodingType*)trans);
  }
}
