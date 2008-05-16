#ifndef RBX_TYPE_INFO_HPP
#define RBX_TYPE_INFO_HPP

#include "prelude.hpp"
#include "object.hpp"

#include <map>
#include <stdexcept>

#include "gc.hpp"

namespace rubinius {
  class VM;
  class Class;

  /* TypeInfo contains varies operations that are registered by types */
  class TypeInfo {
  public:

    typedef std::map<native_int, long> Slots;

    VM* state;
    object_type type;
    Slots slots;

    static void init(STATE);
    TypeInfo(object_type type);
    virtual ~TypeInfo();
    virtual void cleanup(OBJECT obj);
    virtual void mark(OBJECT obj, ObjectMark& mark);
    virtual void set_field(STATE, OBJECT target, size_t index, OBJECT val);
    virtual OBJECT get_field(STATE, OBJECT target, size_t index);
  };

#define BASIC_TYPEINFO(super) \
  Info(object_type type) : super(type) { } \
  virtual void set_field(STATE, OBJECT target, size_t index, OBJECT val); \
  virtual OBJECT get_field(STATE, OBJECT target, size_t index); \
  virtual void mark(OBJECT obj, ObjectMark& mark);

}

#endif
