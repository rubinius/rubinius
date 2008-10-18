#ifndef RBX_SYMBOLTABLE_HPP
#define RBX_SYMBOLTABLE_HPP

#include "oop.hpp"
#include "prelude.hpp"

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
 * a Symbol* by adding the tag value for symbols. (See builtin
 * class Symbol::from_index and oop.hpp.)
 */
namespace rubinius {

  class Array;
  class String;
  class Symbol;

  typedef std::vector<std::string> SymbolStrings;
  typedef std::vector<std::size_t> SymbolIds;
  typedef std::tr1::unordered_map<hashval, SymbolIds> SymbolMap;

  class SymbolTable {
  public:
    Symbol* lookup(STATE, std::string str);
    Symbol* lookup(STATE, const char* str);
    Symbol* lookup(STATE, String* str);
    String* lookup_string(STATE, Symbol* sym);
    const char* lookup_cstring(STATE, Symbol* sym);
    size_t size();
    Array* all_as_array(STATE);

  private:
    SymbolMap symbols;
    SymbolStrings strings;

    size_t add(std::string str);
  };
};

#endif
