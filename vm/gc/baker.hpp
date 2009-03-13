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
    Object* allocate(size_t bytes, bool *collect_now) {
      Object* obj;

#ifdef RBX_GC_STATS
      stats::GCStats::get()->young_bytes_allocated += bytes;
      stats::GCStats::get()->allocate_young.start();
#endif

      if(!current->enough_space_p(bytes)) {
#if 0
        if(!next->enough_space_p(bytes)) {
          return NULL;
        } else {
          total_objects++;
          obj = (Object*)next->allocate(bytes);
        }
#endif
        *collect_now = true;

#ifdef RBX_GC_STATS
      stats::GCStats::get()->allocate_young.stop();
#endif

        return NULL;
      } else {
        total_objects++;
        obj = (Object*)current->allocate(bytes);
      }

      if(watched_p(obj)) {
        std::cout << "detected " << obj << " during baker allocation.\n";
      }

      obj->init_header(YoungObjectZone, bytes);

#ifdef RBX_GC_STATS
      stats::GCStats::get()->allocate_young.stop();
#endif

      return obj;
    }

  private:
    ObjectArray* promoted_;

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
    Object*  next_object(Object* obj);
    void    find_lost_souls();

    ObjectPosition validate_object(Object* obj);
  };
};

#endif
