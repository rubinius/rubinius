#ifndef RBX_BUILTIN_SYMBOL_HPP
#define RBX_BUILTIN_SYMBOL_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

namespace rubinius {
  class String;
  class Hash;
  class Tuple;

  /* Symbols are immediate types, but instances of a Symbol are created
   * via SymbolTable::lookup, which is normally accessed through
   * state->lookup.
   */
  class Symbol : public Object {
  public:
    const static size_t fields = 0;
    const static object_type type = SymbolType;

    native_int index();

    // Ruby.primitive :symbol_index
    Integer* index(STATE);

    static void init(STATE);
    static Symbol* from_index(STATE, size_t index);

    // Ruby.primitive :symbol_to_s
    String* to_str(STATE);

    // Return the char* for the text that was symbolized
    const char* c_str(STATE);

    // Ruby.primitive :symbol_all_symbols
    static Array* all_symbols(STATE);

    class Info : public TypeInfo {
    public:
      Info(object_type type, bool cleanup = false) : TypeInfo(type, cleanup) { }
      virtual void mark(Object*, ObjectMark& mark);
      virtual void show(STATE, Object* self, int level);
      virtual void show_simple(STATE, Object* self, int level);
    };
  };

}

#endif
