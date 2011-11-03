#include "oniguruma.h" // Must be first.

#include "builtin/class.hpp"
#include "builtin/encoding.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"

#include "object_utils.hpp"

#include "ontology.hpp"

#include <ctype.h>

extern OnigEncodingType OnigEncodingASCII;
extern OnigEncodingType OnigEncodingUTF8;

namespace rubinius {
  void Encoding::init(STATE) {
    onig_init();  // in regexp.cpp too, but idempotent.

    Class* enc = ontology::new_class_under(state, "Encoding", G(rubinius));
    enc->name(state, state->symbol("Rubinius::Encoding"));

    GO(encoding).set(ontology::new_class_under(state, "Encoding", enc));
    G(encoding)->set_object_type(state, EncodingType);
    G(encoding)->name(state, state->symbol("Encoding"));

    enc->set_const(state, "SymbolMap", LookupTable::create(state));
    enc->set_const(state, "IndexMap", LookupTable::create(state));

    Encoding* ascii = create_bootstrap(state, "US-ASCII", eAscii, &OnigEncodingASCII);
    Encoding* utf8 = create_bootstrap(state, "UTF-8", eUtf8, &OnigEncodingUTF8);

    ascii->name(state, String::create(state, "US-ASCII"));
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
    symbol_map(state)->store(state, state->symbol(name), e);
    index_map(state)->store(state, Fixnum::from(index), e);
    add_constant(state, name, e);

    return e;
  }

  Encoding* Encoding::define(STATE, const char* name,
                             OnigEncodingType* enc, Object* dummy)
  {
    Encoding* e = create(state, enc, dummy);

    e->name(state, String::create(state, name));

    symbol_map(state)->store(state, state->symbol(name), e);
    add_constant(state, name, e);

    return e;
  }

  Encoding* Encoding::alias(STATE, const char* name, Encoding* enc) {
    symbol_map(state)->store(state, state->symbol(name), enc);
    add_constant(state, name, enc);

    return enc;
  }

  Encoding* Encoding::ascii_encoding(STATE) {
    return as<Encoding>(index_map(state)->fetch(state, Fixnum::from(eAscii)));
  }

  Encoding* Encoding::utf8_encoding(STATE) {
    return as<Encoding>(index_map(state)->fetch(state, Fixnum::from(eUtf8)));
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

  LookupTable* Encoding::symbol_map(STATE) {
    return as<LookupTable>(internal_class(state)->get_const(
              state, state->symbol("SymbolMap")));
  }

  LookupTable* Encoding::index_map(STATE) {
    return as<LookupTable>(internal_class(state)->get_const(
              state, state->symbol("IndexMap")));
  }

  Encoding* Encoding::find(STATE, Symbol* name) {
    return as<Encoding>(symbol_map(state)->fetch(state, name));
  }

  Encoding* Encoding::replicate(STATE, String* name) {
    return Encoding::define(state, name->c_str(state), encoding_);
  }

}
