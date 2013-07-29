#include "gc/gc.hpp"
#include "gc/marksweep.hpp"
#include "objectmemory.hpp"

#include "object_utils.hpp"

#include "builtin/tuple.hpp"
#include "builtin/string.hpp"
#include "builtin/class.hpp"
#include "builtin/symbol.hpp"

#include "configuration.hpp"

#include <iostream>
#include <algorithm>

namespace rubinius {

  MarkSweepGC::MarkSweepGC(ObjectMemory *om, Configuration& config)
    : GarbageCollector(om)
    , allocated_bytes(0)
    , allocated_objects(0)
    , collection_threshold(config.gc_marksweep_threshold)
    , next_collection_bytes(collection_threshold)
    , free_entries(true)
    , times_collected(0)
    , last_freed(0)
  {}

  MarkSweepGC::~MarkSweepGC() { }

  void MarkSweepGC::free_objects() {
    std::list<Object*>::iterator i;

    for(i = entries.begin(); i != entries.end(); ++i) {
      free_object(*i, true);
    }
  }

  Object* MarkSweepGC::allocate(size_t bytes, bool *collect_now) {
    Object* obj = reinterpret_cast<Object*>(malloc(bytes));

    // If the allocation failed, we return a NULL pointer
    if(unlikely(!obj)) {
        return NULL;
    }

    entries.push_back(obj);

    allocated_objects++;
    allocated_bytes += bytes;

    next_collection_bytes -= bytes;
    if(next_collection_bytes < 0) {
      *collect_now = true;
      next_collection_bytes = collection_threshold;
    }

    obj->init_header(MatureObjectZone, InvalidType);

    return obj;
  }

  void MarkSweepGC::free_object(Object* obj, bool fast) {
    if(!fast) {
      delete_object(obj);

      last_freed++;

      allocated_objects--;
      allocated_bytes -= obj->size_in_bytes(object_memory_->state());
    }

    obj->set_zone(UnspecifiedZone);

    free(reinterpret_cast<void*>(obj));
  }

  Object* MarkSweepGC::move_object(Object* orig, size_t bytes,
                                   bool* collect_now)
  {
    Object* obj = allocate(bytes, collect_now);
    memcpy(obj, orig, bytes);

    obj->set_zone(MatureObjectZone);
    obj->set_age(0);

    orig->set_forward(obj);

    return obj;
  }

  Object* MarkSweepGC::copy_object(Object* orig) {
    bool collect;
    Object* obj = allocate(orig->size_in_bytes(object_memory_->state()), &collect);

    obj->initialize_full_state(object_memory_->state(), orig, 0);

    return obj;
  }

  Object* MarkSweepGC::saw_object(Object* obj) {
    if(obj->marked_p(object_memory_->mark())) return NULL;
    obj->mark(object_memory_, object_memory_->mark());

    // Add the object to the mark stack, to be scanned later.
    mark_stack_.push_back(obj);
    return NULL;
  }

  void MarkSweepGC::after_marked() {
    times_collected++;
    last_freed = 0;

    // Cleanup all weakrefs seen
    clean_weakrefs();

    // Sweep up the garbage
    sweep_objects();
  }

  void MarkSweepGC::sweep_objects() {
    std::list<Object*>::iterator i;

    for(i = entries.begin(); i != entries.end();) {
      Object* obj = *i;
      if(obj->marked_p(object_memory_->mark())) {
        ++i;
      } else {
        free_object(obj);
        i = entries.erase(i);
      }
    }
  }

  /*
  static bool sort_by_size(Object* a, Object* b) {
    STATE = rubinius::VM::current_state();
    size_t a_size = a->size_in_bytes(state);
    size_t b_size = b->size_in_bytes(state);

    return b_size < a_size;
  }
  */

  struct PerClass {
    int objects;
    int bytes;

    PerClass()
      : objects(0)
      , bytes(0)
    {}
  };

  void MarkSweepGC::profile(STATE) {

    std::map<Class*, PerClass> stats;

    for(std::list<Object*>::iterator i = entries.begin();
        i != entries.end();
        ++i) {
      Object* obj = *i;
      Class* cls = obj->class_object(state);

      std::map<Class*,PerClass>::iterator j = stats.find(cls);
      if(j == stats.end()) {
        PerClass pc;
        pc.objects++;
        pc.bytes += obj->size_in_bytes(object_memory_->state());

        stats[cls] = pc;
      } else {
        j->second.objects++;
        j->second.bytes += obj->size_in_bytes(object_memory_->state());
      }
    }

    std::cout << stats.size() << " classes:\n";

    for(std::map<Class*,PerClass>::iterator i = stats.begin();
        i != stats.end();
        ++i) {
      std::cout << i->first->debug_str(state) << "\n"
                << "  objects: " << i->second.objects << "\n"
                << "    bytes: " << i->second.bytes << "\n";
    }
  }

  ObjectPosition MarkSweepGC::validate_object(Object* obj) {
    std::list<Object*>::iterator i;

    for(i = entries.begin(); i != entries.end(); ++i) {
      if(*i == obj) return cMatureObject;
    }

    return cUnknown;
  }
}
