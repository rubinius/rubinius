#ifndef RBX_BUILTIN_CHARARRAY_HPP
#define RBX_BUILTIN_CHARARRAY_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

#include <ctype.h>

namespace rubinius {
  class CharArray : public Object {
  public:
    const static object_type type = CharArrayType;

  private:
    native_int full_size_;

    // Body access
    uint8_t bytes[0];

  public:
    static void init(STATE);
    static CharArray* create(STATE, native_int bytes);
    static CharArray* create_pinned(STATE, native_int bytes);

    // Ruby.primitive :chararray_allocate
    static CharArray* allocate(STATE, Fixnum* bytes);

    // Ruby.primitive :chararray_size
    Fixnum* size(STATE);

    // Return the number of bytes this CharArray contains
    native_int size() {
      return full_size_ - sizeof(CharArray);
    }

    uint8_t* raw_bytes() {
      return bytes;
    }

    // Ruby.primitive :chararray_get_byte
    Fixnum* get_byte(STATE, Fixnum* index);

    // Ruby.primitive :chararray_set_byte
    Fixnum* set_byte(STATE, Fixnum* index, Fixnum* value);

    // Ruby.primitive :chararray_move_bytes
    Fixnum* move_bytes(STATE, Fixnum* start, Fixnum* count, Fixnum* dest);

    // Ruby.primitive :chararray_fetch_bytes
    CharArray* fetch_bytes(STATE, Fixnum* start, Fixnum* count);

    // Ruby.primitive :chararray_compare_bytes
    Fixnum* compare_bytes(STATE, CharArray* other, Fixnum* a, Fixnum* b);

    /* ::locate searches for +pattern+ in the CharArray. Returns the
     * number of characters from the front of the CharArray to the end
     * of the pattern if a match is found. Returns Qnil if a match is
     * not found. Starts searching at index +start+.
     */

    // Ruby.primitive :chararray_locate
    Object* locate(STATE, String* pattern, Fixnum* start, Fixnum* max);

    // Ruby.primitive :chararray_prepend
    CharArray* prepend(STATE, String* other);

    // Ruby.primitive :chararray_get_utf8_char
    Object* get_utf8_char(STATE, Fixnum* offset);

    // Ruby.primitive :chararray_reverse
    CharArray* reverse(STATE, Fixnum* start, Fixnum* total);

    char* to_chars(STATE, Fixnum* size);

    class Info : public TypeInfo {
    public:
      Info(object_type type)
        : TypeInfo(type)
      {
        allow_user_allocate = false;
      }

      virtual void mark(Object* t, ObjectMark& mark);
      virtual void auto_mark(Object* obj, ObjectMark& mark) {}
      virtual size_t object_size(const ObjectHeader* object);
    };

    friend class Info;
  };
};

#endif
