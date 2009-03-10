#ifndef RBX_MARKSWEEP_H
#define RBX_MARKSWEEP_H

#include "gc.hpp"
#include "gc_root.hpp"
#include "object_position.hpp"

#include <list>

#include "call_frame_list.hpp"
#include "util/dlmalloc_cpp.hpp"

#define MS_COLLECTION_BYTES 10485760

namespace rubinius {

  /* Forwards */
  class Object;
  class ObjectMemory;
  class CallFrame;

  class MarkSweepGC : public GarbageCollector {
  public:
    typedef std::list<Object*> MarkStack;

    /* Utility classes */
    class Header;

    class Entry {
    public:
      /* Data members */
      int bytes;
      Header *header;

      /* Prototypes */
      Entry(Header*, size_t bytes);
    };

    class Header {
      public:
      Entry *entry;

      /* Inline methods */

      /* Returns the Object* that this is header for, using simple
       * pointer math. */
      Object* to_object() {
        return (Object*)((uintptr_t)this + sizeof(Header));
      }
    };

  private:
      MarkStack mark_stack_;
      DLMalloc malloc_;

  public:
    /* Data members */
    std::list<Entry*> entries;
    size_t allocated_bytes;
    size_t allocated_objects;
    int    next_collection_bytes;
    bool   free_entries;

    /* Prototypes */

    MarkSweepGC(ObjectMemory *om);
    virtual ~MarkSweepGC();
    void   free_objects();
    Object* allocate(size_t bytes, bool *collect_now);
    Object* copy_object(Object* obj);
    Entry *find_entry(Object* obj);
    void   sweep_objects();
    void   clean_weakrefs();
    void   free_object(Entry *entry, bool fast = false);
    virtual Object* saw_object(Object* obj);
    void   collect(Roots &roots, CallFrameLocationList& call_frame);

    ObjectPosition validate_object(Object* obj);
  };
};

#endif
