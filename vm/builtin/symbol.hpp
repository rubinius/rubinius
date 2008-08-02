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
    INTEGER index(STATE); // {

    static Symbol* from_index(STATE, size_t index);

    String* to_str(STATE);

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
      virtual void mark(OBJECT, ObjectMark& mark);
    };
  };

  /* See t1 */
  template <>
    static bool kind_of<Symbol>(OBJECT obj) {
      return obj->symbol_p();
    }

  typedef Symbol* SYMBOL;

  class SymbolTable : public Object {
  public:
    const static size_t fields = 3;
    const static object_type type = SymbolTableType;

    OBJECT __ivars__; // slot
    Tuple* symbols; // slot
    Hash*  strings; // slot

    static SymbolTable* create(STATE);
    SYMBOL lookup(STATE, const char* str, size_t size = 0);
    SYMBOL lookup(STATE, String* str);
    String* find_string(STATE, Symbol* sym);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
}

#endif
