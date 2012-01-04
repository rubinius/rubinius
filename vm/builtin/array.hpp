#ifndef RBX_ARRAY_HPP
#define RBX_ARRAY_HPP

#include "builtin/object.hpp"
#include "builtin/fixnum.hpp"
#include "type_info.hpp"

namespace rubinius {
  class Tuple;

  class Array : public Object {
  public:
    const static object_type type = ArrayType;

  private:
    Fixnum* total_; // slot
    Tuple* tuple_;  // slot
    Fixnum* start_; // slot

  public:
    /* accessors */

    attr_accessor(total, Fixnum);
    attr_accessor(tuple, Tuple);
    attr_accessor(start, Fixnum);

    /* interface */

    size_t size();
    size_t offset();
    static void init(STATE);
    static Array* create(STATE, size_t size);
    static Array* from_tuple(STATE, Tuple* tup);
    static Array* to_ary(STATE, Object* obj, CallFrame* frame);

    void   setup(STATE, size_t size);

    // Rubinius.primitive :array_allocate
    static Array* allocate(STATE, Object* self);

    // Rubinius.primitive :array_new_range
    Array* new_range(STATE, Fixnum* start, Fixnum* count);

    // Rubinius.primitive :array_new_reserved
    Array* new_reserved(STATE, Fixnum* count);

    // Rubinius.primitive :array_aref
    Object* aref(STATE, Fixnum* idx);

    // Rubinius.primitive :array_aset
    Object* aset(STATE, Fixnum* idx, Object* val);

    // Rubinius.primitive :array_concat
    Array* concat(STATE, Array* other);

    // Rubinius.primitive :array_pack18
    String* pack18(STATE, String* directives, CallFrame* calling_environment);

    // Rubinius.primitive :array_pack19
    String* pack19(STATE, String* directives, CallFrame* calling_environment);

    Object* get(STATE, size_t idx);
    Object* set(STATE, size_t idx, Object* val);
    void   unshift(STATE, Object* val);
    Object* shift(STATE);
    Object* append(STATE, Object* val);
    Object* pop(STATE);
    bool   includes_p(STATE, Object* val);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
      virtual void show(STATE, Object* self, int level);
    };
  };
}

#endif
