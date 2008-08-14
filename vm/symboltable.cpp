#include "symboltable.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"

namespace rubinius {
  size_t SymbolTable::add(std::string str) {
    strings.push_back(str);
    return strings.size() - 1;
  }

  SYMBOL SymbolTable::lookup(STATE, std::string str) {
    size_t sym;

    hashval hash = String::hash_str((unsigned char*)str.c_str(), str.size());

    symbol_map::iterator entry = symbols.find(hash);
    if(entry == symbols.end()) {
      sym = add(str);
      symbol_ids v(1, sym);
      symbols[hash] = v;
    } else {
      symbol_ids v = entry->second;
      for(symbol_ids::iterator i = v.begin(); i != v.end(); i++) {
        if(strings[*i] == str) return Symbol::from_index(state, *i);
      }
      sym = add(str);
      v.push_back(sym);
    }

    return Symbol::from_index(state, sym);
  }

  SYMBOL SymbolTable::lookup(STATE, const char* str) {
    return lookup(state, std::string(str));
  }

  SYMBOL SymbolTable::lookup(STATE, String* str) {
    return lookup(state, str->byte_address());
  }

  String* SymbolTable::symbol_to_string(STATE, Symbol* sym) {
    std::string str = strings[sym->index()];
    return String::create(state, str.c_str(), str.size());
  }

  size_t SymbolTable::size() {
    return strings.size();
  }
}
