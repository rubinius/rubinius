#ifndef RBX_BUILTIN_MEMORYPOINTER_HPP
#define RBX_BUILTIN_MEMORYPOINTER_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

namespace rubinius {
  class MemoryPointer : public Object {
    public:
    const static size_t fields = 0;
    const static object_type type = MemPtrType;

    void* pointer;
    bool autorelease;

    static MemoryPointer* create(STATE, void* ptr);

    // Ruby.primitive :memorypointer_address
    OBJECT get_address(STATE);

    // Ruby.primitive :memorypointer_set_autorelease
    OBJECT set_autorelease(STATE, OBJECT val);

    // Ruby.primitive :memorypointer_add
    MemoryPointer* add(STATE, FIXNUM ammount);

    // Ruby.primitive :memorypointer_write_int
    OBJECT write_int(STATE, FIXNUM val);

    // Ruby.primitive :memorypointer_read_int
    OBJECT read_int(STATE);

    // Ruby.primitive :memorypointer_write_long
    OBJECT write_long(STATE, FIXNUM val);

    // Ruby.primitive :memorypointer_read_long
    OBJECT read_long(STATE);

    // Ruby.primitive :memorypointer_write_float
    Float* write_float(STATE, Float* val);

    // Ruby.primitive :memorypointer_read_float
    Float* read_float(STATE);

    // Ruby.primitive :memorypointer_read_pointer
    MemoryPointer* read_pointer(STATE);

    // Ruby.primitive :memorypointer_read_string
    OBJECT read_string(STATE, FIXNUM len);

    // Ruby.primitive :memorypointer_read_string_to_null
    OBJECT read_string_to_null(STATE);

    // Ruby.primitive :memorypointer_write_string
    OBJECT write_string(STATE, String* str, FIXNUM len);

    OBJECT get_field(STATE, int offset, int type);

    // Ruby.primitive :memorypointer_get_field
    OBJECT get_field_prim(STATE, FIXNUM offset, FIXNUM type);

    void   set_field(STATE, int offset, int type, OBJECT val);

    // Ruby.primitive :memorypointer_set_field
    OBJECT set_field_prim(STATE, FIXNUM offset, FIXNUM type, OBJECT val);

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
      virtual void mark(OBJECT t, ObjectMark& mark);
    };
  };
}

#endif
