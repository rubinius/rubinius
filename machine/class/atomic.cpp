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

  Object* AtomicReference::compare_and_set(STATE, Object* old, Object* new_) {
    Object** pp = &_value_;

    if(atomic::compare_and_swap((void**)pp, old, new_)) {
      state->memory()->write_barrier(this, new_);
      return cTrue;
    } else {
      return cFalse;
    }
  }
}
