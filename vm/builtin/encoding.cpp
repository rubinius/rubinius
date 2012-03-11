#include "oniguruma.h" // Must be first.
#include "transcoder.h"
#include "regenc.h"

#include "builtin/array.hpp"
#include "builtin/bytearray.hpp"
#include "builtin/class.hpp"
#include "builtin/data.hpp"
#include "builtin/encoding.hpp"
#include "builtin/io.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/regexp.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "builtin/tuple.hpp"

#include "object_utils.hpp"

#include "ontology.hpp"

#include "vm/config.h"

#include "gc/gc.hpp"

#include <ctype.h>

#ifdef HAVE_NL_LANGINFO
#include <locale.h>
#include <langinfo.h>
#endif

#undef  ENC_DEFINE
#undef  ENC_REPLICATE
#undef  ENC_ALIAS
#undef  ENC_DUMMY

#define ENC_DEFINE(name, data)      define(state, name, ONIG_ENCODING_##data);
#define ENC_REPLICATE(name, orig)   replicate(state, name, orig);
#define ENC_ALIAS(name, orig)       alias(state, name, orig);
#define ENC_DUMMY(name)             define_dummy(state, name);

#define TRANS_DECLARE(from, to, lib)  declare(state, from, to, lib);

namespace rubinius {
  void Encoding::init(STATE) {
    onig_init();  // in regexp.cpp too, but idempotent.

    Class* enc = ontology::new_class_under(state, "EncodingClass", G(rubinius));

    GO(encoding).set(ontology::new_class_under(state, "Encoding", enc));
    G(encoding)->set_object_type(state, EncodingType);

    enc->set_const(state, "EncodingMap", LookupTable::create(state));
    enc->set_const(state, "EncodingList", Array::create(state, 3));

    Encoding* ascii = create_bootstrap(state, "US-ASCII", eAscii, ONIG_ENCODING_US_ASCII);
    Encoding* binary = create_bootstrap(state, "ASCII-8BIT", eBinary, ONIG_ENCODING_ASCII);
    Encoding* utf8 = create_bootstrap(state, "UTF-8", eUtf8, ONIG_ENCODING_UTF_8);

    ascii->name(state, String::create(state, "US-ASCII"));
    binary->name(state, String::create(state, "ASCII-8BIT"));
    utf8->name(state, String::create(state, "UTF-8"));

#include "vm/gen/encoding_database.cpp"

    int index = -1;
    char* locale_charmap;

#ifdef HAVE_NL_LANGINFO
    setlocale(LC_CTYPE, "");
    locale_charmap = nl_langinfo(CODESET);
#else
    locale_charmap = "US-ASCII";
#endif

    enc->set_const(state, "LocaleCharmap", String::create(state, locale_charmap));

    index = find_index(state, locale_charmap);
    if(index < 0) index = find_index(state, "US-ASCII");

    create_internal(state, "locale", index);
    create_internal(state, "external", index);
    create_internal(state, "filesystem", index);
    create_internal(state, "internal", -1);

    Transcoding::init(state);
    Converter::init(state);
  }

  static Symbol* encoding_symbol(STATE, const char* name) {
    char* upper = strdup(name);
    if(!upper) return nil<Symbol>();

    for(char *p = upper; *p; p++) {
      *p = toupper((int)*p);
    }

    Symbol* sym = state->symbol(upper);
    free(upper);

    return sym;
  }

  static Tuple* encoding_reference(STATE, int index, const char* alias_name = 0) {
    Tuple* pair = Tuple::create(state, 2);
    if(!alias_name) {
      pair->put(state, 0, cNil);
    } else {
      pair->put(state, 0, String::create(state, alias_name));
    }

    if(index < 0) {
      pair->put(state, 1, cNil);
    } else {
      pair->put(state, 1, Fixnum::from(index));
    }

    return pair;
  }

  Encoding* Encoding::create(STATE, OnigEncodingType* enc, Object* dummy) {
    Encoding* e = state->new_object<Encoding>(G(encoding));

    e->dummy(state, dummy);
    e->encoding_ = enc;
    e->managed_ = false;

    return e;
  }

  Encoding* Encoding::create_bootstrap(STATE, const char* name,
                                       Index index, OnigEncodingType* enc)
  {
    Encoding* e = create(state, enc);
    Tuple* ref = encoding_reference(state, index);
    encoding_map(state)->store(state, encoding_symbol(state, name), ref);
    encoding_list(state)->set(state, index, e);
    add_constant(state, name, e);

    return e;
  }

