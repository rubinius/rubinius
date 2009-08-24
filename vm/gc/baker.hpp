#ifndef RBX_VM_GC_BAKER_HPP
#define RBX_VM_GC_BAKER_HPP

#include <iostream>
#include <cstring>

#include "heap.hpp"
#include "gc/gc.hpp"
#include "gc/root.hpp"
#include "object_position.hpp"

#include "builtin/object.hpp"

#include "instruments/stats.hpp"

#include "call_frame_list.hpp"

#include "object_watch.hpp"

namespace rubinius {

  class ObjectMemory;
  class GCData;

  class BakerGC : public GarbageCollector {
  public:

    /* Fields */
    Heap heap_a;
    Heap heap_b;
    Heap *current;
    Heap *next;
    size_t lifetime;
    size_t total_objects;

    /* Inline methods */
    Object* raw_allocate(size_t bytes) {
      Object* obj;

#ifdef RBX_GC_STATS
      stats::GCStats::get()->young_bytes_allocated += bytes;
      stats::GCStats::get()->allocate_young.start();
#endif

      if(!current->enough_space_p(bytes)) {
#ifdef RBX_GC_STATS
       stats::GCStats::get()->allocate_young.stop();
#endif
        return NULL;
      } else {
        total_objects++;
        obj = (Object*)current->allocate(bytes);
      }

#ifdef ENABLE_OBJECT_WATCH
      if(watched_p(obj)) {
        std::cout << "detected " << obj << " during baker allocation.\n";
      }
#endif

#ifdef RBX_GC_STATS
      stats::GCStats::get()->allocate_young.stop();
#endif

      return obj;
    }

    Object* allocate(size_t bytes) {
      Object* obj;

#ifdef RBX_GC_STATS
      stats::GCStats::get()->young_bytes_allocated += bytes;
      stats::GCStats::get()->allocate_young.start();
#endif

      if(!current->enough_space_p(bytes)) {
#ifdef RBX_GC_STATS
       stats::GCStats::get()->allocate_young.stop();
#endif
        return NULL;
      } else {
        total_objects++;
        obj = (Object*)current->allocate(bytes);
      }

#ifdef ENABLE_OBJECT_WATCH
      if(watched_p(obj)) {
        std::cout << "detected " << obj << " during baker allocation.\n";
      }
#endif

      obj->init_header(YoungObjectZone, InvalidType);

#ifdef RBX_GC_STATS
      stats::GCStats::get()->allocate_young.stop();
#endif

      return obj;
    }

  private:
    ObjectArray* promoted_;
    ObjectArray::iterator promoted_insert, promoted_current;

    // Assume ObjectArray is a vector!
    void promoted_push(Object* obj) {
      if(promoted_insert == promoted_current) {
        size_t i = promoted_insert - promoted_->begin(),
               j = promoted_current - promoted_->begin();
        promoted_->push_back(obj);
        promoted_current = promoted_->begin() + j;
        promoted_insert = promoted_->begin() + i;
      } else {
        *promoted_insert++ = obj;
      }
    }

  public:
    /* Prototypes */
    BakerGC(ObjectMemory *om, size_t size);
    virtual ~BakerGC();
    void free_objects();
    virtual Object* saw_object(Object* obj);
    void    copy_unscanned();
    bool    fully_scanned_p();
    void    collect(GCData& data);
    void    clear_marks();
    void    find_lost_souls();

    ObjectPosition validate_object(Object* obj);

  private:
    /* Private for inlining */
    Object*  next_object(Object* obj);
  };
};

#endif
