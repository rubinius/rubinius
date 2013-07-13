#include "symboltable.hpp"
#include "exception.hpp"
#include "configuration.hpp"
#include "version.h"

#include "builtin/array.hpp"
#include "builtin/encoding.hpp"
#include "builtin/exception.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"

#include <iostream>
#include <iomanip>

namespace rubinius {

  SymbolTable::Kind SymbolTable::detect_kind(const char* str, size_t size) const {
    const char one = str[0];

    // A constant begins with an uppercase letter.
    if(one >= 'A' && one <= 'Z') {
      // Make sure that the rest of it is only alphanumerics
      for(size_t i = 1; i < size; i++) {
        if((isalnum(str[i]) || str[i] == '_') == false)
          return SymbolTable::Normal;
      }
      return SymbolTable::Constant;
    }

    if(one == '@') {
      // A class variable begins with @@
      if(size > 1 && str[1] == '@') {
        return SymbolTable::CVar;
      }

      // An instance variable begins with @
      return SymbolTable::IVar;
    }

    // A system variable begins with __
    if(size > 2 && one == '_' && str[1] == '_') {
      return SymbolTable::System;
    }

    // Everything else is normal
    return SymbolTable::Normal;
  }

  SymbolTable::Kind SymbolTable::kind(STATE, const Symbol* sym) const {
    return kinds[sym->index()];
  }

  size_t SymbolTable::add(std::string str) {
    bytes_used_ += (str.size() + sizeof(str));

    strings.push_back(str);
    kinds.push_back(detect_kind(str.data(), str.size()));
    return strings.size() - 1;
  }

  Symbol* SymbolTable::lookup(STATE, const char* str, size_t length) {
    if(length == 0 && LANGUAGE_18_ENABLED) {
      Exception::argument_error(state, "Cannot create a symbol from an empty string");
      return NULL;
    }

    return lookup(str, length, state->hash_seed());
  }

  struct SpecialOperator {
    const char* name;
    const char* symbol;
  };

  // These are a set of special operators that MRI
  // changes the symbol value of.
  static SpecialOperator SpecialOperators[] = {
    {"+(binary)", "+"},
    {"-(binary)", "-"},
    {"+(unary)", "+@"},
    {"-(unary)", "-@"},
    {"!(unary)", "!"},
    {"~(unary)", "~"},
    {"!@", "!"},
    {"~@", "~"}
  };

  const static int cNumSpecialOperators = 8;

  static const char* find_special(const char* check, size_t length) {
    for(int i = 0; i < cNumSpecialOperators; i++) {
      SpecialOperator* op = &SpecialOperators[i];
      if(*op->name == *check && strncmp(op->name, check, length) == 0) {
        return op->symbol;
      }
    }

    return 0;
  }

  Symbol* SymbolTable::lookup(SharedState* shared, const std::string& str) {
    return lookup(str.data(), str.size(), shared->hash_seed);
  }

  Symbol* SymbolTable::lookup(STATE, const std::string& str) {
    return lookup(str.data(), str.size(), state->hash_seed());
  }

  Symbol* SymbolTable::lookup(const char* str, size_t length, uint32_t seed) {
    size_t sym;

    if(const char* op = find_special(str, length)) {
      str = op;
      length = strlen(str);
    }

    hashval hash = String::hash_str((unsigned char*)str, length, seed);

    // Symbols can be looked up by multiple threads at the same time.
    // This is fast operation, so we protect this with a spinlock.
    {
      utilities::thread::SpinLock::LockGuard guard(lock_);
      SymbolMap::iterator entry = symbols.find(hash);
      if(entry == symbols.end()) {
        sym = add(std::string(str, length));
        SymbolIds v(1, sym);
        symbols[hash] = v;
      } else {
        SymbolIds& v = entry->second;
        for(SymbolIds::const_iterator i = v.begin(); i != v.end(); ++i) {
          std::string& s = strings[*i];

          if(!strncmp(s.data(), str, length)) return Symbol::from_index(*i);
        }
        sym = add(std::string(str, length));
        v.push_back(sym);
      }
    }

    return Symbol::from_index(sym);
  }

  Symbol* SymbolTable::lookup(STATE, String* str) {
    if(str->nil_p()) {
      Exception::argument_error(state, "Cannot look up Symbol from nil");
      return NULL;
    }

    // Since we also explicitly use the size, we can safely
    // use byte_address() here.
    const char* bytes = (const char*) str->byte_address();
    size_t size = str->byte_size();

    if(LANGUAGE_18_ENABLED) {
      for(size_t i = 0; i < size; i++) {
        if(bytes[i] == 0) {
          Exception::argument_error(state,
              "cannot create a symbol from a string containing `\\0'");
          return NULL;
        }
      }
    }

    return lookup(state, bytes, size);
  }

  String* SymbolTable::lookup_string(STATE, const Symbol* sym) {
    if(sym->nil_p()) {
      Exception::argument_error(state, "Cannot look up Symbol from nil");
      return NULL;
    }

    size_t sym_index = sym->index();
    if(sym_index >= strings.size()) {
      return NULL;
    }
    std::string& str = strings[sym_index];
    String* s = String::create(state, str.data(), str.size());
    s->encoding(state, Encoding::usascii_encoding(state));
    return s;
  }

  std::string& SymbolTable::lookup_cppstring(const Symbol* sym) {
    return strings[sym->index()];
  }

  std::string SymbolTable::lookup_debug_string(const Symbol* sym) {
    std::string str = lookup_cppstring(sym);
    std::ostringstream os;
    unsigned char* cstr = (unsigned char*) str.data();
    size_t size = str.size();
    for(size_t i = 0; i < size; ++i) {
      if(isprint(cstr[i]) && isascii(cstr[i])) {
        os << cstr[i];
      } else {
        os << "\\x" << std::setw(2) << std::setfill('0') << std::hex << (unsigned int)cstr[i];
      }
    }
    return os.str();
  }

  size_t SymbolTable::size() const {
    return strings.size();
  }

  size_t SymbolTable::byte_size() const {
    size_t total = 0;

    for(SymbolStrings::const_iterator i = strings.begin();
        i != strings.end();
        ++i) {
      total += i->size();
      total += sizeof(std::string);
    }

    return total;
  }

  Array* SymbolTable::all_as_array(STATE) {
    size_t idx = 0;
    Array* ary = Array::create(state, this->size());

    for(SymbolMap::iterator s = symbols.begin(); s != symbols.end(); ++s) {
      for(SymbolIds::iterator i = s->second.begin(); i != s->second.end(); ++i) {
        ary->set(state, idx++, Symbol::from_index(state, *i));
      }
    }

    return ary;
  }
}
