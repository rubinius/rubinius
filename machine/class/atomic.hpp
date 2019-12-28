#ifndef RBX_BUILTIN_ATOMIC_HPP
#define RBX_BUILTIN_ATOMIC_HPP

#include "object_utils.hpp"

#include "class/object.hpp"

#include <atomic>

namespace rubinius {

  class AtomicReference : public Object {
  public:
    const static object_type type = AtomicReferenceType;

    std::atomic<Object*> _value_;

    static void bootstrap(STATE);
    static void initialize(STATE, AtomicReference* ref) {
      ref->value(nil<Object>());
    }

    static AtomicReference* allocate(STATE);
    static AtomicReference* create(STATE, Object* val);

    Object* value() {
      return _value_.load(std::memory_order_seq_cst);
    }

    void value(Object* value) {
      _value_.store(value, std::memory_order_seq_cst);
    }

    // Rubinius.primitive+ :atomic_get
    Object* get(STATE) {
      return value();
    }

    // Rubinius.primitive+ :atomic_set
    Object* set(STATE, Object* val) {
      value(val);
      return val;
    }

    // Rubinius.primitive+ :atomic_compare_and_set
    Object* compare_and_set(STATE, Object* old, Object* new_value) {
      /* The simple comparison of two values that are technically nil would
       * fail if either of the values carries a nil tag, so we preserve the
       * nil value in the object in the event that the CAS fails by setting
       * the passed "old" value to be the same as the object's value.
       */
      if(value()->nil_p() && old->nil_p()) {
        old = value();
      }

      return RBOOL(_value_.compare_exchange_strong(old, new_value, std::memory_order_release));
    }

    class Info : public TypeInfo {
      public:
        Info(object_type type)
          : TypeInfo(type)
        {
        }

        virtual void mark(STATE, Object* obj, std::function<void (STATE, Object**)> f);
        virtual void auto_mark(STATE, Object* obj, std::function<void (STATE, Object**)> f) { }
        virtual void set_field(STATE, Object* target, size_t index, Object* val) { }
        virtual Object* get_field(STATE, Object* target, size_t index) { return cNil; }
        virtual void populate_slot_locations() { }
    };
  };
}

#endif
