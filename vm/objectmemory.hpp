#ifndef RBX_OBJECTMEMORY_H
#define RBX_OBJECTMEMORY_H

#include "object.hpp"
#include "gc_marksweep.hpp"
#include "gc_baker.hpp"
#include "objects.hpp"
#include "type_info.hpp"

namespace rubinius {

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

    bool collect_young_now;
    bool collect_mature_now;

    ObjectArray *remember_set;
    BakerGC young;
    MarkSweepGC mature;
    size_t last_object_id;
    TypeInfo* type_info[(int)LastObjectType];

    /* Config variables */
    size_t large_object_threshold;

    ObjectMemory(size_t young_bytes);
    ~ObjectMemory();

    void write_barrier(OBJECT target, OBJECT val);
    void remember_object(OBJECT target);

    void store_object(OBJECT target, size_t index, OBJECT val);
    void set_class(OBJECT target, OBJECT obj);
    OBJECT allocate_object(size_t fields);
    OBJECT allocate_bytes(size_t bytes);
    OBJECT allocate_mature(size_t fields, bool bytes = false);
    OBJECT new_object(Class* cls, size_t fields);
    OBJECT new_object_bytes(Class* cls, size_t bytes);
    OBJECT new_object_mature(Class* cls, size_t fields, bool bytes = false);
    OBJECT create_object(Class* cls, size_t fields);
    TypeInfo* find_type_info(OBJECT obj);
    void set_young_lifetime(size_t age);
    void collect_young(Roots &roots);
    void collect_mature(Roots &roots);
    OBJECT promote_object(OBJECT obj);
    bool valid_object_p(OBJECT obj);
    void debug_marksweep(bool val);
    void add_type_info(TypeInfo* ti);
  };

// Type-safe, write-barrier-enabled version of 'SET'
#define OSET(om, obj, field, val) ({ \
    typeof(obj) _o = (obj); OBJECT  _v = (val); \
    if(_v->nil_p()) { _o->field = (typeof(_o->field))Qnil; } else { \
    _o->field = as<typeof(*_o->field)>(_v); om->write_barrier(_o, _v); } })


#define SET(obj, field, val) OSET(state->om, obj, field, val)

#define FREE(obj) free(obj)
#define ALLOC_N(type, size) ((type*)calloc(size, sizeof(type)))

};


extern "C" {
  void* XMALLOC(size_t bytes);
  void  XFREE(void* ptr);
  void* XREALLOC(void* ptr, size_t bytes);
  void* XCALLOC(size_t items, size_t bytes);
}

#endif
