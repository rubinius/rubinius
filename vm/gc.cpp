/* The GC superclass methods, used by both GCs. */

#include "gc.hpp"
#include "objectmemory.hpp"

#include "builtin/class.hpp"

namespace rubinius {

  ObjectMark::ObjectMark(GarbageCollector* gc) : gc(gc) { }

  OBJECT ObjectMark::call(OBJECT obj) {
    if(obj->reference_p()) {
      return gc->saw_object(obj);
    }

    return NULL;
  }

  void ObjectMark::set(OBJECT target, OBJECT* pos, OBJECT val) {
    *pos = val;
    if(val->reference_p()) {
      gc->object_memory->write_barrier(target, val);
    }
  }

  void ObjectMark::just_set(OBJECT target, OBJECT val) {
    if(val->reference_p()) {
      gc->object_memory->write_barrier(target, val);
    }
  }

  GarbageCollector::GarbageCollector(ObjectMemory *om)
                   :object_memory(om) { }

  /* Understands how to read the inside of an object and find all references
   * located within. It copies the objects pointed to, but does not follow into
   * those further (ie, not recursive) */
  void GarbageCollector::scan_object(OBJECT obj) {
    OBJECT slot;

    if(obj->klass && obj->klass->reference_p()) {
      slot = saw_object(obj->klass);
      if(slot) object_memory->set_class(obj, slot);
    }

    TypeInfo* ti = object_memory->type_info[obj->obj_type];
    if(ti) {
      ObjectMark mark(this);
      ti->mark(obj, mark);
    } else if(obj->stores_references_p()) {
      for(size_t i = 0; i < obj->field_count; i++) {
        slot = obj->field[i];
        if(slot->reference_p()) {
          slot = saw_object(slot);
          if(slot) object_memory->store_object(obj, i, slot);
        }
      }
    }
  }

  void GarbageCollector::delete_object(OBJECT obj) {
    TypeInfo *ti = object_memory->find_type_info(obj);
    if(ti) ti->cleanup(obj);
  }
}
