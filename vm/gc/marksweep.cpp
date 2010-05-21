#include "gc/gc.hpp"
#include "gc/marksweep.hpp"
#include "objectmemory.hpp"

#include "object_utils.hpp"

#include "builtin/tuple.hpp"
#include "builtin/string.hpp"
#include "builtin/class.hpp"
#include "builtin/symbol.hpp"

#include "instruments/stats.hpp"

#include <iostream>
#include <algorithm>

namespace rubinius {

  MarkSweepGC::MarkSweepGC(ObjectMemory *om)
    : GarbageCollector(om)
    , allocated_bytes(0)
    , allocated_objects(0)
    , next_collection_bytes(MS_COLLECTION_BYTES)
    , free_entries(true)
    , times_collected(0)
    , last_freed(0)
  {}

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

    // If the allocation failed, we return a NULL pointer
    if(unlikely(!obj)) return NULL;

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

    last_freed++;

    allocated_objects--;
    allocated_bytes -= obj->size_in_bytes(object_memory_->state);
    obj->set_zone(UnspecifiedZone);

#ifdef USE_DLMALLOC
    malloc_.release(reinterpret_cast<void*>(obj));
#else
    free(reinterpret_cast<void*>(obj));
#endif
  }

  Object* MarkSweepGC::copy_object(Object* orig) {
    bool collect;
    Object* obj = allocate(orig->size_in_bytes(object_memory_->state), &collect);

    obj->initialize_full_state(object_memory_->state, orig, 0);

    return obj;
  }

  Object* MarkSweepGC::saw_object(Object* obj) {
#ifdef RBX_GC_STATS
    stats::GCStats::get()->objects_seen++;
#endif
    if(obj->marked_p(object_memory_->mark())) return NULL;
    obj->mark(object_memory_->mark());

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
        i++;
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

  void MarkSweepGC::profile() {

    std::map<Class*, PerClass> stats;

    for(std::list<Object*>::iterator i = entries.begin();
        i != entries.end();
        i++) {
      Object* obj = *i;
      Class* cls = obj->class_object(object_memory_->state);

      std::map<Class*,PerClass>::iterator j = stats.find(cls);
      if(j == stats.end()) {
        PerClass pc;
        pc.objects++;
        pc.bytes += obj->size_in_bytes(object_memory_->state);

        stats[cls] = pc;
      } else {
        j->second.objects++;
        j->second.bytes += obj->size_in_bytes(object_memory_->state);
      }
    }

    std::cout << stats.size() << " classes:\n";

    for(std::map<Class*,PerClass>::iterator i = stats.begin();
        i != stats.end();
        i++) {
      std::cout << i->first->name()->c_str(object_memory_->state) << "\n"
                << "  objects: " << i->second.objects << "\n"
                << "    bytes: " << i->second.bytes << "\n";
    }

    /*
    int count = 0;

    for(std::list<Object*>::reverse_iterator i = entries.rbegin();
        i != entries.rend();
        i++) {
      Object* obj = *i;
      if(ByteArray* ba = try_as<ByteArray>(obj)) {
        ba->show(object_memory_->state);
        if(++count == 10) break;
      }
    }
    */

    /*
    std::list<Object*> sorted = entries;
    sorted.sort(sort_by_size);

    std::list<Object*>::iterator i;

    std::cout << "Top 30:\n";

    int count = 0;

    for(i = sorted.begin(); i != sorted.end();) {
      Object* obj = *i;

      size_t sz = obj->size_in_bytes(object_memory_->state);

      std::cout << obj->to_s(object_memory_->state, true)->c_str() << " bytes=" << sz << "\n";
      if(++count == 30) break;

      i++;
    }
    */
  }

  ObjectPosition MarkSweepGC::validate_object(Object* obj) {
    std::list<Object*>::iterator i;

    for(i = entries.begin(); i != entries.end(); i++) {
      if(*i == obj) return cMatureObject;
    }

    return cUnknown;
  }
}
