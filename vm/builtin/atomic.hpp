#ifndef RBX_BUILTIN_ATOMIC_HPP
#define RBX_BUILTIN_ATOMIC_HPP

#include "builtin/object.hpp"
#include "util/atomic.hpp"

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
    static AtomicReference* create(STATE, Object* val);

    // Rubinius.primitive+ :atomic_get
    Object* get(STATE) {
      atomic::memory_barrier();
      return value_;
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
