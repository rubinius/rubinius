#ifndef RBX_BUILTIN_CHARARRAY_HPP
#define RBX_BUILTIN_CHARARRAY_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

#include <ctype.h>

namespace rubinius {
  class Encoding;

  class CharArray : public Object {
  public:
    const static object_type type = CharArrayType;

  private:
    Encoding* encoding_;    // slot
    Object* ascii_;         // slot
    Object* valid_;         // slot
    native_int full_size_;

    // Body access
    uint8_t bytes[0];

  public:
    attr_accessor(encoding, Encoding);
    attr_accessor(ascii, Object);
    attr_accessor(valid, Object);

    static void init(STATE);
    static CharArray* create(STATE, native_int bytes);
    static CharArray* create_pinned(STATE, native_int bytes);

    // Rubinius.primitive :chararray_allocate
    static CharArray* allocate(STATE, Fixnum* bytes);

    // Rubinius.primitive :chararray_size
    Fixnum* size(STATE);

    // Return the number of bytes this CharArray contains
    native_int size() {
      return full_size_ - sizeof(CharArray);
    }

    uint8_t* raw_bytes() {
      return bytes;
    }

    // Rubinius.primitive :chararray_get_byte
    Fixnum* get_byte(STATE, Fixnum* index);

    // Rubinius.primitive :chararray_set_byte
    Fixnum* set_byte(STATE, Fixnum* index, Fixnum* value);

    // Rubinius.primitive :chararray_move_bytes
    Fixnum* move_bytes(STATE, Fixnum* start, Fixnum* count, Fixnum* dest);

    // Rubinius.primitive :chararray_fetch_bytes
    CharArray* fetch_bytes(STATE, Fixnum* start, Fixnum* count);

    // Rubinius.primitive :chararray_compare_bytes
    Fixnum* compare_bytes(STATE, CharArray* other, Fixnum* a, Fixnum* b);

    /* ::locate searches for +pattern+ in the CharArray. Returns the
     * number of characters from the front of the CharArray to the end
     * of the pattern if a match is found. Returns Qnil if a match is
     * not found. Starts searching at index +start+.
     */

    // Rubinius.primitive :chararray_locate
    Object* locate(STATE, String* pattern, Fixnum* start, Fixnum* max);

    // Rubinius.primitive :chararray_prepend
    CharArray* prepend(STATE, String* other);

    // Rubinius.primitive :chararray_get_utf8_char
    Object* get_utf8_char(STATE, Fixnum* offset);

    // Rubinius.primitive :chararray_reverse
    CharArray* reverse(STATE, Fixnum* start, Fixnum* total);

    // Rubinius.primitive :chararray_ascii_only_p
    Object* ascii_only(STATE, Fixnum* num);

    // Rubinius.primitive :chararray_valid_encoding_p
    Object* valid_encoding(STATE, Fixnum* num);

    // This accessor should be used by objects that compose CharArray and
    // delegate their encoding attribute to CharArray (eg String).
    //
    // Rubinius.primitive :chararray_encoding
    Encoding* encoding(STATE);

    char* to_chars(STATE, Fixnum* size);

    class Info : public TypeInfo {
    public:
      Info(object_type type)
        : TypeInfo(type)
      {
        allow_user_allocate = false;
      }

      virtual void auto_mark(Object* obj, ObjectMark& mark);
      virtual void auto_visit(Object* obj, ObjectVisitor& visit);
      virtual size_t object_size(const ObjectHeader* object);
      virtual void set_field(STATE, Object* target, size_t index, Object* val);
      virtual Object* get_field(STATE, Object* target, size_t index);
      virtual void populate_slot_locations();
    };

    friend class Info;
  };
};

#endif
