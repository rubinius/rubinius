#include "gc.hpp"
#include "objectmemory.hpp"

namespace rubinius {

  GarbageCollector::GarbageCollector(ObjectMemory *om)
                   :object_memory(om) { }

  /* Understands how to read the inside of an object and find all references
   * located within. It copies the objects pointed to, but does not follow into
   * those further (ie, not recursive) */
  void GarbageCollector::scan_object(OBJECT obj) {
    OBJECT slot;

    if(obj->klass->reference_p()) {
      slot = saw_object(obj->klass);
      if(slot) object_memory->set_class(obj, slot);
    }

    if(obj->stores_references_p()) {
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
    if(ti) ti->delete_object(obj);
  }
}
