#include <cstdlib>
#include <iostream>

#include "vm.hpp"
#include "objectmemory.hpp"
#include "gc_marksweep.hpp"
#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/tuple.hpp"

namespace rubinius {

  /* ObjectMemory methods */
  ObjectMemory::ObjectMemory(STATE, size_t young_bytes):
      state(state),
      young(this, young_bytes),
      mature(this),
      contexts(cContextHeapSize) {

    remember_set = new ObjectArray(0);

    collect_young_now = false;
    collect_mature_now = false;
    large_object_threshold = 2700;
    young.lifetime = 6;
    last_object_id = 0;

    for(size_t i = 0; i < LastObjectType; i++) {
      type_info[i] = NULL;
    }

    // Push the scan pointer off the bottom so nothing is seend
    // as scaned
    contexts.set_scan((address)(((uintptr_t)contexts.current) - 1));
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

  bool ObjectMemory::valid_object_p(Object* obj) {
    if(obj->young_object_p()) {
      return young.current->contains_p(obj);
    } else if(obj->mature_object_p()) {
      return true;
    } else {
      return false;
    }
  }

  /* Garbage collection */

  Object* ObjectMemory::promote_object(Object* obj) {
    Object* copy = mature.copy_object(obj);
    copy->zone = MatureObjectZone;
    return copy;
  }

  void ObjectMemory::collect_young(Roots &roots) {
    static int collect_times = 0;
    young.collect(roots);
    collect_times++;

    contexts.reset();
  }

  void ObjectMemory::collect_mature(Roots &roots) {
    mature.collect(roots);
    young.clear_marks();
    clear_context_marks();
  }

  void ObjectMemory::add_type_info(TypeInfo* ti) {
    type_info[ti->type] = ti;
  }

  /* Store an object into the remember set. Called when we've calculated
   * externally that the object in question needs to be remembered */
  void ObjectMemory::remember_object(Object* target) {
    assert(target->zone == MatureObjectZone);
    /* If it's already remembered, ignore this request */
    if(target->Remember) return;
    target->Remember = 1;
    remember_set->push_back(target);
  }

  void ObjectMemory::unremember_object(Object* target) {
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
  void ObjectMemory::store_object(Object* target, size_t index, Object* val) {
    ((Tuple*)target)->field[index] = val;
    write_barrier(target, val);
  }

  void ObjectMemory::set_class(Object* target, Object* obj) {
    target->klass(state, (Class*)obj);
    if(obj->reference_p()) {
      write_barrier(target, obj);
    }
  }

  Object* ObjectMemory::allocate_object(size_t fields) {
    Object* obj;

    if(fields > large_object_threshold) {
      obj = mature.allocate(fields, &collect_mature_now);
      if(collect_mature_now) {
        state->interrupts.check = true;
      }
    } else {
      obj = young.allocate(fields, &collect_young_now);
      if(obj == NULL) {
        collect_young_now = true;
        state->interrupts.check = true;
        obj = mature.allocate(fields, &collect_mature_now);
      }
    }

    obj->clear_fields();
    return obj;
  }

  Object* ObjectMemory::new_object(Class* cls, size_t fields) {
    Object* obj;

    obj = allocate_object(fields);
    set_class(obj, cls);

    obj->obj_type = (object_type)cls->instance_type()->to_native();
    obj->RequiresCleanup = type_info[obj->obj_type]->instances_need_cleanup;

    return obj;
  }

  /* An Object field is the size of a pointer on any particular
   * platform. An Object that stores bytes must be aligned to an
   * integral number of fields. For example, if sizeof(Object*) == 4,
   * then an object that stores bytes must be 4, 8, 12, 16, ..., 4n
   * bytes in size. This corresponds to 1, 2, 3, 4, ..., n fields.
   */
  Object* ObjectMemory::new_object_bytes(Class* cls, size_t bytes) {
    const size_t mag = sizeof(Object*);
    size_t fields;

    if(bytes == 0) {
      fields = 1;
    } else {
      fields = bytes % mag == 0 ? bytes  : bytes + mag;
      fields /= mag;
    }

    Object* obj = new_object(cls, fields);

    obj->init_bytes();

    return obj;
  }

  TypeInfo* ObjectMemory::find_type_info(Object* obj) {
    return type_info[obj->obj_type];
  }

  ObjectPosition ObjectMemory::validate_object(Object* obj) {
    ObjectPosition pos;

    if(contexts.contains_p((address)obj)) return cContextStack;

    pos = young.validate_object(obj);
    if(pos != cUnknown) return pos;

    return mature.validate_object(obj);
  }

  void ObjectMemory::clear_context_marks() {
    Object* obj = contexts.first_object();
    while(obj < contexts.current) {
      obj->clear_mark();
      obj = (Object*)((uintptr_t)obj + obj->size_in_bytes());
    }
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

