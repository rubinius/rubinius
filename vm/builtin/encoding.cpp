#include "oniguruma.h" // Must be first.
#include "regenc.h"

#include "builtin/array.hpp"
#include "builtin/class.hpp"
#include "builtin/encoding.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "builtin/tuple.hpp"

#include "object_utils.hpp"

#include "ontology.hpp"

#include "vm/config.h"

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

namespace rubinius {
  void Encoding::init(STATE) {
    onig_init();  // in regexp.cpp too, but idempotent.

    Class* enc = ontology::new_class_under(state, "EncodingClass", G(rubinius));
    enc->name(state, state->symbol("Rubinius::EncodingClass"));

    GO(encoding).set(ontology::new_class_under(state, "Encoding", enc));
    G(encoding)->set_object_type(state, EncodingType);
    G(encoding)->name(state, state->symbol("Encoding"));

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
      pair->put(state, 0, Qnil);
    } else {
      pair->put(state, 0, String::create(state, alias_name));
    }

    if(index < 0) {
      pair->put(state, 1, Qnil);
    } else {
      pair->put(state, 1, Fixnum::from(index));
    }

    return pair;
  }

  Encoding* Encoding::create(STATE, OnigEncodingType* enc, Object* dummy) {
    Encoding* e = state->new_object<Encoding>(G(encoding));

    e->dummy(state, dummy);
    e->encoding_ = enc;

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
    return define(state, name, ONIG_ENCODING_ASCII, Qtrue);
  }

  Encoding* Encoding::replicate(STATE, const char* name, const char* original) {
    Encoding* enc = find(state, original);
    if(enc->nil_p()) return nil<Encoding>();

    return define(state, name, enc->get_encoding(), enc->dummy());
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

  LookupTable* Encoding::encoding_map(STATE) {
    return as<LookupTable>(internal_class(state)->get_const(
              state, state->symbol("EncodingMap")));
  }

  Array* Encoding::encoding_list(STATE) {
    return as<Array>(internal_class(state)->get_const(
              state, state->symbol("EncodingList")));
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
    return Encoding::define(state, name->c_str(state), encoding_);
  }

  Object* Encoding::ascii_compatible_p(STATE) {
    bool v = encoding_->min_enc_len == 1 && dummy_ == Qfalse;
    return v ? Qtrue : Qfalse;
  }
}
