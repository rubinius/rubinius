#include "symbol_table.hpp"
#include "exception.hpp"
#include "configuration.hpp"

#include "builtin/array.hpp"
#include "builtin/encoding.hpp"
#include "builtin/exception.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"

#include <iostream>
#include <iomanip>

namespace rubinius {

  SymbolTable::Kind SymbolTable::detect_kind(const char* str, size_t size) {
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

      // An instance variable can't start with a digit and can't be just @.
      if((size == 1) || (size > 1 && ISDIGIT(str[1]))) {
        return SymbolTable::Normal;
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

  SymbolTable::Kind SymbolTable::kind(STATE, const Symbol* sym) {
    utilities::thread::SpinLock::LockGuard guard(lock_);
    return kinds[sym->index()];
  }

  size_t SymbolTable::add(std::string str, int enc) {
    bytes_used_ += (str.size() + sizeof(std::string) + sizeof(int) + sizeof(Kind));

    strings.push_back(str);
    encodings.push_back(enc);
    kinds.push_back(detect_kind(str.data(), str.size()));
    return strings.size() - 1;
  }

  Symbol* SymbolTable::lookup(STATE, const char* str, size_t length) {
    return lookup(str, length, Encoding::eAscii, state->hash_seed());
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
    return lookup(str.data(), str.size(), Encoding::eAscii, shared->hash_seed);
  }

  Symbol* SymbolTable::lookup(STATE, const std::string& str) {
    return lookup(str.data(), str.size(), Encoding::eAscii, state->hash_seed());
  }

  Symbol* SymbolTable::lookup(const char* str, size_t length, int enc, uint32_t seed) {
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
        sym = add(std::string(str, length), enc);
        SymbolIds v(1, sym);
        symbols[hash] = v;
      } else {
        SymbolIds& v = entry->second;
        for(SymbolIds::const_iterator i = v.begin(); i != v.end(); ++i) {
          std::string& s = strings[*i];
          int e = encodings[*i];

          if(!strncmp(s.data(), str, length) && e == enc) return Symbol::from_index(*i);
        }
        sym = add(std::string(str, length), enc);
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

    int enc = str->encoding(state)->index();

    if(CBOOL(str->ascii_only_p(state))) {
      enc = Encoding::eAscii;
    }
    return lookup(bytes, size, enc, state->hash_seed());
  }

  String* SymbolTable::lookup_string(STATE, const Symbol* sym) {
    utilities::thread::SpinLock::LockGuard guard(lock_);
    if(sym->nil_p()) {
      Exception::argument_error(state, "Cannot look up Symbol from nil");
      return NULL;
    }

    size_t sym_index = sym->index();
    if(sym_index >= strings.size()) {
      return NULL;
    }
    std::string& str = strings[sym_index];
    int enc = encodings[sym_index];
    String* s = String::create(state, str.data(), str.size());
    s->encoding(state, Encoding::from_index(state, enc));
    return s;
  }

  std::string& SymbolTable::lookup_cppstring(const Symbol* sym) {
    utilities::thread::SpinLock::LockGuard guard(lock_);
    return strings[sym->index()];
  }

  int SymbolTable::lookup_encoding(const Symbol* sym) {
    utilities::thread::SpinLock::LockGuard guard(lock_);
    return encodings[sym->index()];
  }

  std::string SymbolTable::lookup_debug_string(const Symbol* sym) {
    utilities::thread::SpinLock::LockGuard guard(lock_);
    std::string str = strings[sym->index()];
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

  Array* SymbolTable::all_as_array(STATE) {
    utilities::thread::SpinLock::LockGuard guard(lock_);
    size_t idx = 0;
    Array* ary = Array::create(state, strings.size());

    for(SymbolMap::iterator s = symbols.begin(); s != symbols.end(); ++s) {
      for(SymbolIds::iterator i = s->second.begin(); i != s->second.end(); ++i) {
        ary->set(state, idx++, Symbol::from_index(state, *i));
      }
    }

    return ary;
  }
}
