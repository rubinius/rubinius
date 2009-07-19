#include <cstdlib>
#include <iostream>

#include "vm.hpp"
#include "objectmemory.hpp"
#include "gc/marksweep.hpp"
#include "config_parser.hpp"

#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/tuple.hpp"

#include "capi/handle.hpp"
#include "configuration.hpp"

#include "global_cache.hpp"

namespace rubinius {

  Object* object_watch = 0;

  /* ObjectMemory methods */
  ObjectMemory::ObjectMemory(STATE, Configuration& config)
    : state(state)
    , young(this, config.gc_bytes)
    , mark_sweep_(this)
    , immix_(this)
  {

    // TODO Not sure where this code should be...
    if(char* num = getenv("RBX_WATCH")) {
      object_watch = (Object*)strtol(num, NULL, 10);
      std::cout << "Watching for " << object_watch << "\n";
    }

    remember_set = new ObjectArray(0);

    collect_young_now = false;
    collect_mature_now = false;
    last_object_id = 0;

    large_object_threshold = config.gc_large_object;
    young.lifetime = config.gc_lifetime;

    for(size_t i = 0; i < LastObjectType; i++) {
      type_info[i] = NULL;
    }

    TypeInfo::init(this);
  }

  ObjectMemory::~ObjectMemory() {

    young.free_objects();
    mark_sweep_.free_objects();

    // TODO free immix data

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
      mark_sweep_.free_entries = false;
    } else {
      mark_sweep_.free_entries = true;
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
#ifdef RBX_GC_STATS
    stats::GCStats::get()->objects_promoted++;
#endif

    Object* copy = immix_.allocate(obj->size_in_bytes(state));

    copy->obj_type_ = obj->type_id();
    copy->initialize_copy(obj, 0);
    copy->copy_body(state, obj);

    if(watched_p(obj)) {
      std::cout << "detected object " << obj << " during promotion.\n";
    }

    return copy;
  }

  void ObjectMemory::collect_young(GCData& data) {
    static int collect_times = 0;
    young.collect(data);
    prune_handles(data.handles(), true);
    prune_handles(data.cached_handles(), true);
    collect_times++;

    data.global_cache()->prune_young();
  }

  void ObjectMemory::collect_mature(GCData& data) {
#ifdef RBX_GC_STATS
    stats::GCStats::get()->objects_seen.start();
    stats::GCStats::get()->collect_mature.start();
#endif

    immix_.collect(data);

    mark_sweep_.after_marked();

    immix_.clean_weakrefs();
    prune_handles(data.handles(), false);
    prune_handles(data.cached_handles(), false);

    data.global_cache()->prune_unmarked();

    immix_.unmark_all(data);

#ifdef RBX_GC_STATS
    stats::GCStats::get()->collect_mature.stop();
    stats::GCStats::get()->objects_seen.stop();
#endif
  }

  void ObjectMemory::prune_handles(capi::Handles* handles, bool check_forwards) {
    capi::Handle* handle = handles->front();
    capi::Handle* current;

    int total = 0;
    int count = 0;

    while(handle) {
      current = handle;
      handle = static_cast<capi::Handle*>(handle->next());

      Object* obj = current->object();
      total++;

      // Strong references will already have been updated.
      if(!current->weak_p()) {
        if(check_forwards) assert(!obj->forwarded_p());
      } else if(check_forwards) {
        if(obj->young_object_p()) {

          // A weakref pointing to a valid young object
          if(young.validate_object(obj) == cValid) {
            continue;

          // A weakref pointing to a forwarded young object
          } else if(obj->forwarded_p()) {
            current->set_object(obj->forward());

          // A weakref pointing to a dead young object
          } else {
            count++;
            handles->remove(current);
            delete current;
          }
        }

      // A weakref pointing to a dead mature object
      } else if(!obj->marked_p()) {
        count++;
        handles->remove(current);
        delete current;
      }
    }

    // std::cout << "Pruned " << count << " handles, " << total << "/" << handles->size() << " total.\n";
  }

  void ObjectMemory::add_type_info(TypeInfo* ti) {
    type_info[ti->type] = ti;
  }

  /* Store an object into the remember set. Called when we've calculated
   * externally that the object in question needs to be remembered */
  void ObjectMemory::remember_object(Object* target) {
    assert(target->zone == MatureObjectZone);
    /* If it's already remembered, ignore this request */
    if(target->remembered_p()) return;
    target->set_remember();
    remember_set->push_back(target);
  }

  void ObjectMemory::unremember_object(Object* target) {
    for(ObjectArray::iterator oi = remember_set->begin();
        oi != remember_set->end();
        oi++) {
      if(*oi == target) {
        *oi = NULL;
        target->clear_remember();
      }
    }
  }

