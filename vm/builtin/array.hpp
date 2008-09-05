#ifndef RBX_ARRAY_HPP
#define RBX_ARRAY_HPP

#include "builtin/object.hpp"
#include "builtin/fixnum.hpp"
#include "type_info.hpp"

namespace rubinius {
  class Tuple;

  class Array : public Object {
    public:
    const static size_t fields = 4;
    const static object_type type = ArrayType;

    INTEGER total; // slot
    Tuple* tuple; // slot
    INTEGER start; // slot
    OBJECT shared; // slot

    size_t size();
    static void init(STATE);
    static Array* create(STATE, size_t size);
    static Array* from_tuple(STATE, Tuple* tup);
    void   setup(STATE, size_t size);

    // Ruby.primitive :array_aref
    OBJECT aref(STATE, Fixnum* idx);

    // Ruby.primitive :array_aset
    OBJECT aset(STATE, Fixnum* idx, OBJECT val);

    OBJECT get(STATE, size_t idx);
    OBJECT set(STATE, size_t idx, OBJECT val);
    void   unshift(STATE, OBJECT val);
    OBJECT append(STATE, OBJECT val);
    bool   includes_p(STATE, OBJECT val);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
      virtual void show(STATE, OBJECT self, int level);
    };
  };
};

#endif
