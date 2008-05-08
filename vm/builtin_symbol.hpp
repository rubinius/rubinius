#ifndef RBX_BUILTIN_SYMBOL_HPP
#define RBX_BUILTIN_SYMBOL_HPP

namespace rubinius {

  class String;
  class Hash;
  class Tuple;

  class Symbol : public BuiltinType {
  public:
    const static size_t fields = 0;
    const static object_type type = SymbolType;

    native_int index() {
      return DATA_STRIP_TAG(this);
    }

    static Symbol* from_index(STATE, size_t index) {
      return (Symbol*)DATA_APPLY_TAG(index, DATA_TAG_SYMBOL);
    }

    String* to_str(STATE);

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
    };
  };

  /* See t1 */
  template <>
    static bool kind_of<Symbol>(OBJECT obj) {
      return obj->symbol_p();
    }

  typedef Symbol* SYMBOL;

  class SymbolTable : public BuiltinType {
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
      Info(object_type type) : TypeInfo(type) { }
      virtual void set_field(STATE, OBJECT target, size_t index, OBJECT val);
      virtual OBJECT get_field(STATE, OBJECT target, size_t index);
    };
  };
}

#endif
