#ifndef RBX_OBJECTMEMORY_H
#define RBX_OBJECTMEMORY_H

#include "gc_marksweep.hpp"
#include "gc_baker.hpp"
#include "prelude.hpp"
#include "type_info.hpp"

#include "object_position.hpp"

#include "builtin/contexts.hpp"

namespace rubinius {

  class Object;

  /* ObjectMemory is the primary API that the rest of the VM uses to interact
   * with actions such as allocating objects, storing data in objects, and
   * perform garbage collection.
   *
   * It is current split between 2 generations, the BakerGC, which handles
   * the young objects, and the MarkSweepGC, which handles the mature.
   *
   * Basic tasks:
   *
   * Allocate an object of a given class and number of fields.
   *   If the object is large, it's put to start in the mature space,
   *   otherwise in the young space.
   *
   * Detection of memory condition requiring collection of both generations
   *   independently.
   *
   */

  class ObjectMemory {
  public:

    static const int cContextHeapSize = 1024 * 1024;

    bool collect_young_now;
    bool collect_mature_now;

    STATE;
    ObjectArray *remember_set;
    BakerGC young;
    MarkSweepGC mature;
    Heap contexts;
    size_t last_object_id;
    TypeInfo* type_info[(int)LastObjectType];

    /* Config variables */
    size_t large_object_threshold;

    ObjectMemory(STATE, size_t young_bytes);
    ~ObjectMemory();

    void remember_object(Object* target);
    void unremember_object(Object* target);

    void store_object(Object* target, size_t index, Object* val);
    void set_class(Object* target, Object* obj);
    Object* allocate_object(size_t bytes);

    Object* new_object_typed(Class* cls, size_t bytes, object_type type);

    template <class T>
      T* new_object_bytes(Class* cls, size_t bytes) {
        // Only works because sizeof(Object*) will alwasy be a power of 2
        const int rounding_value = sizeof(Object*) - 1;

        // round up
        bytes = (sizeof(T) + bytes + rounding_value) & ~rounding_value;
        T* obj = reinterpret_cast<T*>(new_object_typed(cls, bytes, T::type));

        obj->init_bytes();

        return obj;
      }

    template <class T>
      T* new_object_variable(Class* cls, size_t fields) {
        return reinterpret_cast<T*>(new_object_typed(cls, sizeof(T) + (fields * sizeof(Object*)), T::type));
      }

    TypeInfo* find_type_info(Object* obj);
    void set_young_lifetime(size_t age);
    void collect_young(Roots &roots);
    void collect_mature(Roots &roots);
    Object* promote_object(Object* obj);
    bool valid_object_p(Object* obj);
    void debug_marksweep(bool val);
    void add_type_info(TypeInfo* ti);

    ObjectPosition validate_object(Object* obj);

    void clear_context_marks();

    // Indicate whether +ctx+ is located on the context stack
    bool context_on_stack_p(MethodContext* ctx) {
      return (Object*)ctx->klass() == Qnil;
    }

    // Indicates if +ctx+ is a referenced context
    bool context_referenced_p(MethodContext* ctx) {
      return contexts.contains_p(ctx) && ctx < contexts.scan;
    }

    // Allocate a MethodContext object containing +stack_slots+
    // stack positions.
    MethodContext* allocate_context(size_t stack_slots) {
      size_t full_size = sizeof(MethodContext) + (stack_slots * sizeof(Object*));

      // Off the end
      if(!contexts.enough_space_p(full_size)) return NULL;

      MethodContext* ctx = static_cast<MethodContext*>(
          contexts.allocate(full_size));

      // Masquerade as being in the Young zone so the write barrier
      // stays happy.
      ctx->init_header(YoungObjectZone, full_size);

      ctx->full_size = full_size;
      return ctx;
    }

    // Return the number of bytes used by +ctx+ to the context
    // storage area.
    bool deallocate_context(MethodContext* ctx) {
      // If ctx is less than the scan, we ignore deallocating it.
      if(ctx < contexts.start || ctx < contexts.scan) return false;
      contexts.put_back(ctx->full_size);
      assert(contexts.current >= contexts.start);
      return true;
    }

    // Mark that +ctx+ has been referenced and should not be
    // deallocated as normal.
    void reference_context(MethodContext* ctx) {
      if(!context_on_stack_p(ctx)) return;

      address barrier = ((address)(((uintptr_t)ctx) + ctx->full_size));

      if(contexts.scan < barrier) {
        contexts.set_scan(barrier);
      }
    }

    // Make sure that all existing contexts are not automatically
    // deallocated
    void clamp_contexts() {
      void* barrier = reinterpret_cast<void*>((uintptr_t)contexts.current + 1);
      contexts.set_scan(barrier);
    }

    void write_barrier(Object* target, Object* val) {
      if(target->Remember) return;
      if(!REFERENCE_P(val)) return;
      if(target->zone != MatureObjectZone) return;
      if(val->zone != YoungObjectZone) return;

      remember_object(target);
    }
  };

#define FREE(obj) free(obj)
#define ALLOC_N(type, size) ((type*)calloc(size, sizeof(type)))
#define ALLOC(t) (t*)XMALLOC(sizeof(t))
#define REALLOC_N(v,t,n) (v)=(t*)realloc((void*)(v), sizeof(t)*n)

};


extern "C" {
  void* XMALLOC(size_t bytes);
  void  XFREE(void* ptr);
  void* XREALLOC(void* ptr, size_t bytes);
  void* XCALLOC(size_t items, size_t bytes);
}

#endif
