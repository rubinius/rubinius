#ifndef RBX_BUILTIN_MEMORYPOINTER_HPP
#define RBX_BUILTIN_MEMORYPOINTER_HPP

#include "builtin/object.hpp"

namespace rubinius {
  class Float;
  class Integer;
  class String;

  class Pointer : public Object {
  public:
    const static object_type type = PointerType;

    void* pointer;
    bool autorelease;
    bool set_finalizer;

    static void init(STATE);

    static Pointer* create(STATE, void* ptr);

    static void finalize(STATE, Pointer* ptr);

    // Rubinius.primitive :pointer_allocate
    static Pointer* allocate(STATE, Object* self);

    // Rubinius.primitive :pointer_malloc
    static Pointer* allocate_memory(STATE, Object* self, Fixnum* bytes);

    // Rubinius.primitive+ :pointer_address
    Integer* get_address(STATE);

    // Rubinius.primitive+ :pointer_set_address
    Integer* set_address(STATE, Integer* ptr);

    // Rubinius.primitive :pointer_set_autorelease
    Object* set_autorelease(STATE, Object* val);

    // Rubinius.primitive :pointer_add
    Pointer* add(STATE, Integer* amount);

    // Rubinius.primitive :pointer_write_char
    Integer* write_char(STATE, Integer* val);

    // Rubinius.primitive :pointer_read_char
    Integer* read_char(STATE, Object* sign);

    // Rubinius.primitive :pointer_write_short
    Integer* write_short(STATE, Integer* val);

    // Rubinius.primitive :pointer_read_short
    Integer* read_short(STATE, Object* sign);

    // Rubinius.primitive :pointer_write_int
    Integer* write_int(STATE, Integer* val);

    // Rubinius.primitive :pointer_read_int
    Integer* read_int(STATE, Object* sign);

    // Rubinius.primitive :pointer_write_long
    Integer* write_long(STATE, Integer* val);

    // Rubinius.primitive :pointer_read_long
    Integer* read_long(STATE, Object* sign);

    // Rubinius.primitive :pointer_write_long_long
    Integer* write_long_long(STATE, Integer* val);

    // Rubinius.primitive :pointer_read_long_long
    Integer* read_long_long(STATE, Object* sign);

    // Rubinius.primitive :pointer_write_float
    Float* write_float(STATE, Float* val);

    // Rubinius.primitive :pointer_read_float
    Float* read_float(STATE);

    // Rubinius.primitive :pointer_write_double
    Float* write_double(STATE, Float* val);

    // Rubinius.primitive :pointer_read_double
    Float* read_double(STATE);

    // Rubinius.primitive :pointer_write_pointer
    Pointer* write_pointer(STATE, Pointer* ptr);

    // Rubinius.primitive :pointer_read_pointer
    Pointer* read_pointer(STATE);

    // Rubinius.primitive :pointer_read_string
    String* read_string(STATE, Fixnum* len);

    // Rubinius.primitive :pointer_read_string_to_null
    String* read_string_to_null(STATE);

    // Rubinius.primitive :pointer_write_string
    Pointer* write_string(STATE, String* str, Fixnum* len);

    Object* get_field(STATE, int offset, int type);

    // Rubinius.primitive :pointer_get_at_offset
    Object* get_at_offset(STATE, Fixnum* offset, Fixnum* type);

    void   set_field(STATE, int offset, int type, Object* val);

    // Rubinius.primitive :pointer_set_at_offset
    Object* set_at_offset(STATE, Fixnum* offset, Fixnum* type, Object* val);

    // Rubinius.primitive :pointer_network_order
    Object* network_order(STATE, Fixnum* offset, Fixnum* intsize);

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
      virtual void mark(Object* t, ObjectMark& mark);
      virtual void auto_mark(Object* obj, ObjectMark& mark) {}
    };
  };
}

#endif
