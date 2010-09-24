#ifndef RBX_BUILTIN_TUPLE_HPP
#define RBX_BUILTIN_TUPLE_HPP

#include "builtin/object.hpp"
#include "builtin/exception.hpp"
#include "type_info.hpp"

namespace rubinius {
  class Tuple : public Object {
  public:
    const static object_type type = TupleType;

    // Must NOT be a native_int, because that changes between sizes
    // between 32 and 64bit, thusly types.gen.cpp will not
    // be portable.
    uint32_t full_size_;

    /* Body access */
    Object* field[0];

  public:
    native_int num_fields() const {
      return (full_size_ - sizeof(Tuple)) / sizeof(Object*);
    }

    static Tuple* create(STATE, native_int fields);
    static Tuple* from(STATE, native_int fields, ...);

    /** Shift all elements leftward, clear old slots. */
    Tuple* lshift_inplace(STATE, Fixnum* shift);


  /** Primitives */
  public:

    // Ruby.primitive :tuple_allocate
    static Tuple* allocate(STATE, Fixnum* fields);

    // Ruby.primitive :tuple_at
    Object* at_prim(STATE, Fixnum* pos);

    Object* put(STATE, native_int idx, Object* val);

    // Ruby.primitive :tuple_put
    Object* put_prim(STATE, Fixnum* idx, Object* val);

    // Ruby.primitive :tuple_fields
    Object* fields_prim(STATE);

    // Ruby.primitive :tuple_pattern
    static Tuple* pattern(STATE, Fixnum* size, Object* val);

    // Ruby.primitive :tuple_copy_from
    Tuple* copy_from(STATE, Tuple* other, Fixnum *start, Fixnum *length, Fixnum *dest);

    // Ruby.primitive :tuple_delete_inplace
    Fixnum* delete_inplace(STATE, Fixnum *start, Fixnum *length, Object *obj);

    // Ruby.primitive :tuple_reverse
    Object* reverse(STATE, Fixnum* start, Fixnum* total);

    Tuple* bounds_exceeded_error(STATE, const char* method, int index);

  public: // Inline Functions
    Object* at(native_int index) {
      if(index < 0 || num_fields() <= index) return Qnil;
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
      virtual void visit(Object* obj, ObjectVisitor& visit);
      virtual void show(STATE, Object* self, int level);
      virtual void show_simple(STATE, Object* self, int level);
      virtual size_t object_size(const ObjectHeader* object);
    };
  };
};

#endif
