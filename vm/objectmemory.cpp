#include <cstdlib>
#include <iostream>

#include "objectmemory.hpp"
#include "gc_marksweep.hpp"

namespace rubinius {


  /* ObjectMemory methods */
  ObjectMemory::ObjectMemory(size_t young_bytes)
               :young(this, young_bytes), mature(this) {

    remember_set = new ObjectArray(0);

    collect_young_now = false;
    collect_mature_now = false;
    large_object_threshold = 2700;
    young.lifetime = 6;
    last_object_id = 0;
  }

  ObjectMemory::~ObjectMemory() {
    delete remember_set;
  }

  void ObjectMemory::set_young_lifetime(size_t age) {
    young.lifetime = age;
  }

  void ObjectMemory::debug_marksweep(bool val) {
    if(val) {
      mature.free_entries = false;
    } else {
      mature.free_entries = true;
    }
  }

  bool ObjectMemory::valid_object_p(OBJECT obj) {
    if(obj->young_object_p()) {
      return young.current->contains_p(obj);
    } else if(obj->mature_object_p()) {
      return true;
    } else {
      return false;
    }
  }

  /* Garbage collection */

  OBJECT ObjectMemory::promote_object(OBJECT obj) {
    OBJECT copy = mature.copy_object(obj);
    copy->zone = MatureObjectZone;
    return copy;
  }

  void ObjectMemory::collect_young(ObjectArray &roots) {
    young.collect(roots);
  }

  void ObjectMemory::collect_mature(ObjectArray &roots) {
    mature.collect(roots);
    young.clear_marks();
  }
};

void* XMALLOC(size_t bytes) {
  return malloc(bytes);
}

void XFREE(void* ptr) {
  free(ptr);
}

void* XREALLOC(void* ptr, size_t bytes) {
  return realloc(ptr, bytes);
}

void* XCALLOC(size_t items, size_t bytes) {
  return calloc(items, bytes);
}

