#include "gc/gc.hpp"
#include "gc/marksweep.hpp"
#include "objectmemory.hpp"

#include "object_utils.hpp"

#include "builtin/tuple.hpp"

#include "instruments/stats.hpp"

#include <iostream>

namespace rubinius {

  MarkSweepGC::MarkSweepGC(ObjectMemory *om)
              :GarbageCollector(om) {
    allocated_objects = 0;
    allocated_bytes = 0;
    next_collection_bytes = MS_COLLECTION_BYTES;
    free_entries = true;
  }

  MarkSweepGC::~MarkSweepGC() { }

  void MarkSweepGC::free_objects() {
    std::list<Object*>::iterator i;

    for(i = entries.begin(); i != entries.end(); i++) {
      free_object(*i, true);
    }
  }

  Object* MarkSweepGC::allocate(size_t bytes, bool *collect_now) {
    Object* obj;

#ifdef RBX_GC_STATS
    stats::GCStats::get()->mature_bytes_allocated += bytes;
    stats::GCStats::get()->allocate_mature.start();
#endif

#ifdef USE_DLMALLOC
    obj = reinterpret_cast<Object*>(malloc_.allocate(bytes));
#else
    obj = reinterpret_cast<Object*>(malloc(bytes));
#endif

    entries.push_back(obj);

    allocated_objects++;
    allocated_bytes += bytes;

    next_collection_bytes -= bytes;
    if(next_collection_bytes < 0) {
      *collect_now = true;
      next_collection_bytes = MS_COLLECTION_BYTES;
    }

    obj->init_header(MatureObjectZone, InvalidType);

#ifdef RBX_GC_STATS
    stats::GCStats::get()->allocate_mature.stop();
#endif

    return obj;
  }

  void MarkSweepGC::free_object(Object* obj, bool fast) {
    if(!fast) {
      delete_object(obj);
    }

    allocated_objects--;
    allocated_bytes -= obj->size_in_bytes(object_memory_->state);

#ifdef USE_DLMALLOC
    malloc_.release(reinterpret_cast<void*>(obj));
#else
    free(reinterpret_cast<void*>(obj));
#endif
  }

  Object* MarkSweepGC::copy_object(Object* orig) {
    bool collect;
    Object* obj = allocate(orig->size_in_bytes(object_memory_->state), &collect);

    obj->initialize_copy(orig, 0);
    obj->copy_body(object_memory_->state, orig);

    return obj;
  }

  Object* MarkSweepGC::saw_object(Object* obj) {
#ifdef RBX_GC_STATS
    stats::GCStats::get()->objects_seen++;
#endif
    if(obj->marked_p()) return NULL;
    obj->mark();

    // Add the object to the mark stack, to be scanned later.
    mark_stack_.push_back(obj);
    return NULL;
  }

  void MarkSweepGC::collect(Roots &roots, CallFrameLocationList& call_frames) {
    Object* tmp;

    Root* root = static_cast<Root*>(roots.head());
    while(root) {
      tmp = root->get();
      if(tmp->reference_p()) {
        saw_object(tmp);
      }

      root = static_cast<Root*>(root->next());
    }

    // Walk all the call frames
    for(CallFrameLocationList::const_iterator i = call_frames.begin();
        i != call_frames.end();
        i++) {
      CallFrame** loc = *i;
      walk_call_frame(*loc);
    }

    while(!mark_stack_.empty()) {
      tmp = mark_stack_.back();
      mark_stack_.pop_back();
      scan_object(tmp);
    }

    after_marked();
  }

  void MarkSweepGC::after_marked() {
    // Cleanup all weakrefs seen
    clean_weakrefs();

    // Sweep up the garbage
    sweep_objects();
  }

  void MarkSweepGC::sweep_objects() {
    std::list<Object*>::iterator i;

    for(i = entries.begin(); i != entries.end();) {
      Object* obj = *i;
      if(obj->marked_p()) {
        i++;
      } else {
        free_object(obj);
        i = entries.erase(i);
      }
    }
  }

  ObjectPosition MarkSweepGC::validate_object(Object* obj) {
    std::list<Object*>::iterator i;

    for(i = entries.begin(); i != entries.end(); i++) {
      if(*i == obj) return cMatureObject;
    }

    return cUnknown;
  }
}
