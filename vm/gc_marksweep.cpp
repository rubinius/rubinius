#include "gc.hpp"
#include "gc_marksweep.hpp"
#include "objectmemory.hpp"

#include "vm/object_utils.hpp"

#include "builtin/tuple.hpp"

#include "instruments/stats.hpp"

#include <iostream>

namespace rubinius {

  MarkSweepGC::Entry::Entry(Header* h, size_t b) {
    bytes = b;
    header = h;
  }

  MarkSweepGC::MarkSweepGC(ObjectMemory *om)
              :GarbageCollector(om) {
    allocated_objects = 0;
    allocated_bytes = 0;
    next_collection_bytes = MS_COLLECTION_BYTES;
    free_entries = true;
  }

  MarkSweepGC::~MarkSweepGC() {
    std::list<Entry*>::iterator i;

    for(i = entries.begin(); i != entries.end(); i++) {
      delete *i;
    }
  }

  void MarkSweepGC::free_objects() {
    std::list<Entry*>::iterator i;

    for(i = entries.begin(); i != entries.end(); i++) {
      free_object(*i, true);
    }
  }

  Object* MarkSweepGC::allocate(size_t obj_bytes, bool *collect_now) {
    size_t bytes;
    Object* obj;

#ifdef RBX_GC_STATS
    // duplicating the calulation so it is included in the time below
    stats::GCStats::get()->mature_bytes_allocated += sizeof(Header) + obj_bytes;
    stats::GCStats::get()->allocate_mature.start();
#endif

    bytes = sizeof(Header) + obj_bytes;

    Header *header = (Header*)malloc_.allocate(bytes);
    Entry *entry = new Entry(header, bytes);
    header->entry = entry;

    entries.push_back(entry);

    allocated_objects++;
    allocated_bytes += bytes;

    next_collection_bytes -= bytes;
    if(next_collection_bytes < 0) {
      *collect_now = true;
      next_collection_bytes = MS_COLLECTION_BYTES;
    }

    obj = header->to_object();

    obj->init_header(MatureObjectZone, obj_bytes);

#ifdef RBX_GC_STATS
    stats::GCStats::get()->allocate_mature.stop();
#endif

    return obj;
  }

  void MarkSweepGC::free_object(Entry *entry, bool fast) {
    if(!fast) {
      delete_object(entry->header->to_object());
    }

    allocated_objects--;
    allocated_bytes -= entry->bytes;

    // A debugging tag to see if we try to use a free'd object
    entry->header->to_object()->IsMeta = 1;

    malloc_.release(entry->header);
  }

  Object* MarkSweepGC::copy_object(Object* orig) {
    bool collect;
    Object* obj = allocate(orig->size_in_bytes(), &collect);

    obj->initialize_copy(orig, 0);
    obj->copy_body(orig);

    return obj;
  }

  MarkSweepGC::Entry *MarkSweepGC::find_entry(Object* obj) {
    Header *header = (Header*)((uintptr_t)obj - sizeof(Header));
    return header->entry;
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

    // Cleanup all weakrefs seen
    clean_weakrefs();

    // Sweep up the garbage
    sweep_objects();
  }

  void MarkSweepGC::sweep_objects() {
    std::list<Entry*>::iterator i;

    for(i = entries.begin(); i != entries.end();) {
      Entry* ent = *i;
      Object* obj = ent->header->to_object();
      if(obj->marked_p()) {
        i++;
      } else {
        free_object(ent);
        if(free_entries) delete ent;
        i = entries.erase(i);
      }
    }
  }

  ObjectPosition MarkSweepGC::validate_object(Object* obj) {
    std::list<Entry*>::iterator i;

    for(i = entries.begin(); i != entries.end(); i++) {
      Entry* entry = *i;
      if(entry->header->to_object() == obj) {
        return cMatureObject;
      }
    }

    return cUnknown;
  }

  // HACK todo test this!
  void MarkSweepGC::clean_weakrefs() {
    if(!weak_refs) return;

    for(ObjectArray::iterator i = weak_refs->begin();
        i != weak_refs->end();
        i++) {
      // ATM, only a Tuple can be marked weak.
      Tuple* tup = as<Tuple>(*i);
      for(size_t ti = 0; ti < tup->num_fields(); ti++) {
        Object* obj = tup->at(object_memory->state, ti);

        if(!obj->reference_p()) continue;

        if(!obj->marked_p()) {
          tup->field[ti] = Qnil;
        }
      }
    }

    delete weak_refs;
    weak_refs = NULL;
  }
}
