#ifndef RBX_BUILTIN_ATOMIC_HPP
#define RBX_BUILTIN_ATOMIC_HPP

#include "vm.hpp"
#include "builtin/object.hpp"

namespace rubinius {

  class AtomicReference : public Object {
  public:
    const static object_type type = AtomicReferenceType;

  private:
    Object* value_; // slot

  public:
    attr_accessor(value, Object);

  public:
    static void init(STATE);

    static AtomicReference* allocate(STATE);

    // Rubinius.primitive :atomic_compare_and_set
    Object* compare_and_set(STATE, Object* old, Object* new_value);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
}

#endif
