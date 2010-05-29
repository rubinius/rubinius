#ifndef RBX_BUILTIN_MEMORYPOINTER_HPP
#define RBX_BUILTIN_MEMORYPOINTER_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

namespace rubinius {
  class Pointer : public Object {
  public:
    const static object_type type = PointerType;

    void* pointer;
    bool autorelease;
    bool set_finalizer;

    static void init(STATE);

    static Pointer* create(STATE, void* ptr);

    static void finalize(STATE, Pointer* ptr);

    // Ruby.primitive :pointer_allocate
    static Pointer* allocate(STATE, Object* self);

    // Ruby.primitive :pointer_malloc
    static Pointer* allocate_memory(STATE, Object* self, Fixnum* bytes);

    // Ruby.primitive :pointer_address
    Integer* get_address(STATE);

    // Ruby.primitive :pointer_set_address
    Integer* set_address(STATE, Integer* ptr);

    // Ruby.primitive :pointer_set_autorelease
    Object* set_autorelease(STATE, Object* val);

    // Ruby.primitive :pointer_add
    Pointer* add(STATE, Integer* amount);

    // Ruby.primitive :pointer_write_short
    Integer* write_short(STATE, Integer* val);

    // Ruby.primitive :pointer_read_short
    Integer* read_short(STATE);

    // Ruby.primitive :pointer_write_int
    Integer* write_int(STATE, Integer* val);

    // Ruby.primitive :pointer_read_int
    Integer* read_int(STATE, Object* sign);

    // Ruby.primitive :pointer_write_long
    Integer* write_long(STATE, Integer* val);

    // Ruby.primitive :pointer_read_long
    Integer* read_long(STATE);

    // Ruby.primitive :pointer_write_long_long
    Integer* write_long_long(STATE, Integer* val);

    // Ruby.primitive :pointer_read_long_long
    Integer* read_long_long(STATE);

    // Ruby.primitive :pointer_write_float
    Float* write_float(STATE, Float* val);

    // Ruby.primitive :pointer_read_float
    Float* read_float(STATE);

    // Ruby.primitive :pointer_write_double
    Float* write_double(STATE, Float* val);

    // Ruby.primitive :pointer_read_double
    Float* read_double(STATE);

    // Ruby.primitive :pointer_read_pointer
    Pointer* read_pointer(STATE);

    // Ruby.primitive :pointer_read_string
    String* read_string(STATE, Fixnum* len);

    // Ruby.primitive :pointer_read_string_to_null
    String* read_string_to_null(STATE);

    // Ruby.primitive :pointer_write_string
    Pointer* write_string(STATE, String* str, Fixnum* len);

    Object* get_field(STATE, int offset, int type);

    // Ruby.primitive :pointer_get_at_offset
    Object* get_at_offset(STATE, Fixnum* offset, Fixnum* type);

    void   set_field(STATE, int offset, int type, Object* val);

    // Ruby.primitive :pointer_set_at_offset
    Object* set_at_offset(STATE, Fixnum* offset, Fixnum* type, Object* val);

    // Ruby.primitive :pointer_network_order
    Object* network_order(STATE, Fixnum* offset, Fixnum* intsize);

    class Info : public TypeInfo {
    public:
      Info(object_type type, bool cleanup = false) : TypeInfo(type, cleanup) { }
      virtual void mark(Object* t, ObjectMark& mark);
      virtual void auto_mark(Object* obj, ObjectMark& mark) {}
    };
  };
}

#endif
