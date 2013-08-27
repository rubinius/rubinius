#ifndef RBX_SYMBOLTABLE_HPP
#define RBX_SYMBOLTABLE_HPP

#include "oop.hpp"
#include "prelude.hpp"
#include "util/thread.hpp"

#include <string>
#include <vector>

#include "missing/unordered_map.hpp"

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
  class SharedState;

  typedef std::vector<std::string> SymbolStrings;
  typedef std::vector<std::size_t> SymbolIds;
  typedef std_unordered_map<hashval, SymbolIds> SymbolMap;

  class SymbolTable {
  public: // Types

    // We encode in the symbol some information about it, mostly
    // what kind of prefix it has. This lets us ask questions
    // easily about it without having to get the string representation
    // and perform comparisons against the char data.
    enum Kind {
      Normal,
      Constant,
      IVar,
      CVar,
      System
    };

    typedef std::vector<Kind> SymbolKinds;

  private:
    SymbolMap symbols;
    SymbolStrings strings;
    SymbolKinds kinds;
    utilities::thread::SpinLock lock_;
    size_t bytes_used_;

    Symbol* lookup(const char* str, size_t length, uint32_t seed);

  public:

    SymbolTable()
      : bytes_used_(0)
    {
      lock_.init();
    }

    size_t& bytes_used() {
      return bytes_used_;
    }

    Symbol* lookup(SharedState* shared, const std::string& str);
    Symbol* lookup(STATE, const std::string& str);
    Symbol* lookup(STATE, const char* str, size_t length);
    Symbol* lookup(STATE, String* str);
    String* lookup_string(STATE, const Symbol* sym);

    std::string& lookup_cppstring(const Symbol* sym);
    std::string lookup_debug_string(const Symbol* sym);

    size_t size() const;
    Array* all_as_array(STATE);

    Kind kind(STATE, const Symbol* sym) const;

    size_t byte_size() const;

    size_t add(std::string str);
    static Kind detect_kind(const char* str, size_t size);
  };
};

#endif
