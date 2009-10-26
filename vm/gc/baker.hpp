#ifndef RBX_VM_GC_BAKER_HPP
#define RBX_VM_GC_BAKER_HPP

#include <iostream>
#include <cstring>

#include "gc/heap.hpp"
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
  class YoungCollectStats;

  class BakerGC : public GarbageCollector {
    Heap eden;
    Heap heap_a;
    Heap heap_b;
    Heap *current;
    Heap *next;

  public:
    size_t lifetime;
    size_t total_objects;

    /* Inline methods */
    Object* raw_allocate(size_t bytes, bool* limit_hit) {
      Object* obj;

#ifdef RBX_GC_STATS
      stats::GCStats::get()->young_bytes_allocated += bytes;
      stats::GCStats::get()->allocate_young.start();
#endif

      if(!eden.enough_space_p(bytes)) {
#ifdef RBX_GC_STATS
       stats::GCStats::get()->allocate_young.stop();
#endif
        return NULL;
      } else {
        total_objects++;
        obj = (Object*)eden.allocate(bytes);

        if(eden.over_limit_p(obj)) {
          *limit_hit = true;
        }
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

      if(!eden.enough_space_p(bytes)) {
#ifdef RBX_GC_STATS
       stats::GCStats::get()->allocate_young.stop();
#endif
        return NULL;
      } else {
        total_objects++;
        obj = (Object*)eden.allocate(bytes);
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
    int copy_spills_;

    int promoted_objects_;
    ObjectArray* promoted_;
    ObjectArray::iterator promoted_insert, promoted_current;

    // Assume ObjectArray is a vector!
    void promoted_push(Object* obj) {
      promoted_objects_++;

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

    void reset_promoted() {
      promoted_objects_ = 0;
      promoted_ = new ObjectArray(0);
      promoted_current = promoted_insert = promoted_->begin();
    }

    void clear_promotion() {
      assert(promoted_->size() == 0);
      if(promoted_) delete promoted_;
      promoted_ = 0;
    }

  public:
    size_t bytes_used() {
      return eden.used();
    }

  public:
    /* Prototypes */
    BakerGC(ObjectMemory *om, size_t size);
    virtual ~BakerGC();
    void free_objects();
    virtual Object* saw_object(Object* obj);
    void    copy_unscanned();
    bool    fully_scanned_p();
    void    collect(GCData& data, YoungCollectStats* stats = 0);
    void    clear_marks();
    void    find_lost_souls();

    ObjectPosition validate_object(Object* obj);
    bool in_current_p(Object* obj);

  private:
    /* Private for inlining */
    Object*  next_object(Object* obj);
  };
};

#endif
