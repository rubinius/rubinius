#ifndef RBX_SYMBOLTABLE_HPP
#define RBX_SYMBOLTABLE_HPP

#include "oop.hpp"
#include "prelude.hpp"

#include <map>
#include <string>
#include <vector>
#include <tr1/unordered_map>

/* SymbolTable provides a one-to-one map between a symbol ID
 * and a string. The hashing algorithm used to generate a
 * hashval from a string is not perfect. For instance, the
 * strings "__uint_fast64_t" and "TkIF_MOD" generate the same
 * hashval. SymbolTable handles this by first looking up a
 * symbol ID for a string by the string's hashval and then
 * comparing the candidate string with the stored string that
 * corresponds to the possible symbol ID.
 *
 * When a symbol ID is generated, the string corresponding to
 * that symbol ID is stored in a vector. The symbol ID is the
 * index of that string in the vector. The symbol ID becomes
 * a SYMBOL by adding the tag value for symbols. (See builtin
 * class Symbol::from_index and oop.hpp.)
 */
namespace rubinius {
  class String;

  typedef std::vector<std::string> symbol_strings;
  typedef std::vector<std::size_t> symbol_ids;
  typedef std::tr1::unordered_map<hashval, symbol_ids> symbol_map;

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
