#ifndef RBX_BUILTIN_ATOMIC_HPP
#define RBX_BUILTIN_ATOMIC_HPP

#include "object_utils.hpp"

#include "class/object.hpp"

#include "util/atomic.hpp"

namespace rubinius {

  class AtomicReference : public Object {
  public:
    const static object_type type = AtomicReferenceType;

    attr_accessor(value, Object);

    static void bootstrap(STATE);
    static void initialize(STATE, AtomicReference* ref) {
      ref->value(nil<Object>());
    }

    static AtomicReference* allocate(STATE);
    static AtomicReference* create(STATE, Object* val);

    // Rubinius.primitive+ :atomic_get
    Object* get(STATE) {
      atomic::memory_barrier();
      return value();
    }

    // Rubinius.primitive+ :atomic_set
    Object* set(STATE, Object* val) {
      value(state, val);
      atomic::memory_barrier();
      return val;
    }

    // Rubinius.primitive+ :atomic_compare_and_set
    Object* compare_and_set(STATE, Object* old, Object* new_value);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
}

#endif
