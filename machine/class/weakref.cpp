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
    ref->set_object(state, obj);

    return ref;
  }

  void WeakRef::set_object(Memory* om, Object* obj) {
    object(obj);
    om->write_barrier(this, obj);
  }

  Object* WeakRef::set_object(STATE, Object* obj) {
    object(obj);
    state->memory()->write_barrier(this, obj);
    return obj;
  }

  void WeakRef::Info::mark(Object* obj, memory::ObjectMark& mark) {
    WeakRef* ref = as<WeakRef>(obj);
    if(ref->alive_p()) {
      mark.gc->add_weak_ref(obj);
    }
  }
}
