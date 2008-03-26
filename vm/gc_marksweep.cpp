#include "gc.h"
#include "gc_marksweep.hpp"
#include "objectmemory.hpp"

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
  }

  MarkSweepGC::~MarkSweepGC() {
    std::list<Entry*>::iterator i;

    for(i = entries.begin(); i != entries.end(); i++) {
      delete *i;
    }
  }

  OBJECT MarkSweepGC::allocate(size_t fields, bool *collect_now) {
    size_t bytes;
    OBJECT obj;

    bytes = sizeof(Header) + SIZE_IN_BYTES_FIELDS(fields);

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

    return obj;
  }

  void MarkSweepGC::free_object(Entry *entry) {
    allocated_objects--;
    allocated_bytes -= entry->bytes;

    free(entry->header);
  }

  OBJECT MarkSweepGC::copy_object(OBJECT orig) {
    bool collect;
    OBJECT obj = allocate(orig->field_count, &collect);

    obj->all_flags   = orig->all_flags;
    obj->field_count = orig->field_count;
    obj->klass       = orig->klass;
    obj->age         = 0;

    for(size_t i = 0; i < orig->field_count; i++) {
      obj->field[i] = orig->field[i];
    }

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

  void MarkSweepGC::collect(ObjectArray &roots) {
    OBJECT tmp;
    for(size_t i = 0; i < roots.size(); i++) {
      tmp = roots[i];
      if(tmp->reference_p()) {
        saw_object(tmp);
      }
    }

    sweep_objects();
  }

  void MarkSweepGC::sweep_objects() {
    std::list<Entry*>::iterator i;

    for(i = entries.begin(); i != entries.end(); i++) {
      if((*i)->unmarked_p()) {
        free_object(*i);
        entries.erase(i);
      } else {
        (*i)->clear();
      }
    }
  }
}
