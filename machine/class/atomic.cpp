#include "memory.hpp"

#include "class/atomic.hpp"
#include "class/class.hpp"

namespace rubinius {
  void AtomicReference::bootstrap(STATE) {
    GO(atomic_ref).set(state->memory()->new_class<Class, AtomicReference>(
          state, G(rubinius), "AtomicReference"));
  }

  AtomicReference* AtomicReference::allocate(STATE) {
    return state->memory()->new_object<AtomicReference>(state, G(atomic_ref));
  }

  AtomicReference* AtomicReference::create(STATE, Object* obj) {
    AtomicReference* ref = AtomicReference::allocate(state);
    ref->set(state, obj);
    return ref;
  }

  void AtomicReference::Info::mark(STATE, Object* obj, std::function<void (STATE, Object**)> f) {
    AtomicReference* ref = force_as<AtomicReference>(obj);

    Object* value = ref->value();

    f(state, &value);

    ref->value(value);
  }
}