  void Encoding::create_internal(STATE, const char* name, int index) {
    Tuple* ref = encoding_reference(state, index, name);
    encoding_map(state)->store(state, encoding_symbol(state, name), ref);
  }

  Encoding* Encoding::define(STATE, const char* name,
                             OnigEncodingType* enc, Object* dummy)
  {
    Encoding* e = create(state, enc, dummy);

    e->name(state, String::create(state, name));

    Array* list = encoding_list(state);
    size_t index = list->size();

    Tuple* ref = encoding_reference(state, index);
    encoding_map(state)->store(state, encoding_symbol(state, name), ref);
    list->set(state, index, e);
    add_constant(state, name, e);

    return e;
  }

  Encoding* Encoding::define_dummy(STATE, const char* name) {
    return define(state, name, ONIG_ENCODING_ASCII, cTrue);
  }

  Encoding* Encoding::replicate(STATE, const char* name, Encoding* enc) {
    Encoding* repl = define(state, name, enc->get_encoding(), enc->dummy());
    repl->make_managed(state, name, enc->get_encoding());

    return repl;
  }

  Encoding* Encoding::replicate(STATE, const char* name, const char* original) {
    Encoding* enc = find(state, original);
    if(enc->nil_p()) return nil<Encoding>();

    return replicate(state, name, enc);
  }

  Encoding* Encoding::alias(STATE, const char* name, const char* original) {
    int index = find_index(state, original);
    if(index < 0) return nil<Encoding>();

    Tuple* ref = encoding_reference(state, index, name);
    encoding_map(state)->store(state, encoding_symbol(state, name), ref);
    add_constant(state, name, from_index(state, index));

    return as<Encoding>(encoding_list(state)->get(state, index));
  }

  Encoding* Encoding::usascii_encoding(STATE) {
    return as<Encoding>(encoding_list(state)->get(state, eAscii));
  }

  Encoding* Encoding::utf8_encoding(STATE) {
    return as<Encoding>(encoding_list(state)->get(state, eUtf8));
  }

  Encoding* Encoding::ascii8bit_encoding(STATE) {
    return as<Encoding>(encoding_list(state)->get(state, eBinary));
  }

  Encoding* Encoding::default_external(STATE) {
    return Encoding::find(state, "external");
  }

  Encoding* Encoding::default_internal(STATE) {
    return Encoding::find(state, "internal");
  }

  Encoding* Encoding::get_object_encoding(STATE, Object* obj) {
    if(String* str = try_as<String>(obj)) {
      return str->encoding(state);
    } else if(Symbol* sym = try_as<Symbol>(obj)) {
      return sym->encoding(state);
    } else if(Regexp* reg = try_as<Regexp>(obj)) {
      return reg->encoding(state);
    } else if(Encoding* enc = try_as<Encoding>(obj)) {
      return enc;
    } else if(try_as<IO>(obj)) {
      // TODO
      return nil<Encoding>();
    } else {
      // MRI permits associating an Encoding with anything.
      Object* e = obj->get_ivar(state, state->symbol("__encoding__"));
      if(Encoding* enc = try_as<Encoding>(e)) return enc;
    }

    return nil<Encoding>();
  }

  void Encoding::set_object_encoding(STATE, Object* obj, Encoding* enc) {
    if(String* str = try_as<String>(obj)) {
      str->encoding(state, enc);
    } else if(Regexp* reg = try_as<Regexp>(obj)) {
      reg->encoding(state, enc);
    } else if(Symbol* sym = try_as<Symbol>(obj)) {
      sym->encoding(state, enc);
    } else if(try_as<IO>(obj)) {
      // TODO
    } else {
      // MRI permits associating an Encoding with anything.
      obj->set_ivar(state, state->symbol("__encoding__"), enc);
    }
  }

