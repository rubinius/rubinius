#ifndef RBX_ARRAY_HPP
#define RBX_ARRAY_HPP

namespace rubinius {
  class Array : public BuiltinType {
    public:
    const static size_t fields = 4;
    const static object_type type = ArrayType;

    INTEGER total;
    Tuple* tuple;
    INTEGER start;
    OBJECT shared;

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
  };
};

#endif
