#include "builtin/weakref.hpp"
#include "builtin/class.hpp"

namespace rubinius {
  void WeakRef::init(STATE) {
    GO(cls_weakref).set(state->new_class("WeakRef", G(object)));
     G(cls_weakref)->set_object_type(state, WeakRefType);
  }

  WeakRef* WeakRef::create(STATE, Object* obj) {
    WeakRef* ref = state->new_object<WeakRef>(G(cls_weakref));
    ref->set_object(obj);

    return ref;
  }
}
