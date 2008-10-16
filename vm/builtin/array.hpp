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
    INTEGER total_; // slot
    Tuple* tuple_;  // slot
    INTEGER start_; // slot
    OBJECT shared_; // slot

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
    OBJECT aref(STATE, Fixnum* idx);

    // Ruby.primitive :array_aset
    OBJECT aset(STATE, Fixnum* idx, OBJECT val);

    OBJECT get(STATE, size_t idx);
    OBJECT set(STATE, size_t idx, OBJECT val);
    void   unshift(STATE, OBJECT val);
    OBJECT shift(STATE);
    OBJECT append(STATE, OBJECT val);
    bool   includes_p(STATE, OBJECT val);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
      virtual void show(STATE, OBJECT self, int level);
    };
  };
};

#endif
