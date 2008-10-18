#ifndef RBX_MARKSWEEP_H
#define RBX_MARKSWEEP_H

#include "gc.hpp"
#include "gc_root.hpp"
#include "object_position.hpp"

#include <list>

#define MS_COLLECTION_BYTES 10485760

namespace rubinius {

  /* Forwards */
  class Object;
  class ObjectMemory;


  class MarkSweepGC : public GarbageCollector {
  public:

    /* Utility classes */
    class Header;

    class Entry {
    public:
      /* Data members */
      int bytes;
      int fields;
      bool marked;
      Header *header;

      /* Inline methods */
      void mark() {
        marked = true;
      }

      void clear() {
        marked = false;
      }

      bool marked_p() {
        return marked;
      }

      bool unmarked_p() {
        return !marked;
      }

      /* Prototypes */

      Entry(Header*, size_t bytes, size_t fields);
    };

    class Header {
      public:
      Entry *entry;

      /* Inline methods */

      /* Returns the OBJECT that this is header for, using simple
       * pointer math. */
      OBJECT to_object() {
        return (OBJECT)((uintptr_t)this + sizeof(Header));
      }
    };

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
    OBJECT allocate(size_t fields, bool *collect_now);
    OBJECT copy_object(OBJECT obj);
    Entry *find_entry(OBJECT obj);
    void   sweep_objects();
    void   clean_weakrefs();
    void   free_object(Entry *entry, bool fast = false);
    virtual OBJECT saw_object(OBJECT obj);
    void   collect(Roots &roots);

    ObjectPosition validate_object(OBJECT obj);
  };
};

#endif
