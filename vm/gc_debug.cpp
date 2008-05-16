#include "prelude.hpp"
#include "gc.hpp"
#include "gc_debug.hpp"
#include "objectmemory.hpp"

#include <iostream>
#include <stdexcept>

namespace rubinius {
  HeapDebug::HeapDebug(ObjectMemory *om) : 
    GarbageCollector(om), seen_objects(0) {

  }

  void HeapDebug::walk(Roots &roots) {
    OBJECT tmp;

    Roots::iterator i;
    for(i = roots.begin(); i != roots.end(); i++) {
      tmp = (*i)->get();
      if(tmp->reference_p()) {
        saw_object(tmp);
      }
    }
  }

  OBJECT HeapDebug::saw_object(OBJECT obj) {
    if(!obj->reference_p()) return NULL;

    if(seen[obj]) return NULL;
    
    seen_objects++;

    seen[obj] = 1;

    if(obj->young_object_p()) {
      if(!object_memory->young.current->contains_p(obj)) {
        throw std::runtime_error("Invalid young object detected.");
      }
    }

    scan_object(obj);

    return NULL;
  }
}
