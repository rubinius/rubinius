#include "configuration.hpp"
#include "object_memory.hpp"
#include "object_utils.hpp"

#include "builtin/class.hpp"
#include "builtin/weakref.hpp"

#include "memory/gc.hpp"

namespace rubinius {
  void WeakRef::bootstrap(STATE) {
    GO(cls_weakref).set(Class::bootstrap_class(state, G(basicobject), WeakRefType));
  }

  WeakRef* WeakRef::create(STATE, Object* obj) {
    WeakRef* ref = state->memory()->new_object<WeakRef>(state, G(cls_weakref));
    ref->set_object(state, obj);

    return ref;
  }

  void WeakRef::set_object(ObjectMemory* om, Object* obj) {
    object_ = obj;
    om->write_barrier(this, obj);
  }

  Object* WeakRef::set_object(STATE, Object* obj) {
    object_ = obj;
    state->memory()->write_barrier(this, obj);
    return obj;
  }

  void WeakRef::Info::mark(Object* obj, ObjectMark& mark) {
    WeakRef* ref = as<WeakRef>(obj);
    if(ref->alive_p()) {
      mark.gc->add_weak_ref(obj);
    }
  }
}
