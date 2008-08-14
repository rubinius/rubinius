#ifndef RBX_SYMBOLTABLE_HPP
#define RBX_SYMBOLTABLE_HPP

#include "oop.hpp"
#include "prelude.hpp"

#include <map>
#include <string>
#include <vector>

namespace rubinius {
  class String;

  typedef std::vector<std::size_t>      symbol_ids;
  typedef std::map<hashval, symbol_ids> symbol_map;
  typedef std::vector<std::string>      symbol_strings;

  class SymbolTable {
  public:
    SYMBOL lookup(STATE, std::string str);
    SYMBOL lookup(STATE, const char* str);
    SYMBOL lookup(STATE, String* str);
    String* symbol_to_string(STATE, Symbol* sym);
    size_t size();

  private:
    symbol_map symbols;
    symbol_strings strings;

    size_t add(std::string str);
  };
};

#endif
