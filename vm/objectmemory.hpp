#ifndef RBX_OBJECTMEMORY_H
#define RBX_OBJECTMEMORY_H

#include "object.hpp"
#include "gc_marksweep.hpp"
#include "gc_baker.hpp"
#include "objects.hpp"

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

  /* Config variables */
  size_t large_object_threshold;

  /* Inline methods */

  void write_barrier(OBJECT target, OBJECT val) {
    if(!target->Remember && val->reference_p() &&
        target->zone == MatureObjectZone &&
        val->zone != MatureObjectZone) {

      target->Remember = 1;
      remember_set->push_back(target);
    }
  }

  void store_object(OBJECT target, size_t index, OBJECT val) {
    target->field[index] = val;
    if(val->reference_p()) write_barrier(target, val);
  }

  void set_class(OBJECT target, OBJECT obj) {
    target->klass = obj;
    if(obj->reference_p()) {
      write_barrier(target, obj);

      Class *cls = (Class*)obj;
      target->obj_type = (object_type)cls->object_type->n2i();
      target->CanStoreIvars = (cls->has_ivars == Qtrue);
      target->RequiresCleanup = (cls->needs_cleanup == Qtrue);
    }
  }

  OBJECT new_object(OBJECT cls, size_t fields) {
    OBJECT obj = create_object(cls, fields);

    obj->clear_fields();

    return obj;
  }

  OBJECT new_object_bytes(OBJECT cls, size_t fields) {
    OBJECT obj = create_object(cls, fields);

    obj->init_bytes();

    return obj;
  }

  OBJECT new_object_mature(OBJECT cls, size_t fields, bool bytes = false) {
    OBJECT obj = mature.allocate(fields, &collect_mature_now);

    obj->init(MatureObjectZone, fields);

    if(bytes) {
      obj->init_bytes();
    } else {
      obj->clear_fields();
    }

    set_class(obj, cls);

    return obj;
  }

  OBJECT create_object(OBJECT cls, size_t fields) {
    OBJECT obj;
    gc_zone loc;

    if(fields > large_object_threshold) {
      obj = mature.allocate(fields, &collect_mature_now);
      loc = MatureObjectZone;
    } else {
      if((obj = young.allocate(fields, &collect_young_now))) {
        loc = YoungObjectZone;
      } else {
        obj = mature.allocate(fields, &collect_mature_now);
        loc = MatureObjectZone;
      }
    }

    obj->init(loc, fields);
    set_class(obj, cls);

    return obj;
  }

  /* Prototypes */
  ObjectMemory(size_t young_bytes);
  void set_young_lifetime(size_t age);
  void collect_young(ObjectArray &roots);
  void collect_mature(ObjectArray &roots);
  OBJECT promote_object(OBJECT obj);
  bool valid_object_p(OBJECT obj);
};

#define SET(obj, field, val) ({ \
    typeof(obj) _o = (obj); OBJECT  _v = (val); _o->field = (typeof(_o->field))_v; state->om->write_barrier(_o, _v); })

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
