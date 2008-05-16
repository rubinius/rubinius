#ifndef RBX_ARRAY_HPP
#define RBX_ARRAY_HPP

namespace rubinius {
  class Array : public BuiltinType {
    public:
    const static size_t fields = 5;
    const static object_type type = ArrayType;

    OBJECT __ivars__; // slot
    INTEGER total; // slot
    Tuple* tuple; // slot
    INTEGER start; // slot
    OBJECT shared; // slot

    size_t size() {
      return total->n2i();
    }

    static Array* create(STATE, size_t size);
    static Array* from_tuple(STATE, Tuple* tup);
    void   setup(STATE, size_t size);
    OBJECT get(STATE, size_t idx);
    OBJECT set(STATE, size_t idx, OBJECT val);
    OBJECT append(STATE, OBJECT val);
    bool   includes_p(STATE, OBJECT val);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
};

#endif
