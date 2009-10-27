#include "gc/object_mark.hpp"
#include "gc/gc.hpp"
#include "object_utils.hpp"
#include "objectmemory.hpp"

#include "vm.hpp"

namespace rubinius {
  VM* ObjectMark::state() {
    return gc->state();
  }

  Object* ObjectMark::call(Object* obj) {
    if(!obj->reference_p()) return NULL;
#ifdef RBX_DEBUG
      if(unlikely(obj->zone() == UnspecifiedZone)) {
        std::cout << "USZ!\n";
        char* bad = (char*)0;
        if(*bad) exit(11);
      }
#endif
    return gc->saw_object(obj);
  }

  void ObjectMark::set(Object* target, Object** pos, Object* val) {
    *pos = val;
    if(val->reference_p()) {
      gc->object_memory_->write_barrier(target, val);
    }
  }

  void ObjectMark::just_set(Object* target, Object* val) {
    if(val->reference_p()) {
      gc->object_memory_->write_barrier(target, val);
    }
  }
}
