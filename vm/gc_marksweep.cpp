#include "gc.hpp"
#include "gc_marksweep.hpp"
#include "objectmemory.hpp"
#include "builtin/tuple.hpp"

#include <iostream>

namespace rubinius {

  MarkSweepGC::Entry::Entry(Header* h, size_t b, size_t f) {
    bytes = b;
    fields = f;
    marked = FALSE;
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
      free_object(*i);
    }
  }

  OBJECT MarkSweepGC::allocate(size_t fields, bool *collect_now) {
    size_t bytes;
    OBJECT obj;

    bytes = sizeof(Header) + SIZE_IN_BYTES_FIELDS(fields);

    // std::cout << "ms: " << bytes << ", fields: " << fields << "\n";

    Header *header = (Header*)calloc(1, bytes);
    Entry *entry = new Entry(header, bytes, fields);
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

    obj->field_count = fields;
    obj->zone = MatureObjectZone;

    return obj;
  }

  void MarkSweepGC::free_object(Entry *entry) {

    delete_object(entry->header->to_object());

    allocated_objects--;
    allocated_bytes -= entry->bytes;

    free(entry->header);
  }

  OBJECT MarkSweepGC::copy_object(OBJECT orig) {
    bool collect;
    OBJECT obj = allocate(orig->field_count, &collect);

    obj->initialize_copy(orig, 0);
    obj->copy_body(orig);

    return obj;
  }

  MarkSweepGC::Entry *MarkSweepGC::find_entry(OBJECT obj) {
    Header *header = (Header*)((uintptr_t)obj - sizeof(Header));
    return header->entry;
  }

  OBJECT MarkSweepGC::saw_object(OBJECT obj) {
    if(obj->young_object_p()) {
      if(obj->marked_p()) return NULL;

      obj->mark();
    } else {
      Entry *entry = find_entry(obj);
      if(entry->marked_p()) return NULL;

      entry->mark();
    }

    /* Recurse down, scanning each object as we see it. */
    scan_object(obj);
    return NULL;
  }

  void MarkSweepGC::collect(Roots &roots) {
    OBJECT tmp;

    Root* root = static_cast<Root*>(roots.head());
    while(root) {
      tmp = root->get();
      if(tmp->reference_p()) {
        saw_object(tmp);
      }

      root = static_cast<Root*>(root->next());
    }

    // Cleanup all weakrefs seen
    clean_weakrefs();

    // Sweep up the garbage
    sweep_objects();
  }

  void MarkSweepGC::sweep_objects() {
    std::list<Entry*>::iterator i;

    for(i = entries.begin(); i != entries.end();) {
      if((*i)->unmarked_p()) {
        free_object(*i);
        if(free_entries) delete *i;
        i = entries.erase(i);
      } else {
        (*i)->clear();
        i++;
      }
    }
  }

  ObjectPosition MarkSweepGC::validate_object(OBJECT obj) {
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
      for(size_t ti = 0; ti < tup->field_count; ti++) {
        OBJECT obj = tup->at(ti);
        if(obj->young_object_p()) {
          if(!obj->marked_p()) {
            tup->field[ti] = Qnil;
          }
        } else {
          Entry *entry = find_entry(obj);
          if(!entry->marked_p()) {
            tup->field[ti] = Qnil;
          }
        }
      }
    }
  }
}
