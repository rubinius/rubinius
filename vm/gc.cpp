/* The GC superclass methods, used by both GCs. */

#include "gc.hpp"
#include "objectmemory.hpp"

#include "builtin/class.hpp"
#include "builtin/tuple.hpp"

namespace rubinius {

  ObjectMark::ObjectMark(GarbageCollector* gc) : gc(gc) { }

  OBJECT ObjectMark::call(OBJECT obj) {
    if(obj->reference_p()) {
      assert(obj->zone != UnspecifiedZone);
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
                   :object_memory(om), weak_refs(NULL) { }

  /* Understands how to read the inside of an object and find all references
   * located within. It copies the objects pointed to, but does not follow into
   * those further (ie, not recursive) */
  void GarbageCollector::scan_object(OBJECT obj) {
    OBJECT slot;

    // If this object's refs are weak, then add it to the weak_refs
    // vector and don't look at it otherwise.
    if(obj->RefsAreWeak) {
      if(!weak_refs) {
        weak_refs = new ObjectArray(0);
      }

      weak_refs->push_back(obj);
      return;
    }

    if(obj->klass() && obj->klass()->reference_p()) {
      slot = saw_object(obj->klass());
      if(slot) object_memory->set_class(obj, slot);
    }

    if(obj->ivars() && obj->ivars()->reference_p()) {
      slot = saw_object(obj->ivars());
      if(slot) obj->ivars(object_memory->state, slot);
    }

    TypeInfo* ti = object_memory->type_info[obj->obj_type];
    if(ti) {
      ObjectMark mark(this);
      ti->mark(obj, mark);
    } else if(obj->stores_references_p()) {
      // HACK copied from Tuple;
      OBJECT tmp;
      Tuple* tup = static_cast<Tuple*>(obj);

      for(size_t i = 0; i < tup->field_count; i++) {
        tmp = tup->field[i];
        if(tmp->reference_p()) {
          tmp = saw_object(tmp);
          if(tmp) {
            tup->field[i] = tmp;
            object_memory->write_barrier(tup, tmp);
          }
        }
      }
    }
  }

  void GarbageCollector::delete_object(OBJECT obj) {
    TypeInfo *ti = object_memory->find_type_info(obj);
    if(ti) ti->cleanup(obj);
  }
}