  Encoding* Encoding::compatible_p(STATE, Object* a, Object* b) {
    Encoding* enc_a = get_object_encoding(state, a);
    if(enc_a->nil_p()) return nil<Encoding>();

    Encoding* enc_b = get_object_encoding(state, b);
    if(enc_b->nil_p()) return nil<Encoding>();

    if(enc_a == enc_b) return enc_a;

    String* str_a = try_as<String>(a);
    String* str_b = try_as<String>(b);

    if(str_b && str_b->byte_size() == 0) return enc_a;
    if(str_a && str_a->byte_size() == 0) {
      if(CBOOL(enc_a->ascii_compatible_p(state))
          && CBOOL(str_b->ascii_only_p(state))) {
        return enc_a;
      } else {
        return enc_b;
      }
    }

    if(!CBOOL(enc_a->ascii_compatible_p(state))
        || !CBOOL(enc_b->ascii_compatible_p(state))) {
      return nil<Encoding>();
    }

    Encoding* ascii = Encoding::usascii_encoding(state);

    if(!str_b && enc_b == ascii) return enc_a;
    if(!str_a && enc_a == ascii) return enc_b;

    if(!str_a && !str_b) return nil<Encoding>();

    if(str_a && str_b) {
      bool ascii_a = CBOOL(str_a->ascii_only_p(state));
      bool ascii_b = CBOOL(str_b->ascii_only_p(state));

      if(ascii_a != ascii_b) {
        if(ascii_a) return enc_b;
        if(ascii_b) return enc_a;
      }

      if(ascii_b) return enc_a;
      if(ascii_a) return enc_b;
    } else if(!str_b) {
      if(CBOOL(str_a->ascii_only_p(state))) return enc_b;
    }

    return nil<Encoding>();
  }

#define ENCODING_NAMELEN_MAX 63

  void Encoding::add_constant(STATE, const char* name, Encoding* enc) {
    if(ISDIGIT(*name)) return;

    char* s = const_cast<char*>(name);
    bool has_lower = false;
    bool has_upper = false;
    bool valid = false;

    if(ISUPPER(*s)) {
      has_upper = true;
      while(*++s && (ISALNUM(*s) || *s == '_')) {
        if(ISLOWER(*s)) has_lower = true;
      }
    }

    if(!*s) {
      if(s - name > ENCODING_NAMELEN_MAX) return;

      valid = true;
      G(encoding)->set_const(state, state->symbol(name), enc);
    }

    if(!valid || has_lower) {
      size_t len = s - name;
      if(len > ENCODING_NAMELEN_MAX) return;

      if(!has_lower || !has_upper) {
        do {
          if(ISLOWER(*s)) has_lower = true;
          if(ISUPPER(*s)) has_upper = true;
        } while(*++s && (!has_lower || !has_upper));
        len = s - name;
      }

      len += strlen(s);
      if(len++ > ENCODING_NAMELEN_MAX) return;

      char* p = s = strdup(name);
      if(!s) return;
      name = s;

      if(!valid) {
        if(ISLOWER(*s)) *s = ONIGENC_ASCII_CODE_TO_UPPER_CASE((int)*s);
        for(; *s; ++s) {
          if(!ISALNUM(*s)) *s = '_';
        }
        if(has_upper) {
          G(encoding)->set_const(state, state->symbol(name), enc);
        }
      }

      if(has_lower) {
        for(s = (char *)name; *s; ++s) {
          if(ISLOWER(*s)) *s = ONIGENC_ASCII_CODE_TO_UPPER_CASE((int)*s);
        }
        G(encoding)->set_const(state, state->symbol(name), enc);
      }

      free(p);
    }

  }

  Class* Encoding::internal_class(STATE) {
    return as<Class>(G(rubinius)->get_const(state, state->symbol("EncodingClass")));
  }

  Class* Encoding::transcoding_class(STATE) {
    return as<Class>(G(encoding)->get_const(state, state->symbol("Transcoding")));
  }

  LookupTable* Encoding::encoding_map(STATE) {
    return as<LookupTable>(internal_class(state)->get_const(
              state, state->symbol("EncodingMap")));
  }

  Array* Encoding::encoding_list(STATE) {
    return as<Array>(internal_class(state)->get_const(
              state, state->symbol("EncodingList")));
  }

  LookupTable* Encoding::transcoding_map(STATE) {
    return as<LookupTable>(internal_class(state)->get_const(
              state, state->symbol("TranscodingMap")));
  }

  Encoding* Encoding::from_index(STATE, int index) {
    if(Encoding* enc = try_as<Encoding>(encoding_list(state)->get(state, index))) {
      return enc;
    } else {
      return nil<Encoding>();
    }
  }

  int Encoding::find_index(STATE, const char* name) {
    Object* obj = encoding_map(state)->fetch(state, encoding_symbol(state, name));

    if(Tuple* ref = try_as<Tuple>(obj)) {
      Fixnum* index = try_as<Fixnum>(ref->at(1));
      if(index) {
        return index->to_native();
      } else {
        return -1;
      }
    } else {
      return -1;
    }
  }

  Encoding* Encoding::find(STATE, const char* name) {
    int index = find_index(state, name);
    if(index < 0) return nil<Encoding>();
    return as<Encoding>(encoding_list(state)->get(state, index));
  }

