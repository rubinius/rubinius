#ifndef RBX_BUILTIN_SYMBOL_HPP
#define RBX_BUILTIN_SYMBOL_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

namespace rubinius {
  class String;
  class Hash;
  class Tuple;

  class Symbol : public Object {
  public:
    const static size_t fields = 0;
    const static object_type type = SymbolType;

    native_int index();

    // Ruby.primitive :symbol_index
    INTEGER index(STATE);

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
      Info(object_type type) : TypeInfo(type) { }
      virtual void mark(OBJECT, ObjectMark& mark);
    };
  };

  typedef Symbol* SYMBOL;
}

#endif
