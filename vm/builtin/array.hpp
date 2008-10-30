#ifndef RBX_ARRAY_HPP
#define RBX_ARRAY_HPP

#include "builtin/object.hpp"
#include "builtin/fixnum.hpp"
#include "type_info.hpp"

namespace rubinius {
  class Tuple;

  class Array : public Object {
  public:
    const static size_t fields = 4;
    const static object_type type = ArrayType;

  private:
    Integer* total_; // slot
    Tuple* tuple_;  // slot
    Integer* start_; // slot
    Object* shared_; // slot

  public:
    /* accessors */

    attr_accessor(total, Integer);
    attr_accessor(tuple, Tuple);
    attr_accessor(start, Integer);
    attr_accessor(shared, Object);

    /* interface */

    size_t size();
    static void init(STATE);
    static Array* create(STATE, size_t size);
    static Array* from_tuple(STATE, Tuple* tup);
    void   setup(STATE, size_t size);

    // Ruby.primitive :array_aref
    Object* aref(STATE, Fixnum* idx);

    // Ruby.primitive :array_aset
    Object* aset(STATE, Fixnum* idx, Object* val);

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
