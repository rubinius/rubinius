#ifndef RBX_BUILTIN_SYMBOL_HPP
#define RBX_BUILTIN_SYMBOL_HPP

#include "builtin/object.hpp"

namespace rubinius {
  class Encoding;
  class String;
  class Tuple;

  /* Symbols are immediate types, but instances of a Symbol are created
   * via SymbolTable::lookup, which is normally accessed through
   * state->lookup.
   */
  class Symbol : public Object {
  public:
    const static object_type type = SymbolType;

    static Symbol* from_index(size_t index) {
      return reinterpret_cast<Symbol*>(APPLY_SYMBOL_TAG(index));
    }

    native_int index() const;

    // Rubinius.primitive+ :symbol_s_eqq
    static Object* is_symbol(STATE, Object* obj) {
      return RBOOL(obj->symbol_p());
    }

    // Rubinius.primitive+ :symbol_index
    Integer* index(STATE);

    static void init(STATE);
    static Symbol* from_index(STATE, size_t index);

    // Rubinius.primitive :symbol_to_s
    String* to_str(STATE);

    // Return the underlying std::string from the symbol table
    std::string& cpp_str(STATE);

    // Return a representation to be used when debugging
    std::string debug_str(STATE);
    std::string debug_str(SharedState& shared);

    // Rubinius.primitive :symbol_all_symbols
    static Array* all_symbols(STATE);

    // Rubinius.primitive+ :symbol_is_ivar
    Object* is_ivar_p(STATE);

    // Rubinius.primitive+ :symbol_is_cvar
    Object* is_cvar_p(STATE);

    // Rubinius.primitive+ :symbol_is_constant
    Object* is_constant_p(STATE);

    // Rubinius.primitive+ :symbol_encoding
    Encoding* encoding(STATE);

    Encoding* encoding(STATE, Encoding* enc);

    class Info : public TypeInfo {
    public:
      Info(object_type type)
        : TypeInfo(type)
      {
        allow_user_allocate = false;
      }

      virtual void mark(Object*, ObjectMark& mark);
      virtual void show(STATE, Object* self, int level);
      virtual void show_simple(STATE, Object* self, int level);
      virtual void auto_mark(Object* obj, ObjectMark& mark) {}
    };
  };

}

#endif
