#include "builtin/weakref.hpp"
#include "builtin/class.hpp"

#include "object_utils.hpp"
#include "gc/gc.hpp"

#include "ontology.hpp"
#include "configuration.hpp"

namespace rubinius {
  void WeakRef::init(STATE) {
    Class* super_class;
    if(!LANGUAGE_18_ENABLED(state)) {
      super_class = G(basicobject);
    } else {
      super_class = G(object);
    }
    GO(cls_weakref).set(ontology::new_class(state, "WeakRef", super_class));
     G(cls_weakref)->set_object_type(state, WeakRefType);
  }

  WeakRef* WeakRef::create(STATE, Object* obj) {
    WeakRef* ref = state->new_object<WeakRef>(G(cls_weakref));
    ref->set_object(state, obj);

    return ref;
  }

  void WeakRef::Info::mark(Object* obj, ObjectMark& mark) {
    WeakRef* ref = as<WeakRef>(obj);
    if(ref->reference_alive_p()) {
      mark.gc->add_weak_ref(obj);
    }
  }
}
