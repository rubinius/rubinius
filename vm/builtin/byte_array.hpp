#ifndef RBX_BUILTIN_BYTEARRAY_HPP
#define RBX_BUILTIN_BYTEARRAY_HPP

#include "builtin/object.hpp"

namespace rubinius {
  class ByteArray : public Object {
  public:
    const static object_type type = ByteArrayType;
    static uintptr_t bytes_offset;

  private:
    native_int full_size_;

    // Body access
    uint8_t bytes[0];

  public:
    static void init(STATE);
    static ByteArray* create(STATE, native_int bytes);
    static ByteArray* create_pinned(STATE, native_int bytes);
    static ByteArray* create_dirty(STATE, native_int bytes);

    template <typename Any>
      static ByteArray* from_body(Any obj) {
        uintptr_t ptr = reinterpret_cast<uintptr_t>(obj);
        return reinterpret_cast<ByteArray*>(ptr - bytes_offset);
      }

    // Rubinius.primitive :bytearray_allocate
    static ByteArray* allocate(STATE, Fixnum* bytes);

    // Rubinius.primitive+ :bytearray_size
    Fixnum* size(STATE);

    // Return the number of bytes this ByteArray contains
    native_int size() const {
      return full_size_ - bytes_offset;
    }

    uint8_t* raw_bytes() {
      return bytes;
    }

    // Rubinius.primitive :bytearray_get_byte
    Fixnum* get_byte(STATE, Fixnum* index);

    // Rubinius.primitive :bytearray_set_byte
    Fixnum* set_byte(STATE, Fixnum* index, Fixnum* value);

    // Rubinius.primitive :bytearray_move_bytes
    Fixnum* move_bytes(STATE, Fixnum* start, Fixnum* count, Fixnum* dest);

    // Rubinius.primitive :bytearray_fetch_bytes
    ByteArray* fetch_bytes(STATE, Fixnum* start, Fixnum* count);

    // Rubinius.primitive :bytearray_compare_bytes
    Fixnum* compare_bytes(STATE, ByteArray* other, Fixnum* a, Fixnum* b);

    /* ::locate searches for +pattern+ in the ByteArray. Returns the
     * number of characters from the front of the ByteArray to the end
     * of the pattern if a match is found. Returns cNil if a match is
     * not found. Starts searching at index +start+.
     */

    // Rubinius.primitive :bytearray_locate
    Object* locate(STATE, String* pattern, Fixnum* start, Fixnum* max);

    // Rubinius.primitive :bytearray_prepend
    ByteArray* prepend(STATE, String* other);

    // Rubinius.primitive :bytearray_reverse
    ByteArray* reverse(STATE, Fixnum* start, Fixnum* total);

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
