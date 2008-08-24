#include <cstdlib>
#include <iostream>

#include "objectmemory.hpp"
#include "gc_marksweep.hpp"
#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"

namespace rubinius {


  /* ObjectMemory methods */
  ObjectMemory::ObjectMemory(size_t young_bytes)
               :young(this, young_bytes), mature(this) {

    remember_set = new ObjectArray(0);

    collect_young_now = false;
    collect_mature_now = false;
    large_object_threshold = 2700;
    young.lifetime = 6;
    last_object_id = 0;

    for(size_t i = 0; i < LastObjectType; i++) {
      type_info[i] = NULL;
    }
  }

  ObjectMemory::~ObjectMemory() {

    young.free_objects();
    mature.free_objects();

    delete remember_set;

    for(size_t i = 0; i < LastObjectType; i++) {
      if(type_info[i]) delete type_info[i];
    }
  }

  void ObjectMemory::set_young_lifetime(size_t age) {
    young.lifetime = age;
  }

  void ObjectMemory::debug_marksweep(bool val) {
    if(val) {
      mature.free_entries = false;
    } else {
      mature.free_entries = true;
    }
  }

  bool ObjectMemory::valid_object_p(OBJECT obj) {
    if(obj->young_object_p()) {
      return young.current->contains_p(obj);
    } else if(obj->mature_object_p()) {
      return true;
    } else {
      return false;
    }
  }

  /* Garbage collection */

  OBJECT ObjectMemory::promote_object(OBJECT obj) {
    OBJECT copy = mature.copy_object(obj);
    copy->zone = MatureObjectZone;
    return copy;
  }

  void ObjectMemory::collect_young(Roots &roots) {
    young.collect(roots);
  }

  void ObjectMemory::collect_mature(Roots &roots) {
    mature.collect(roots);
    young.clear_marks();
  }

  void ObjectMemory::add_type_info(TypeInfo* ti) {
    type_info[ti->type] = ti;
  }

  void ObjectMemory::write_barrier(OBJECT target, OBJECT val) {
    if(!target->Remember && val->reference_p() &&
       target->zone == MatureObjectZone &&
       val->zone != MatureObjectZone) {

      target->Remember = 1;
      remember_set->push_back(target);
    }
  }

  /* Store an object into the remember set. Called when we've calculated
   * externally that the object in question needs to be remembered */
  void ObjectMemory::remember_object(OBJECT target) {
    /* If it's already remembered, ignore this request */
    if(target->Remember) return;
    target->Remember = 1;
    remember_set->push_back(target);
  }

  void ObjectMemory::store_object(OBJECT target, size_t index, OBJECT val) {
    if(target->field_count <= index) {
      ObjectBoundsExceeded::raise(target, index);
    }

    target->field[index] = val;
    if(val->reference_p()) write_barrier(target, val);
  }

  void ObjectMemory::set_class(OBJECT target, OBJECT obj) {
    target->klass = (Class*)obj;
    if(obj->reference_p()) {
      write_barrier(target, obj);
    }
  }

  OBJECT ObjectMemory::allocate_object(size_t fields) {
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

  OBJECT ObjectMemory::allocate_bytes(size_t bytes) {
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

  OBJECT ObjectMemory::allocate_mature(size_t fields, bool bytes) {
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

  OBJECT ObjectMemory::new_object(Class* cls, size_t fields) {
    OBJECT obj = create_object(cls, fields);

    obj->clear_fields();

    return obj;
  }

  /* An Object field is the size of a pointer on any particular
   * platform. An Object that stores bytes must be aligned to an
   * integral number of fields. For example, if sizeof(OBJECT) == 4,
   * then an object that stores bytes must be 4, 8, 12, 16, ..., 4n
   * bytes in size. This corresponds to 1, 2, 3, 4, ..., n fields.
   */
  OBJECT ObjectMemory::new_object_bytes(Class* cls, size_t bytes) {
    const size_t mag = sizeof(OBJECT);
    size_t fields;

    if(bytes == 0) {
      fields = 1;
    } else {
      fields = bytes % mag == 0 ? bytes  : bytes + mag;
      fields /= mag;
    }

    OBJECT obj = create_object(cls, fields);

    obj->init_bytes();

    return obj;
  }

  OBJECT ObjectMemory::new_object_mature(Class* cls, size_t fields, bool bytes) {
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

  OBJECT ObjectMemory::create_object(Class* cls, size_t fields) {
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

    obj->obj_type = (object_type)cls->instance_type->to_native();
    obj->RequiresCleanup = (cls->needs_cleanup == Qtrue);

    return obj;
  }

  TypeInfo* ObjectMemory::find_type_info(OBJECT obj) {
    return type_info[obj->obj_type];
  }
};

void* XMALLOC(size_t bytes) {
  return malloc(bytes);
}

void XFREE(void* ptr) {
  free(ptr);
}

void* XREALLOC(void* ptr, size_t bytes) {
  return realloc(ptr, bytes);
}

void* XCALLOC(size_t items, size_t bytes) {
  return calloc(items, bytes);
}

