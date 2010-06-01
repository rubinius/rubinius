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
    const static object_type type = SymbolType;

    static Symbol* from_index(size_t index) {
      return (Symbol*)APPLY_SYMBOL_TAG(index);
    }

    native_int index() const;

    // Ruby.primitive :symbol_s_eqq
    static Object* is_symbol(STATE, Object* obj) {
      return obj->symbol_p() ? Qtrue : Qfalse;
    }

    // Ruby.primitive :symbol_index
    Integer* index(STATE);

    static void init(STATE);
    static Symbol* from_index(STATE, size_t index);

    // Ruby.primitive :symbol_to_s
    String* to_str(STATE);

    // Return the char* for the text that was symbolized
    const char* c_str(STATE) const;

    // Ruby.primitive :symbol_all_symbols
    static Array* all_symbols(STATE);

    // Ruby.primitive :symbol_is_ivar
    Object* is_ivar_p(STATE);

    // Ruby.primitive :symbol_is_cvar
    Object* is_cvar_p(STATE);

    // Ruby.primitive :symbol_is_constant
    Object* is_constant_p(STATE);

    class Info : public TypeInfo {
    public:
      Info(object_type type, bool cleanup = false)
        : TypeInfo(type, cleanup)
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
