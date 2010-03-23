#include "builtin/weakref.hpp"
#include "builtin/class.hpp"

#include "object_utils.hpp"
#include "gc/gc.hpp"

namespace rubinius {
  void WeakRef::init(STATE) {
    GO(cls_weakref).set(state->new_class("WeakRef", G(object)));
     G(cls_weakref)->set_object_type(state, WeakRefType);
  }

  WeakRef* WeakRef::create(STATE, Object* obj) {
    WeakRef* ref = state->new_object<WeakRef>(G(cls_weakref));
    ref->set_object(state, obj);

    return ref;
  }

  void WeakRef::Info::mark(Object* obj, ObjectMark& mark) {
    WeakRef* ref = as<WeakRef>(obj);
    if(ref->alive_p()) {
      mark.gc->add_weak_ref(obj);
    }
  }
}
