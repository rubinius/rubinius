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

    OBJECT at(size_t index);
    OBJECT put(STATE, size_t idx, OBJECT val);
    void copy_from(STATE, Tuple* other, int start, int end);

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
      virtual void mark(OBJECT t, ObjectMark& mark);
    };
  };
};

#endif
