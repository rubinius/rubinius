#include "vm/symboltable.hpp"
#include "vm/exception.hpp"

#include "builtin/array.hpp"
#include "builtin/exception.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"

namespace rubinius {

  SymbolTable::Kind SymbolTable::detect_kind(const char* str, int size) {
    const char one = str[0];

    // A constant begins with an uppercase letter.
    if(one >= 'A' && one <= 'Z') {
      // Make sure that the rest of it is only alphanumerics
      for(int i = 1; i < size; i++) {
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

  SymbolTable::Kind SymbolTable::kind(STATE, const Symbol* sym) {
    return kinds[sym->index()];
  }

  size_t SymbolTable::add(std::string str) {
    bytes_used_ += (str.size() + sizeof(str));

    strings.push_back(str);
    kinds.push_back(detect_kind(str.c_str(), str.size()));
    return strings.size() - 1;
  }

  Symbol* SymbolTable::lookup(STATE, const char* str) {
    Symbol* sym = lookup(str);
    if(!sym) {
      Exception::argument_error(state, "Cannot create a symbol from an empty string");
      return NULL;
    }

    return sym;
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

  static const char* find_special(const char* check) {
    for(int i = 0; i < cNumSpecialOperators; i++) {
      SpecialOperator* op = &SpecialOperators[i];
      if(*op->name == *check && strcmp(op->name, check) == 0) {
        return op->symbol;
      }
    }

    return 0;
  }


  Symbol* SymbolTable::lookup(const char* str) {
    size_t sym;

    if(*str == 0) return NULL;

    if(const char* op = find_special(str)) {
      str = op;
    }

    hashval hash = String::hash_str(str);

    // Symbols can be looked up by multiple threads at the same time.
    // This is fast operation, so we protect this with a spinlock.
    {
      thread::SpinLock::LockGuard guard(lock_);
      SymbolMap::iterator entry = symbols.find(hash);
      if(entry == symbols.end()) {
        sym = add(std::string(str));
        SymbolIds v(1, sym);
        symbols[hash] = v;
      } else {
        SymbolIds& v = entry->second;
        for(SymbolIds::iterator i = v.begin(); i != v.end(); i++) {
          std::string& s = strings[*i];

          if(!strcmp(s.c_str(), str)) return Symbol::from_index(*i);
        }
        sym = add(std::string(str));
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

    const char* bytes = str->c_str(state);

    for(native_int i = 0; i < str->size(); i++) {
      if(bytes[i] == 0) {
        Exception::argument_error(state,
            "cannot create a symbol from a string containing `\\0'");
        return NULL;
      }
    }

    return lookup(state, bytes);
  }

  String* SymbolTable::lookup_string(STATE, const Symbol* sym) {
    if(sym->nil_p()) {
      Exception::argument_error(state, "Cannot look up Symbol from nil");
      return NULL;
    }

    std::string& str = strings[sym->index()];
    return String::create(state, str.c_str());
  }

  const char* SymbolTable::lookup_cstring(STATE, const Symbol* sym) {
    if(sym->nil_p()) {
      Exception::argument_error(state, "Cannot look up Symbol from nil");
      return NULL;
    }

    std::string& str = strings[sym->index()];
    return str.c_str();
  }

  const char* SymbolTable::lookup_cstring(const Symbol* sym) {
    std::string& str = strings[sym->index()];
    return str.c_str();
  }

  std::string& SymbolTable::lookup_cppstring(const Symbol* sym) {
    return strings[sym->index()];
  }

  size_t SymbolTable::size() {
    return strings.size();
  }

  int SymbolTable::byte_size() {
    int total = 0;

    for(SymbolStrings::iterator i = strings.begin();
        i != strings.end();
        i++) {
      total += i->size();
      total += sizeof(std::string);
    }

    return total;
  }

  Array* SymbolTable::all_as_array(STATE) {
    size_t idx = 0;
    Array* ary = Array::create(state, this->size());

    for(SymbolMap::iterator s = symbols.begin(); s != symbols.end(); s++) {
      for(SymbolIds::iterator i = s->second.begin(); i != s->second.end(); i++) {
        ary->set(state, idx++, (Object*)Symbol::from_index(state, *i));
      }
    }

    return ary;
  }
}
