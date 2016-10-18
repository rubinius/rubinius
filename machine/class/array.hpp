#ifndef RBX_ARRAY_HPP
#define RBX_ARRAY_HPP

#include "object_utils.hpp"

#include "class/fixnum.hpp"
#include "class/object.hpp"
#include "class/tuple.hpp"

namespace rubinius {
  class Tuple;
  class Fixnum;

  class Array : public Object {
  public:
    const static object_type type = ArrayType;

    attr_accessor(total, Fixnum);
    attr_accessor(tuple, Tuple);
    attr_accessor(start, Fixnum);

    native_int size();
    native_int offset();
    void set_size(native_int size);

    static void bootstrap(STATE);
    static void initialize(STATE, Array* array) {
      array->total(Fixnum::from(0));
      array->tuple(nil<Tuple>());
      array->start(Fixnum::from(0));
    }

    static Array* create(STATE, native_int size);
    static Array* from_tuple(STATE, Tuple* tup);
    static Array* to_ary(STATE, Object* obj);

    // Rubinius.primitive :array_allocate
    static Array* allocate(STATE, Object* self);

    // Rubinius.primitive :array_dup_as_array
    static Array* dup_as_array(STATE, Object* obj);

    // Rubinius.primitive :array_new_range
    Array* new_range(STATE, Fixnum* index, Fixnum* count);

    // Rubinius.primitive :array_new_reserved
    Array* new_reserved(STATE, Fixnum* count);

    // Rubinius.primitive+ :array_aref
    Object* aref(STATE, Fixnum* idx);

    // Rubinius.primitive :array_aset
    Object* aset(STATE, Fixnum* idx, Object* val);

    // Rubinius.primitive :array_concat
    Array* concat(STATE, Array* other);

    // Rubinius.primitive :array_pack
    String* pack(STATE, String* directives);

    Object* get(STATE, native_int idx);
    Object* set(STATE, native_int idx, Object* val);
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
