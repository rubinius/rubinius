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
    Object* shared_; // slot

  public:
    /* accessors */

    attr_accessor(total, Fixnum);
    attr_accessor(tuple, Tuple);
    attr_accessor(start, Fixnum);
    attr_accessor(shared, Object);

    /* interface */

    size_t size();
    static void init(STATE);
    static Array* create(STATE, size_t size);
    static Array* from_tuple(STATE, Tuple* tup);
    static Array* to_ary(STATE, Object* obj, CallFrame* frame);

    void   setup(STATE, size_t size);

    // Ruby.primitive :array_allocate
    static Array* allocate(STATE, Object* self);

    // Ruby.primitive :array_aref
    Object* aref(STATE, Fixnum* idx);

    // Ruby.primitive :array_aset
    Object* aset(STATE, Fixnum* idx, Object* val);

    // Ruby.primitive :array_concat
    Array* concat(STATE, Array* other);

    // Ruby.primitive :array_pack
    String* pack(STATE, String* directives, CallFrame* calling_environment);

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

  class ArrayIterator : public Object {
  public:
    const static object_type type = ArrayIteratorType;

  private:
    Tuple*  tuple_;     // slot

    bool        reverse_;
    native_int  first_;
    native_int  last_;
    native_int  step_;
    native_int  left_;
    native_int  right_;
    native_int  index_;

  public:
    attr_accessor(tuple, Tuple);

    static void init(STATE);
    // Ruby.primitive :array_iterator_allocate
    static ArrayIterator* allocate(STATE, Array* array, Fixnum* step, Object* reverse);

    // Ruby.primitive :array_iterator_next
    Object* next(STATE);
    // Ruby.primitive :array_iterator_rnext
    Object* rnext(STATE);
    // Ruby.primitive :array_iterator_item
    Object* item(STATE);
    // Ruby.primitive :array_iterator_at
    Object* at(STATE, Fixnum* relative);
    // Ruby.primitive :array_iterator_index
    Fixnum* index(STATE);
    // Ruby.primitive :array_iterator_bounds
    Object* bounds(STATE, Fixnum* left, Fixnum* right);

    void set_index();

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
}

#endif
