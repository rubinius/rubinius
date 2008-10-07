#include <cstdlib>
#include <iostream>

#include "objectmemory.hpp"
#include "gc_marksweep.hpp"
#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/tuple.hpp"

namespace rubinius {

  /* ObjectMemory methods */
  ObjectMemory::ObjectMemory(STATE, size_t young_bytes)
               :state(state), young(this, young_bytes), mature(this) {

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
    static int collect_times = 0;
    young.collect(roots);
    collect_times++;
  }

  void ObjectMemory::collect_mature(Roots &roots) {
    mature.collect(roots);
    young.clear_marks();
  }

  void ObjectMemory::add_type_info(TypeInfo* ti) {
    type_info[ti->type] = ti;
  }

  /* Store an object into the remember set. Called when we've calculated
   * externally that the object in question needs to be remembered */
  void ObjectMemory::remember_object(OBJECT target) {
    assert(target->zone == MatureObjectZone);
    /* If it's already remembered, ignore this request */
    if(target->Remember) return;
    target->Remember = 1;
    remember_set->push_back(target);
  }

  void ObjectMemory::unremember_object(OBJECT target) {
    for(ObjectArray::iterator oi = remember_set->begin();
        oi != remember_set->end();
        oi++) {
      if(*oi == target) {
        *oi = NULL;
        target->Remember = 0;
      }
    }
  }

  // DEPRECATED
  void ObjectMemory::store_object(OBJECT target, size_t index, OBJECT val) {
    ((Tuple*)target)->field[index] = val;
    write_barrier(target, val);
  }

  void ObjectMemory::set_class(OBJECT target, OBJECT obj) {
    target->klass(state, (Class*)obj);
    if(obj->reference_p()) {
      write_barrier(target, obj);
    }
  }

  OBJECT ObjectMemory::allocate_object(size_t fields) {
    OBJECT obj;

    if(fields > large_object_threshold) {
      obj = mature.allocate(fields, &collect_mature_now);
    } else {
      obj = young.allocate(fields, &collect_young_now);
      if(obj == NULL) {
        obj = mature.allocate(fields, &collect_mature_now);
      }
    }

    obj->clear_fields();
    return obj;
  }

  OBJECT ObjectMemory::new_object(Class* cls, size_t fields) {
    OBJECT obj;

    obj = allocate_object(fields);
    set_class(obj, cls);

    obj->obj_type = (object_type)cls->instance_type()->to_native();
    obj->RequiresCleanup = type_info[obj->obj_type]->instances_need_cleanup;

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

    OBJECT obj = new_object(cls, fields);

    obj->init_bytes();

    return obj;
  }

  TypeInfo* ObjectMemory::find_type_info(OBJECT obj) {
    return type_info[obj->obj_type];
  }

  ObjectPosition ObjectMemory::validate_object(OBJECT obj) {
    ObjectPosition pos;

    pos = young.validate_object(obj);
    if(pos != cUnknown) return pos;

    return mature.validate_object(obj);
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

