#ifndef RBX_VM_GC_BAKER_HPP
#define RBX_VM_GC_BAKER_HPP

#include <iostream>
#include <cstring>

#include "vm/heap.hpp"
#include "vm/gc.hpp"
#include "vm/gc_root.hpp"
#include "vm/object_position.hpp"

#include "builtin/object.hpp"

#include "call_frame_list.hpp"

namespace rubinius {

  class ObjectMemory;

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
        return NULL;
      } else {
        total_objects++;
        obj = (Object*)current->allocate(bytes);
      }

      obj->init_header(YoungObjectZone, bytes);
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
    void    collect(Roots &roots, CallFrameList& call_frames);
    void    clear_marks();
    Object*  next_object(Object* obj);
    void    find_lost_souls();
    void    clean_weakrefs();

    ObjectPosition validate_object(Object* obj);
  };
};

#endif
