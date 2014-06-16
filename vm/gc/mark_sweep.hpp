#ifndef RBX_MARKSWEEP_H
#define RBX_MARKSWEEP_H

#include "gc/gc.hpp"
#include "gc/root.hpp"
#include "object_position.hpp"

#include <list>

namespace rubinius {

  /* Forwards */
  class Object;
  class ObjectMemory;
  struct CallFrame;

  class MarkSweepGC : public GarbageCollector {
  public:
    typedef std::list<Object*> MarkStack;

  private:
      MarkStack mark_stack_;

  public:
    /* Data members */
    std::list<Object*> entries;
    size_t allocated_bytes;
    size_t allocated_objects;
    int    collection_threshold;
    int    next_collection_bytes;
    bool   free_entries;
    int    times_collected;
    int    last_freed;

    /* Prototypes */

    MarkSweepGC(ObjectMemory *om, Configuration& config);
    virtual ~MarkSweepGC();
    void   free_objects();
    Object* allocate(size_t bytes, bool *collect_now);
    Object* move_object(Object* orig, size_t bytes, bool *collect_now);
    Object* copy_object(Object* obj);
    void   sweep_objects();
    void   free_object(Object* obj, bool fast = false);
    virtual Object* saw_object(Object* obj);
    virtual void scanned_object(Object* obj) {}
    virtual bool mature_gc_in_progress();
    void after_marked();

    void profile(STATE);

    ObjectPosition validate_object(Object* obj);
  };
};

#endif
