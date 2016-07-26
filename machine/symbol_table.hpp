#ifndef RBX_SYMBOLTABLE_HPP
#define RBX_SYMBOLTABLE_HPP

#include "memory/header.hpp"

#include "defines.hpp"
#include "diagnostics.hpp"

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
  typedef std::vector<int> SymbolEncodings;
  typedef std::vector<std::size_t> SymbolIds;
  typedef std_unordered_map<hashval, SymbolIds> SymbolMap;

  class SymbolTable {
  public:
    class Diagnostics : public diagnostics::MemoryDiagnostics {
    public:
      Diagnostics()
        : diagnostics::MemoryDiagnostics()
      { }

      void update();
    };

  public: // Types

    // We encode in the symbol some information about it, mostly
    // what kind of prefix it has. This lets us ask questions
    // easily about it without having to get the string representation
    // and perform comparisons against the char data.
    enum Kind {
      eUnknown,
      eNormal,
      eConstant,
      eIVar,
      eCVar,
      eSystem
    };

    typedef std::vector<Kind> SymbolKinds;

  private:
    SymbolMap symbols;
    SymbolStrings strings;
    SymbolEncodings encodings;
    SymbolKinds kinds;
    utilities::thread::SpinLock lock_;
    Diagnostics* diagnostics_;

  public:

    SymbolTable()
      : diagnostics_(new Diagnostics())
    {
      lock_.init();
    }

    virtual ~SymbolTable() {
      if(diagnostics()) delete diagnostics();
    }

    Diagnostics* diagnostics() {
      return diagnostics_;
    }

    size_t size() {
      return symbols.size();
    };

    Symbol* lookup(STATE, SharedState* shared, const std::string& str);
    Symbol* lookup(STATE, const std::string& str);
    Symbol* lookup(STATE, const char* str, size_t length);
    Symbol* lookup(STATE, const char* str, size_t length, int enc, uint32_t seed);
    Symbol* lookup(STATE, String* str);
    String* lookup_string(STATE, const Symbol* sym);

    std::string& lookup_cppstring(const Symbol* sym);
    int lookup_encoding(const Symbol* sym);
    std::string lookup_debug_string(const Symbol* sym);

    Array* all_as_array(STATE);

    Kind kind(STATE, const Symbol* sym);

    size_t add(STATE, std::string str, int enc);
    Kind detect_kind(STATE, const Symbol* sym);
  };
};

#endif
