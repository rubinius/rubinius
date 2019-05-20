#ifndef RBX_MARKSWEEP_H
#define RBX_MARKSWEEP_H

#include "memory/gc.hpp"
#include "memory/mark_stack.hpp"
#include "memory/root.hpp"

#include "object_position.hpp"

#include "diagnostics.hpp"

#include <stdint.h>
#include <list>

namespace rubinius {

  /* Forwards */
  class Object;
  class Memory;
  struct CallFrame;

namespace memory {
  class MarkSweepGC : public GarbageCollector {
  public:
  private:
      MarkStack mark_stack_;

      diagnostics::MarkSweep* diagnostic_;

  public:
    /* Data members */
    std::list<Object*> entries;
    int    collection_threshold;
    int    next_collection_bytes;

    /* Prototypes */

    MarkSweepGC(Memory *om, Configuration& config);
    virtual ~MarkSweepGC();
    void   free_objects();
    Object* allocate(size_t bytes, bool& collect_now);
    Object* move_object(Object* orig, size_t bytes, bool& collect_now);
    Object* copy_object(Object* obj, bool& collect_now);
    void   sweep_objects();
    void   free_object(Object* obj, bool fast = false);
    virtual Object* saw_object(void* parent, Object* child);
    virtual void scanned_object(Object* obj) {}
    void after_marked();

    void profile(STATE);

    ObjectPosition validate_object(Object* obj);

    diagnostics::MarkSweep* diagnostic() {
      return diagnostic_;
    }
  };
}
}

#endif