  // DEPRECATED
  void ObjectMemory::store_object(Object* target, size_t index, Object* val) {
    ((Tuple*)target)->field[index] = val;
    write_barrier(target, val);
  }

  void ObjectMemory::set_class(Object* target, Object* obj) {
    // the setter calls write_barrier when necessary.
    target->klass(state, (Class*)obj);
  }

  Object* ObjectMemory::allocate_object(size_t bytes) {
    Object* obj;

    if(unlikely(bytes > large_object_threshold)) {
      obj = mark_sweep_.allocate(bytes, &collect_mature_now);
      if(collect_mature_now) {
        state->interrupts.check = true;
      }

#ifdef RBX_GC_STATS
    stats::GCStats::get()->large_objects++;
#endif

    } else {
      obj = young.allocate(bytes, &collect_young_now);
      if(unlikely(obj == NULL)) {
        collect_young_now = true;
        state->interrupts.check = true;

        obj = immix_.allocate(bytes);
        if(collect_mature_now) {
          state->interrupts.check = true;
        }
      }
    }

#ifdef ENABLE_OBJECT_WATCH
    if(watched_p(obj)) {
      std::cout << "detected " << obj << " during allocation\n";
    }
#endif

    obj->clear_fields(bytes);
    return obj;
  }

  Object* ObjectMemory::allocate_object_mature(size_t bytes) {
    Object* obj;

    if(bytes > large_object_threshold) {
      obj = mark_sweep_.allocate(bytes, &collect_mature_now);
      if(collect_mature_now) {
        state->interrupts.check = true;
      }

#ifdef RBX_GC_STATS
    stats::GCStats::get()->large_objects++;
#endif

    } else {
      obj = immix_.allocate(bytes);
      if(collect_mature_now) {
        state->interrupts.check = true;
      }
    }

#ifdef ENABLE_OBJECT_WATCH
    if(watched_p(obj)) {
      std::cout << "detected " << obj << " during mature allocation\n";
    }
#endif

    obj->clear_fields(bytes);
    return obj;
  }

  Object* ObjectMemory::new_object_typed(Class* cls, size_t bytes, object_type type) {
    Object* obj;

#ifdef RBX_GC_STATS
    stats::GCStats::get()->young_object_types[type]++;
#endif

    obj = allocate_object(bytes);
    set_class(obj, cls);

    obj->obj_type_ = type;
    obj->set_requires_cleanup(type_info[type]->instances_need_cleanup);

    return obj;
  }

  Object* ObjectMemory::new_object_typed_mature(Class* cls, size_t bytes, object_type type) {
    Object* obj;

#ifdef RBX_GC_STATS
    stats::GCStats::get()->mature_object_types[type]++;
#endif

    obj = allocate_object_mature(bytes);
    set_class(obj, cls);

    obj->obj_type_ = type;
    obj->set_requires_cleanup(type_info[type]->instances_need_cleanup);

    return obj;
  }

  TypeInfo* ObjectMemory::find_type_info(Object* obj) {
    return type_info[obj->type_id()];
  }

  ObjectPosition ObjectMemory::validate_object(Object* obj) {
    ObjectPosition pos;

    pos = young.validate_object(obj);
    if(pos != cUnknown) return pos;

    pos = immix_.validate_object(obj);
    if(pos != cUnknown) return pos;

    return mark_sweep_.validate_object(obj);
  }
};

#define DEFAULT_MALLOC_THRESHOLD 10000000

static long bytes_until_collection = DEFAULT_MALLOC_THRESHOLD;

void* XMALLOC(size_t bytes) {
  bytes_until_collection -= bytes;
  if(bytes_until_collection <= 0) {
    rubinius::VM::current_state()->run_gc_soon();
    bytes_until_collection = DEFAULT_MALLOC_THRESHOLD;
  }
  return malloc(bytes);
}

void XFREE(void* ptr) {
  free(ptr);
}

void* XREALLOC(void* ptr, size_t bytes) {
  bytes_until_collection -= bytes;
  if(bytes_until_collection <= 0) {
    rubinius::VM::current_state()->run_gc_soon();
    bytes_until_collection = DEFAULT_MALLOC_THRESHOLD;
  }

  return realloc(ptr, bytes);
}

void* XCALLOC(size_t items, size_t bytes_per) {
  size_t bytes = bytes_per * items;

  bytes_until_collection -= bytes;
  if(bytes_until_collection <= 0) {
    rubinius::VM::current_state()->run_gc_soon();
    bytes_until_collection = DEFAULT_MALLOC_THRESHOLD;
  }

  return calloc(items, bytes_per);
}

