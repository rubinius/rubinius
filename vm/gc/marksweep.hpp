#ifndef RBX_MARKSWEEP_H
#define RBX_MARKSWEEP_H

#include "gc/gc.hpp"
#include "gc/root.hpp"
#include "object_position.hpp"

#include <list>

#include "call_frame_list.hpp"

#ifdef USE_DLMALLOC
#include "util/dlmalloc_cpp.hpp"
#endif


#define MS_COLLECTION_BYTES 10485760

namespace rubinius {

  /* Forwards */
  class Object;
  class ObjectMemory;
  class CallFrame;

  class MarkSweepGC : public GarbageCollector {
  public:
    typedef std::list<Object*> MarkStack;

  private:
      MarkStack mark_stack_;

#ifdef USE_DLMALLOC
      DLMalloc malloc_;
#endif

  public:
    /* Data members */
    std::list<Object*> entries;
    size_t allocated_bytes;
    size_t allocated_objects;
    int    next_collection_bytes;
    bool   free_entries;
    int    times_collected;
    int    last_freed;

    /* Prototypes */

    MarkSweepGC(ObjectMemory *om);
    virtual ~MarkSweepGC();
    void   free_objects();
    Object* allocate(size_t bytes, bool *collect_now);
    Object* copy_object(Object* obj);
    void   sweep_objects();
    void   free_object(Object* obj, bool fast = false);
    virtual Object* saw_object(Object* obj);
    void   collect(Roots &roots, CallFrameLocationList& call_frame);
    void after_marked();

    void profile();

    ObjectPosition validate_object(Object* obj);
  };
};

#endif
