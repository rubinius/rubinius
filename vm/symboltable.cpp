#include "vm/symboltable.hpp"
#include "vm/exception.hpp"

#include "builtin/array.hpp"
#include "builtin/exception.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"

namespace rubinius {
  size_t SymbolTable::add(std::string str) {
    strings.push_back(str);
    return strings.size() - 1;
  }

  Symbol* SymbolTable::lookup(STATE, std::string str) {
    size_t sym;

    if(str.size() == 0) {
      Exception::argument_error(state, "cannot create a symbol from an empty string");
    }

    hashval hash = String::hash_str((unsigned char*)str.c_str(), str.size());

    SymbolMap::iterator entry = symbols.find(hash);
    if(entry == symbols.end()) {
      sym = add(str);
      SymbolIds v(1, sym);
      symbols[hash] = v;
    } else {
      SymbolIds& v = entry->second;
      for(SymbolIds::iterator i = v.begin(); i != v.end(); i++) {
        if(strings[*i] == str) return Symbol::from_index(state, *i);
      }
      sym = add(str);
      v.push_back(sym);
    }

    return Symbol::from_index(state, sym);
  }

  Symbol* SymbolTable::lookup(STATE, const char* str) {
    return lookup(state, std::string(str));
  }

  Symbol* SymbolTable::lookup(STATE, String* str) {
    char* bytes = str->c_str();

    for(size_t i = 0; i < str->size(); i++) {
      if(bytes[i] == 0) {
        Exception::argument_error(state,
            "cannot create a symbol from a string containing `\\0'");
      }
    }

    return lookup(state, bytes);
  }

  String* SymbolTable::lookup_string(STATE, Symbol* sym) {
    std::string str = strings[sym->index()];
    // HACK does c_str() return a copy? If so, we need to free() it.
    return String::create(state, str.c_str());
  }

  const char* SymbolTable::lookup_cstring(STATE, Symbol* sym) {
    std::string& str = strings[sym->index()];
    return str.c_str();
  }

  size_t SymbolTable::size() {
    return strings.size();
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
