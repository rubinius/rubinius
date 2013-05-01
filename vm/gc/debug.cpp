#include "prelude.hpp"
#include "gc/gc.hpp"
#include "gc/debug.hpp"
#include "objectmemory.hpp"

#include <iostream>
#include <stdexcept>

namespace rubinius {
  HeapDebug::HeapDebug(ObjectMemory *om) :
    GarbageCollector(om), seen_objects(0) {

  }

  void HeapDebug::walk(Roots &roots) {
    Root* root = static_cast<Root*>(roots.head());
    while(root) {
      Object* tmp = root->get();
      if(tmp->reference_p()) {
        saw_object(tmp);
      }

      root = static_cast<Root*>(root->next());
    }
  }

  Object* HeapDebug::saw_object(Object* obj) {
    if(!obj->reference_p()) return NULL;

    if(seen[obj]) return NULL;

    seen_objects++;

    seen[obj] = 1;

    /*
    if(obj->young_object_p()) {
      if(!object_memory_->young.current->contains_p(obj)) {
        throw std::runtime_error("Invalid young object detected.");
      }
    }
    */

    scan_object(obj);

    return NULL;
  }
}
