#include "oniguruma.h" // Must be first.

#include "builtin/array.hpp"
#include "builtin/class.hpp"
#include "builtin/encoding.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"

#include "object_utils.hpp"

#include "ontology.hpp"

#include <ctype.h>

#define ENC_DEFINE(name, data)      define(state, name, ONIG_ENCODING_##data);
#define ENC_REPLICATE(name, orig)   replicate(state, name, orig);
#define ENC_ALIAS(name, orig)       alias(state, name, orig);
#define ENC_DUMMY(name)             set_dummy(state, name);

namespace rubinius {
  void Encoding::init(STATE) {
    onig_init();  // in regexp.cpp too, but idempotent.

    Class* enc = ontology::new_class_under(state, "Encoding", G(rubinius));
    enc->name(state, state->symbol("Rubinius::Encoding"));

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
    encoding_map(state)->store(state, state->symbol(name), Fixnum::from(index));
    encoding_list(state)->set(state, index, e);
    add_constant(state, name, e);

    return e;
  }

  Encoding* Encoding::define(STATE, const char* name,
                             OnigEncodingType* enc, Object* dummy)
  {
    Encoding* e = create(state, enc, dummy);

    e->name(state, String::create(state, name));

    Array* list = encoding_list(state);
    size_t index = list->size();

    encoding_map(state)->store(state, state->symbol(name), Fixnum::from(index));
    list->set(state, index, e);
    add_constant(state, name, e);

    return e;
  }

  Encoding* Encoding::alias(STATE, const char* name, const char* original) {
    int index = find_index(state, original);
    if(index < 0) return nil<Encoding>();

    encoding_map(state)->store(state, state->symbol(name), Fixnum::from(index));

    return as<Encoding>(encoding_list(state)->get(state, index));
  }

  Encoding* Encoding::set_dummy(STATE, const char* name) {
    Encoding* enc = find(state, name);
    if(enc->nil_p()) return nil<Encoding>();

    enc->dummy(state, Qtrue);
    return enc;
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
    if(ISDIGIT(*name) || !ISALNUM(*name)) return;

    char* s = const_cast<char*>(name);
    bool has_lower = false;

    if(ISUPPER(*s)) {
      while(*++s && (ISALNUM(*s) || *s == '_')) {
        if(ISLOWER(*s)) has_lower = true;
      }
    }

    if(!*s && !has_lower) {
      if(s - name > ENCODING_NAMELEN_MAX) return;

      G(encoding)->set_const(state, state->symbol(name), enc);
      return;
    }

    char* p = s = strdup(name);
    if(!s) return;

    if(ISUPPER(*s)) {
      while(*++s) {
        if(!ISALNUM(*s)) *s = '_';
        if(ISLOWER(*s)) has_lower = true;
      }

      if(s - p > ENCODING_NAMELEN_MAX) {
        free(s);
        return;
      }
      G(encoding)->set_const(state, state->symbol(p), enc);
    } else {
      has_lower = true;
    }

    if(has_lower) {
      s = p;
      while(*s) {
        if(!ISALNUM(*s)) *s = '_';
        if(ISLOWER(*s)) *s = toupper((int)*s);
        ++s;
      }
      G(encoding)->set_const(state, state->symbol(p), enc);
    }

    free(p);
  }

  Class* Encoding::internal_class(STATE) {
    return as<Class>(G(rubinius)->get_const(state, state->symbol("Encoding")));
  }

  LookupTable* Encoding::encoding_map(STATE) {
    return as<LookupTable>(internal_class(state)->get_const(
              state, state->symbol("EncodingMap")));
  }

  Array* Encoding::encoding_list(STATE) {
    return as<Array>(internal_class(state)->get_const(
              state, state->symbol("EncodingList")));
  }

  int Encoding::find_index(STATE, const char* name) {
    Object* obj = encoding_map(state)->fetch(state, state->symbol(name));
    if(Fixnum* index = try_as<Fixnum>(obj)) {
      return index->to_native();
    } else {
      return -1;
    }
  }

  Encoding* Encoding::find(STATE, const char* name) {
    int index = find_index(state, name);
    if(index < 0) return nil<Encoding>();
    return as<Encoding>(encoding_list(state)->get(state, index));
  }

  Encoding* Encoding::replicate(STATE, const char* name, const char* original) {
    Encoding* enc = find(state, original);
    if(enc->nil_p()) return nil<Encoding>();

    return define(state, name, enc->get_encoding(), enc->dummy());
  }

  Encoding* Encoding::replicate(STATE, String* name) {
    return Encoding::define(state, name->c_str(state), encoding_);
  }

}