  // Encoding#replicate primitive
  Encoding* Encoding::replicate(STATE, String* name) {
    return Encoding::replicate(state, name->c_str(state), this);
  }

  Object* Encoding::ascii_compatible_p(STATE) {
    bool v = encoding_->min_enc_len == 1 && dummy_ == cFalse;
    return v ? cTrue : cFalse;
  }

  void Encoding::make_managed(STATE, const char* name, OnigEncodingType* enc) {
    ByteArray* enc_ba = ByteArray::create(state, sizeof(OnigEncodingType));
    memcpy(enc_ba->raw_bytes(), enc, sizeof(OnigEncodingType));

    encoding_ = reinterpret_cast<OnigEncodingType*>(enc_ba->raw_bytes());
    write_barrier(state, enc_ba);

    int size = strlen(name);
    if(size >= ENCODING_NAMELEN_MAX) size = ENCODING_NAMELEN_MAX-1;

    ByteArray* name_ba = ByteArray::create(state, size);
    memcpy(name_ba->raw_bytes(), name, size);
    name_ba->raw_bytes()[size] = 0;
    encoding_->name = reinterpret_cast<const char*>(name_ba->raw_bytes());
    write_barrier(state, name_ba);

    managed_ = true;
  }

  void Encoding::Info::mark(Object* obj, ObjectMark& mark) {
    auto_mark(obj, mark);

    Encoding* enc_o = force_as<Encoding>(obj);
    if(!enc_o->get_managed()) return;

    OnigEncodingType* enc = enc_o->get_encoding();
    if(!enc) return;

    ByteArray* enc_ba = ByteArray::from_body(enc);
    if(ByteArray* tmp = force_as<ByteArray>(mark.call(enc_ba))) {
      enc_o->set_encoding(reinterpret_cast<OnigEncodingType*>(tmp->raw_bytes()));
      mark.just_set(obj, tmp);

      enc = enc_o->get_encoding();
    }

    if(enc->name) {
      ByteArray* ba = ByteArray::from_body(const_cast<char*>(enc->name));
      if(ByteArray* tmp = force_as<ByteArray>(mark.call(ba))) {
        enc->name = reinterpret_cast<const char*>(tmp->raw_bytes());
        mark.just_set(obj, tmp);
      }
    }
  }

  void Encoding::Info::show(STATE, Object* self, int level) {
    Encoding* enc = as<Encoding>(self);

    class_header(state, self);
    indent_attribute(++level, "name"); enc->name()->show_simple(state, level);
    indent_attribute(level, "dummy?"); enc->dummy()->show_simple(state, level);
    close_body(level);
  }

  void Transcoding::init(STATE) {
    ontology::new_class_under(state, "Transcoding", G(encoding));

    Class* cls = Encoding::internal_class(state);
    cls->set_const(state, "TranscodingMap", LookupTable::create(state));

#include "vm/gen/transcoder_database.cpp"
  }

  Transcoding* Transcoding::create(STATE, OnigTranscodingType* tr) {
    Class* cls = Encoding::transcoding_class(state);
    Transcoding* t = state->new_object<Transcoding>(cls);

    t->source(state, String::create(state, tr->src_encoding));
    t->target(state, String::create(state, tr->dst_encoding));
    t->transcoder_ = tr;

    return t;
  }

  void Transcoding::declare(STATE, const char* from, const char* to, const char* lib) {
    LookupTable* map = Encoding::transcoding_map(state);
    Object* obj = map->fetch(state, encoding_symbol(state, from));
    LookupTable* table;

    if(obj->nil_p()) {
      table = LookupTable::create(state);
      map->store(state, encoding_symbol(state, from), table);
    } else {
      table = as<LookupTable>(obj);
    }

    table->store(state, encoding_symbol(state, to), String::create(state, lib));
  }

  void Transcoding::define(STATE, OnigTranscodingType* tr) {
    LookupTable* map = Encoding::transcoding_map(state);
    Object* obj = map->fetch(state, encoding_symbol(state, tr->src_encoding));
    LookupTable* table;

    if(obj->nil_p()) {
      table = LookupTable::create(state);
      map->store(state, encoding_symbol(state, tr->src_encoding), table);
    } else {
      table = as<LookupTable>(obj);
    }

    Transcoding* t = Transcoding::create(state, tr);
    table->store(state, encoding_symbol(state, tr->dst_encoding), t);
  }

  void Converter::init(STATE) {
    ontology::new_class_under(state, "Converter", G(encoding));
  }
}
