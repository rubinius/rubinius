#include "builtin/class.hpp"
#include "builtin/weakref.hpp"
#include "configuration.hpp"
#include "gc/gc.hpp"
#include "object_utils.hpp"
#include "ontology.hpp"

namespace rubinius {

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
