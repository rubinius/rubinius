
#include "builtin/class.hpp"
#include "builtin/atomic.hpp"
#include "ontology.hpp"

namespace rubinius {
  void AtomicReference::init(STATE) {
    GO(atomic_ref).set(ontology::new_class(state,
          "AtomicReference", G(object), G(rubinius)));

    G(atomic_ref)->set_object_type(state, AtomicReferenceType);
    G(atomic_ref)->name(state, state->symbol("Rubinius::AtomicReference"));
  }

  AtomicReference* AtomicReference::allocate(STATE) {
    return state->new_object<AtomicReference>(G(atomic_ref));
  }

  Object* AtomicReference::compare_and_set(STATE, Object* old, Object* new_) {
    Object** pp = &value_;
    return atomic::compare_and_swap((void**)pp, old, new_) ? cTrue : cFalse;
  }
}
