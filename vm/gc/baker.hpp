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

#include "util/thread.hpp"

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

    // No locking for now, but this will need to change
    // eventually.
    thread::NullLock lock_;

  public:
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
        lock_.lock();

        total_objects++;
        obj = (Object*)eden.allocate(bytes);

        lock_.unlock();

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
        lock_.lock();

        total_objects++;
        obj = (Object*)eden.allocate(bytes);

        lock_.unlock();
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

    bool autotune_;
    int tune_threshold_;
    size_t original_lifetime_;
    size_t lifetime_;

    int promoted_objects_;
    ObjectArray promoted_stack_;

    void promoted_push(Object* obj) {
      promoted_objects_++;
      promoted_stack_.push_back(obj);
    }

    void reset_promoted() {
      promoted_objects_ = 0;
    }

  public:
    size_t bytes_used() {
      return eden.used();
    }

    void set_lifetime(int val) {
      lifetime_ = original_lifetime_ = val;
    }

    void set_autotune(bool val = true) {
      autotune_ = val;
    }

    bool autotune() {
      return autotune_;
    }

  public:
    /* Prototypes */
    BakerGC(ObjectMemory *om, size_t size);
    virtual ~BakerGC();
    virtual Object* saw_object(Object* obj);
    void    copy_unscanned();
    bool    fully_scanned_p();
    void    collect(GCData& data, YoungCollectStats* stats = 0);
    void    clear_marks();
    void    find_lost_souls();
    void    check_finalize();
    void    handle_promotions();

    ObjectPosition validate_object(Object* obj);
    bool in_current_p(Object* obj);

  private:
    /* Private for inlining */
    Object*  next_object(Object* obj);
  };
};

#endif
