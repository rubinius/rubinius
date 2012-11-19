#include "gc/object_mark.hpp"
#include "gc/gc.hpp"
#include "object_utils.hpp"
#include "objectmemory.hpp"

#include "vm.hpp"

namespace rubinius {
  VM* ObjectMark::state() {
    return gc->state();
  }

  /**
   * Checks the reference to the target obj, and if it is valid, notifies the GC
   * that the object is still alive.
   *
   * /param obj The object reference to be marked as still reachable/alive.
   * /returns the location of the object after GC has processed the object,
   *          which may involve moving it.
   */
  Object* ObjectMark::call(Object* obj) {
    if(!obj->reference_p()) return NULL;
#ifdef RBX_DEBUG
      if(unlikely(obj->zone() == UnspecifiedZone)) {
        rubinius::bug("Unspecified zone for object");
      }
#endif
    return gc->saw_object(obj);
  }

  /**
   * Updates the reference pos in target with the new location of val, and
   * then runs the write-barrier.
   *
   * /param target The object containing the reference to val.
   * /param pos    A pointer to the location in target that references val.
   * /param val    The object being referenced.
   */
  void ObjectMark::set(Object* target, Object** pos, Object* val) {
    *pos = val;
    if(val->reference_p()) {
      gc->object_memory_->write_barrier(target, val);
    }
  }

  /**
   * Runs the write-barrier for the reference from target to val.
   *
   * /param target The object containing the reference to val.
   * /param val    The object being referenced.
   */
  void ObjectMark::just_set(Object* target, Object* val) {
    if(val->reference_p()) {
      gc->object_memory_->write_barrier(target, val);
    }
  }
}

