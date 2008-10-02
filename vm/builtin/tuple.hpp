#ifndef RBX_BUILTIN_TUPLE_HPP
#define RBX_BUILTIN_TUPLE_HPP

#include "builtin/object.hpp"
#include "builtin/exception.hpp"
#include "type_info.hpp"

namespace rubinius {
  class Tuple : public Object {
  public:
    const static size_t fields = 0;
    const static object_type type = TupleType;

    /* Body access */
    OBJECT field[];

    static Tuple* create(STATE, size_t fields);
    static Tuple* from(STATE, size_t fields, ...);

    // Ruby.primitive :tuple_allocate
    static Tuple* allocate(STATE, FIXNUM fields);

    // Ruby.primitive :tuple_at
    OBJECT at_prim(STATE, FIXNUM pos);

    OBJECT put(STATE, size_t idx, OBJECT val);

    // Ruby.primitive :tuple_put
    OBJECT put_prim(STATE, FIXNUM idx, OBJECT val);

    // Ruby.primitive :tuple_fields
    OBJECT fields_prim(STATE);

    // Ruby.primitive :tuple_pattern
    static Tuple* pattern(STATE, FIXNUM size, OBJECT val);

    // Ruby.primitive :tuple_copy_from
    Tuple* copy_from(STATE, Tuple* other, FIXNUM start, FIXNUM dest);

    // Ruby.primitive :tuple_shifted
    Tuple* shifted(STATE, FIXNUM num);

    // Ruby.primitive :tuple_create_weakref
    static Tuple* create_weakref(STATE, OBJECT obj);

    void replace_with(STATE, Tuple* other, int start, int end);

  public: // Inline Functions
    OBJECT at(STATE, size_t index) {
      if(num_fields() <= index) {
        Exception::object_bounds_exceeded_error(state, this, index);
      }
      return field[index];
    }

  public: // Rubinius Type stuff
    class Info : public TypeInfo {
    public:
      Info(object_type type, bool cleanup = false) : TypeInfo(type, cleanup) { }
      virtual void mark(OBJECT t, ObjectMark& mark);
      virtual void show(STATE, OBJECT self, int level);
      virtual void show_simple(STATE, OBJECT self, int level);
    };
  };
};

#endif
