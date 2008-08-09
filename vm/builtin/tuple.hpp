#ifndef RBX_BUILTIN_TUPLE_HPP
#define RBX_BUILTIN_TUPLE_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

namespace rubinius {
  class Tuple : public Object {
  public:
    const static size_t fields = 0;
    const static object_type type = TupleType;

    static Tuple* create(STATE, size_t fields);
    static Tuple* from(STATE, size_t fields, ...);

    // Ruby.primitive :tuple_allocate
    static Tuple* allocate(STATE, FIXNUM fields);

    OBJECT at(size_t index);

    // Ruby.primitive :tuple_at
    OBJECT at_prim(STATE, FIXNUM pos);

    OBJECT put(STATE, size_t idx, OBJECT val);

    // Ruby.primitive :tuple_put
    OBJECT put_prim(STATE, FIXNUM idx, OBJECT val);

    // Ruby.primitive :tuple_fields
    OBJECT fields_prim(STATE);

    void copy_from(STATE, Tuple* other, int start, int end);

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
      virtual void mark(OBJECT t, ObjectMark& mark);
    };
  };
};

#endif
