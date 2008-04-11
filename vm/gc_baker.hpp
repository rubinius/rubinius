#ifndef RBX_GC_BAKER_H
#define RBX_GC_BAKER_H

#include "gc.hpp"
#include <iostream>

namespace rubinius {
  typedef void *address;

  class ObjectMemory;

  class BakerGC : public GarbageCollector {

    public:

    class Heap {
      /* Fields */

      public:
      address start;
      address current;
      address last;
      address scan;

      size_t size;

      /* Inline methods */

      address allocate(size_t size) {
        address addr;
        addr = current;
        std::memset((void*)addr, 0, size);
        current = (address)((uintptr_t)current +  size);

        return addr;
      }

      address allocate_dirty(size_t size) {
        address addr;
        addr = current;
        current = (address)((uintptr_t)current +  size);

        return addr;
      }

      bool contains_p(address addr) {
        if(addr < start) return FALSE;
        if(addr >= last) return FALSE;
        return TRUE;
      }

      bool enough_space_p(size_t size) {
        if((uintptr_t)current + size > (uintptr_t)last) return FALSE;
        return TRUE;
      }

      bool fully_scanned_p() {
        return scan == current;
      }

      OBJECT next_unscanned() {
        OBJECT obj;
        if(fully_scanned_p()) return NULL;

        obj = (OBJECT)scan;
        scan = (address)((uintptr_t)scan + obj->size_in_bytes());
        return obj;
      }

      OBJECT first_object() {
        return (OBJECT)start;
      }

      /* Prototypes */
      Heap(size_t size);
      ~Heap();
      void reset();
      size_t remaining();
      size_t used();
      OBJECT copy_object(OBJECT);
    };

    /* Fields */
    Heap heap_a;
    Heap heap_b;
    Heap *current;
    Heap *next;
    size_t lifetime;

    /* Inline methods */
    OBJECT allocate(size_t fields, bool *collect_now) {
      size_t bytes = SIZE_IN_BYTES_FIELDS(fields);
      OBJECT obj;

      if(!current->enough_space_p(bytes)) {
        if(!next->enough_space_p(bytes)) {
          return NULL;
        } else {
          obj = (OBJECT)next->allocate(bytes);
        }

        *collect_now = true;
      } else {
        obj = (OBJECT)current->allocate(bytes);
      }

      return obj;
    }

    /* Prototypes */
    BakerGC(ObjectMemory *om, size_t size);
    virtual ~BakerGC();
    void free_objects();
    virtual OBJECT saw_object(OBJECT obj);
    void    copy_unscanned();
    void    collect(ObjectArray &roots);
    void    clear_marks();
    OBJECT  next_object(OBJECT obj);
    void    find_lost_souls();
  };
};

#endif
