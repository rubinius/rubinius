#include "symbol_table.hpp"
#include "exception.hpp"
#include "configuration.hpp"

#include "builtin/array.hpp"
#include "builtin/exception.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"

#include "util/logger.hpp"

#include <iostream>
#include <iomanip>

namespace rubinius {
  void SymbolTable::Diagnostics::log() {
    if(!modified_p()) return;

    diagnostics::Diagnostics::log();

    utilities::logger::write("symbol table: diagnostics: symbols: %ld, bytes: %ld",
        objects_, bytes_);
  }

  SymbolTable::Kind SymbolTable::detect_kind(STATE, const Symbol* sym) {
    std::string str = strings[sym->index()];
    size_t size = str.size();
    uint8_t* p = reinterpret_cast<uint8_t*>(const_cast<char*>(str.c_str()));

    // Constants start with A-Z, followed by alphanumeric characters or '_'.
    if(isupper(*p)) {
      for(size_t i = 1; i < size; i++) {
        if(!isalnum(p[i]) && p[i] != '_') {
          return SymbolTable::eNormal;
        }
      }

      return SymbolTable::eConstant;
    }

    if(p[0] == '@') {
      // A class variable begins with @@
      if(size > 1 && p[1] == '@') {
        return SymbolTable::eCVar;
      }

      // An instance variable can't start with a digit
      if(size > 1 && ISDIGIT(str[1])) {
        return SymbolTable::eNormal;
      }

      // An instance variable begins with @
      return SymbolTable::eIVar;
    }

    // A system variable begins with __
    if(size > 2 && p[0] == '_' && p[1] == '_') {
      return SymbolTable::eSystem;
    }

    // Everything else is normal
    return SymbolTable::eNormal;
  }

  SymbolTable::Kind SymbolTable::kind(STATE, const Symbol* sym) {
    utilities::thread::SpinLock::LockGuard guard(lock_);

    Kind k = kinds[sym->index()];

    if(k == eUnknown) {
      k = kinds[sym->index()] = detect_kind(state, sym);
    }

    return k;
  }

  size_t SymbolTable::add(STATE, std::string str) {
    size_t bytes = (str.size() + sizeof(std::string) + sizeof(Kind));

    diagnostics_.objects_++;
    diagnostics_.bytes_ += bytes;
    diagnostics_.modify();

    strings.push_back(str);
    kinds.push_back(eUnknown);

    state->vm()->metrics().memory.symbols++;
    state->vm()->metrics().memory.symbols_bytes += bytes;

    return strings.size() - 1;
  }

  Symbol* SymbolTable::lookup(STATE, const char* str, size_t length) {
    return lookup(state, str, length, state->hash_seed());
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

  Symbol* SymbolTable::lookup(STATE, SharedState* shared, const std::string& str) {
    return lookup(state, str.data(), str.size(), shared->hash_seed);
  }

  Symbol* SymbolTable::lookup(STATE, const std::string& str) {
    return lookup(state, str.data(), str.size(), state->hash_seed());
  }

  Symbol* SymbolTable::lookup(STATE, const char* str, size_t length, uint32_t seed) {
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
        sym = add(state, std::string(str, length));
        SymbolIds v(1, sym);
        symbols[hash] = v;
      } else {
        SymbolIds& v = entry->second;
        for(SymbolIds::const_iterator i = v.begin(); i != v.end(); ++i) {
          std::string& s = strings[*i];

          if(!strncmp(s.data(), str, length)) return Symbol::from_index(*i);
        }
        sym = add(state, std::string(str, length));
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
    size_t size = str->size();

    if(size == 0) {
      Exception::argument_error(state, "Cannot create a symbol from an empty string");
      return NULL;
    }

    if(strnlen(bytes, size) < size) {
      Exception::argument_error(state,
          "cannot create a symbol from a string containing `\\0'");
      return NULL;
    }

    return lookup(state, bytes, size, state->hash_seed());
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
    String* s = String::create(state, str.data(), str.size());
    return s;
  }

  std::string& SymbolTable::lookup_cppstring(const Symbol* sym) {
    utilities::thread::SpinLock::LockGuard guard(lock_);
    return strings[sym->index()];
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
