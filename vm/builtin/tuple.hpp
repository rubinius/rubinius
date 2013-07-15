#ifndef RBX_BUILTIN_TUPLE_HPP
#define RBX_BUILTIN_TUPLE_HPP

#include "builtin/object.hpp"

namespace rubinius {
  class Tuple : public Object {
  public:
    const static object_type type = TupleType;
    static uintptr_t fields_offset;

    native_int full_size_;

    /* Body access */
    Object* field[0];

  public:
    native_int num_fields() const {
      return (full_size_ - fields_offset) / sizeof(Object*);
    }

    static void init(STATE);
    static Tuple* create(STATE, native_int fields);
    static Tuple* from(STATE, native_int fields, ...);

    /** Shift all elements leftward, clear old slots. */
    Tuple* lshift_inplace(STATE, Fixnum* shift);


  /** Primitives */
  public:

    // Rubinius.primitive :tuple_allocate
    static Tuple* allocate(STATE, Fixnum* fields);

    // Rubinius.primitive :tuple_at
    Object* at_prim(STATE, Fixnum* pos);

    Object* put(STATE, native_int idx, Object* val);

    // Rubinius.primitive :tuple_put
    Object* put_prim(STATE, Fixnum* idx, Object* val);

    // Rubinius.primitive+ :tuple_fields
    Object* fields_prim(STATE);

    // Rubinius.primitive :tuple_pattern
    static Tuple* pattern(STATE, Fixnum* size, Object* val);

    // Rubinius.primitive :tuple_copy_from
    Tuple* copy_from(STATE, Tuple* other, Fixnum *start, Fixnum *length, Fixnum *dest);

    native_int delete_inplace(native_int start, native_int length, Object* obj);
    // Rubinius.primitive :tuple_delete_inplace
    Fixnum* delete_inplace_prim(STATE, Fixnum *start, Fixnum *length, Object *obj);

    // Rubinius.primitive :tuple_reverse
    Object* reverse(STATE, Fixnum* start, Fixnum* total);

    Tuple* bounds_exceeded_error(STATE, const char* method, int index);

    // Rubinius.primitive :tuple_dup
    Tuple* tuple_dup(STATE);

  public: // Inline Functions
    Object* at(native_int index) {
      if(index < 0 || num_fields() <= index) return cNil;
      return field[index];
    }

    Object* at(STATE, native_int index) {
      return at(index);
    }

  public: // Rubinius Type stuff
    class Info : public TypeInfo {
    public:
      Info(object_type type)
        : TypeInfo(type)
      {
        allow_user_allocate = false;
      }

      virtual void mark(Object* obj, ObjectMark& mark);
      virtual void auto_mark(Object* obj, ObjectMark& mark) {}
      virtual void show(STATE, Object* self, int level);
      virtual void show_simple(STATE, Object* self, int level);
      virtual size_t object_size(const ObjectHeader* object);
    };
  };
};

#endif
