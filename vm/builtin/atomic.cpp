#include "builtin/atomic.hpp"
#include "builtin/class.hpp"
#include "ontology.hpp"

namespace rubinius {
  void AtomicReference::init(STATE) {
    GO(atomic_ref).set(ontology::new_class(state,
          "AtomicReference", G(object), G(rubinius)));

    G(atomic_ref)->set_object_type(state, AtomicReferenceType);
  }

  AtomicReference* AtomicReference::allocate(STATE) {
    return state->new_object<AtomicReference>(G(atomic_ref));
  }

  AtomicReference* AtomicReference::create(STATE, Object* obj) {
    AtomicReference* ref = AtomicReference::allocate(state);
    ref->set(state, obj);
    return ref;
  }

  Object* AtomicReference::compare_and_set(STATE, Object* old, Object* new_) {
    Object** pp = &value_;

    if(atomic::compare_and_swap((void**)pp, old, new_)) {
      this->write_barrier(state, new_);
      return cTrue;
    } else {
      return cFalse;
    }
  }
}
