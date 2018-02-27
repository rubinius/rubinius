#include "configuration.hpp"
#include "memory.hpp"
#include "object_utils.hpp"

#include "class/class.hpp"
#include "class/weakref.hpp"

#include "memory/gc.hpp"

namespace rubinius {
  void WeakRef::bootstrap(STATE) {
    GO(cls_weakref).set(Class::bootstrap_class(state, G(basicobject), WeakRefType));
  }

  WeakRef* WeakRef::create(STATE, Object* obj) {
    WeakRef* ref = state->memory()->new_object<WeakRef>(state, G(cls_weakref));

    ref->set_weakref(state);
    ref->object(obj);

    return ref;
  }

  void WeakRef::Info::mark_weakref(Object* obj, memory::ObjectMark& mark) {
    WeakRef* ref = as<WeakRef>(obj);

    if(ref->object()->reference_p()) {
      if(!ref->object()->marked_p(mark.vm()->memory()->mark())) {
        ref->object(cNil);
      }
    }
  }
}
