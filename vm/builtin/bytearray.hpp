#ifndef RBX_BUILTIN_BYTEARRAY_HPP
#define RBX_BUILTIN_BYTEARRAY_HPP

#include <ctype.h>
#include "object_types.hpp"
#include "builtin/object.hpp"
#include "type_info.hpp"

namespace rubinius {
  class ByteArray : public Object {
    public:
    const static size_t fields = 0;
    const static object_type type = ByteArrayType;

    static ByteArray* create(STATE, size_t bytes);

    // Ruby.primitive :bytearray_size
    INTEGER size(STATE);

    class Info : public TypeInfo {
    public:
      Info(object_type type): TypeInfo(type) { }
      virtual void mark(OBJECT t, ObjectMark& mark);
    };
  };
};

#endif
