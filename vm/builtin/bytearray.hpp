#ifndef RBX_BUILTIN_BYTEARRAY_HPP
#define RBX_BUILTIN_BYTEARRAY_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

#include <ctype.h>
#include "object_types.hpp"

namespace rubinius {
  class ByteArray : public Object {
    public:
    const static size_t fields = 0;
    const static object_type type = ByteArrayType;

    static void init(STATE);
    static ByteArray* create(STATE, size_t bytes);

    // Ruby.primitive :bytearray_allocate
    static ByteArray* allocate(STATE, INTEGER bytes);

    // Ruby.primitive :bytearray_size
    INTEGER size(STATE);

    // Return the number of bytes this ByteArray contains
    size_t size() {
      return SIZE_OF_BODY(this);
    }

    // Ruby.primitive :bytearray_get_byte
    FIXNUM get_byte(STATE, INTEGER index);

    // Ruby.primitive :bytearray_set_byte
    FIXNUM set_byte(STATE, INTEGER index, FIXNUM value);

    // Ruby.primitive :bytearray_move_bytes
    INTEGER move_bytes(STATE, INTEGER start, INTEGER count, INTEGER dest);

    // Ruby.primitive :bytearray_fetch_bytes
    ByteArray* fetch_bytes(STATE, INTEGER start, INTEGER count);

    // Ruby.primitive :bytearray_compare_bytes
    FIXNUM compare_bytes(STATE, ByteArray* other, INTEGER a, INTEGER b);

    // Ruby.primitive :bytearray_dup_into
    ByteArray* dup_into(STATE, ByteArray* other);

    // Used like strstr, searching a ByteArray for where a String sequence
    // appears.

    // Ruby.primitive :bytearray_locate
    OBJECT locate(STATE, String* little);

    char* to_chars(STATE);

    class Info : public TypeInfo {
    public:
      Info(object_type type): TypeInfo(type) { }
      virtual void mark(OBJECT t, ObjectMark& mark);
    };
  };
};

#endif
