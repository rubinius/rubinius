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
      if(target->field_count <= index) {
        throw new ObjectBoundsExceeded(target, index);
      }

      target->field[index] = val;
      if(val->reference_p()) write_barrier(target, val);
    }

    void set_class(OBJECT target, OBJECT obj) {
      target->klass = (Class*)obj;
      if(obj->reference_p()) {
        write_barrier(target, obj);
      }
    }

    OBJECT allocate_object(size_t fields) {
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

      obj->klass = NULL;
      obj->init(loc, fields);
      obj->clear_fields();
      return obj;
    }

    OBJECT allocate_bytes(size_t bytes) {
      const size_t mag = sizeof(OBJECT);
      size_t fields;
      size_t needed = bytes + 1;
      if(needed <= mag) {
        fields =  1;
      } else {
        fields = (needed + (mag - (needed & mag - 1))) / mag;
      }

      OBJECT obj = allocate_object(fields);
      obj->init_bytes();

      return obj;
    }

    OBJECT allocate_mature(size_t fields, bool bytes = false) {
      OBJECT obj = mature.allocate(fields, &collect_mature_now);

      obj->klass = NULL;
      obj->init(MatureObjectZone, fields);

      if(bytes) {
        obj->init_bytes();
      } else {
        obj->clear_fields();
      }

      return obj;
    }

    OBJECT new_object(Class* cls, size_t fields) {
      OBJECT obj = create_object(cls, fields);

      obj->clear_fields();

      return obj;
    }

    OBJECT new_object_bytes(Class* cls, size_t bytes) {
      const size_t mag = sizeof(OBJECT);
      size_t fields;
      size_t needed = bytes + 1;
      if(needed <= mag) {
        fields =  1;
      } else {
        fields = (needed + (mag - (needed & mag - 1))) / mag;
      }

      OBJECT obj = create_object(cls, fields);

      obj->init_bytes();

      return obj;
    }

    OBJECT new_object_mature(Class* cls, size_t fields, bool bytes = false) {
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

    OBJECT create_object(Class* cls, size_t fields) {
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

      obj->obj_type = (object_type)cls->instance_type->to_nint();
      obj->CanStoreIvars = (cls->has_ivars == Qtrue);
      obj->RequiresCleanup = (cls->needs_cleanup == Qtrue);

      return obj;
    }

    TypeInfo* find_type_info(OBJECT obj) {
      return type_info[obj->obj_type];
    }

    /* Prototypes */
    ObjectMemory(size_t young_bytes);
    ~ObjectMemory();
    void set_young_lifetime(size_t age);
    void collect_young(Roots &roots);
    void collect_mature(Roots &roots);
    OBJECT promote_object(OBJECT obj);
    bool valid_object_p(OBJECT obj);
    void debug_marksweep(bool val);
    void add_type_info(TypeInfo* ti);

  };

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
