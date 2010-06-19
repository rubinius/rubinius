#include <cstdlib>
#include <iostream>

#include "vm.hpp"
#include "objectmemory.hpp"
#include "gc/marksweep.hpp"
#include "gc/baker.hpp"
#include "gc/immix.hpp"
#include "gc/inflated_headers.hpp"
#include "gc/walker.hpp"

#include "config_parser.hpp"

#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/tuple.hpp"
#include "builtin/io.hpp"
#include "builtin/fiber.hpp"
#include "builtin/string.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/ffi_pointer.hpp"
#include "builtin/data.hpp"
#include "builtin/dir.hpp"

#include "capi/handle.hpp"
#include "configuration.hpp"

#include "global_cache.hpp"

namespace rubinius {

  Object* object_watch = 0;

  /* ObjectMemory methods */
  ObjectMemory::ObjectMemory(STATE, Configuration& config)
    : young_(new BakerGC(this, config.gc_bytes))
    , mark_sweep_(new MarkSweepGC(this))
    , immix_(new ImmixGC(this))
    , inflated_headers_(new InflatedHeaders)
    , mark_(1)
    , code_manager_(&state->shared)
    , allow_gc_(true)
    , collect_young_now(false)
    , collect_mature_now(false)
    , state(state)

    , objects_allocated(0)
    , bytes_allocated(0)
    , young_collections(0)
    , full_collections(0)
    , young_collection_time(0)
    , full_collection_time(0)
  {
    // TODO Not sure where this code should be...
    if(char* num = getenv("RBX_WATCH")) {
      object_watch = (Object*)strtol(num, NULL, 10);
      std::cout << "Watching for " << object_watch << "\n";
    }

    collect_mature_now = false;
    last_object_id = 0;

    large_object_threshold = config.gc_large_object;
    young_->set_lifetime(config.gc_lifetime);

    if(config.gc_autotune) young_->set_autotune();

    for(size_t i = 0; i < LastObjectType; i++) {
      type_info[i] = NULL;
    }

    TypeInfo::init(this);
  }

  ObjectMemory::~ObjectMemory() {

    mark_sweep_->free_objects();

    // TODO free immix data

    for(size_t i = 0; i < LastObjectType; i++) {
      if(type_info[i]) delete type_info[i];
    }

    delete immix_;
    delete mark_sweep_;
    delete young_;
  }

  Object* ObjectMemory::new_object_fast(Class* cls, size_t bytes, object_type type) {
    if(Object* obj = young_->raw_allocate(bytes, &collect_young_now)) {
      objects_allocated++;
      bytes_allocated += bytes;

      if(collect_young_now) state->interrupts.set_perform_gc();
      obj->init_header(cls, YoungObjectZone, type);
      obj->clear_fields(bytes);
      return obj;
    } else {
      return new_object_typed(cls, bytes, type);
    }
  }


  void ObjectMemory::set_young_lifetime(size_t age) {
    young_->set_lifetime(age);
  }

  void ObjectMemory::debug_marksweep(bool val) {
    if(val) {
      mark_sweep_->free_entries = false;
    } else {
      mark_sweep_->free_entries = true;
    }
  }

  bool ObjectMemory::valid_object_p(Object* obj) {
    if(obj->young_object_p()) {
      return young_->validate_object(obj) == cValid;
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

    objects_allocated++;
    size_t sz = obj->size_in_bytes(state);
    bytes_allocated += sz;

    Object* copy = immix_->allocate(sz);

    copy->set_obj_type(obj->type_id());
    copy->initialize_full_state(state, obj, 0);

    if(watched_p(obj)) {
      std::cout << "detected object " << obj << " during promotion.\n";
    }

    return copy;
  }

  void ObjectMemory::collect_young(GCData& data, YoungCollectStats* stats) {
    collect_young_now = false;

    timer::Running<size_t, 1000000> timer(young_collection_time);

    // validate_handles(data.handles());
    // validate_handles(data.cached_handles());

    young_->reset_stats();

    young_->collect(data, stats);

    prune_handles(data.handles(), true);
    prune_handles(data.cached_handles(), true);
    young_collections++;

    data.global_cache()->prune_young();
  }

  void ObjectMemory::collect_mature(GCData& data) {
#ifdef RBX_GC_STATS
    stats::GCStats::get()->objects_seen.start();
    stats::GCStats::get()->collect_mature.start();
#endif

    // validate_handles(data.handles());
    // validate_handles(data.cached_handles());

    timer::Running<size_t, 1000000> timer(full_collection_time);

    collect_mature_now = false;

    code_manager_.clear_marks();

    immix_->reset_stats();

    immix_->collect(data);

    immix_->clean_weakrefs();

    code_manager_.sweep();

    data.global_cache()->prune_unmarked(mark());

    prune_handles(data.handles(), false);
    prune_handles(data.cached_handles(), false);


    // Have to do this after all things that check for mark bits is
    // done, as it free()s objects, invalidating mark bits.
    mark_sweep_->after_marked();

    inflated_headers_->deallocate_headers(mark());

    // We no longer need to unmark all, we use the rotating mark instead.
    // This means that the mark we just set on all reachable objects will
    // be ignored next time anyway.
    //
    // immix_->unmark_all(data);

    rotate_mark();
    full_collections++;

#ifdef RBX_GC_STATS
    stats::GCStats::get()->collect_mature.stop();
    stats::GCStats::get()->objects_seen.stop();
#endif
  }

  InflatedHeader* ObjectMemory::inflate_header(ObjectHeader* obj) {
    if(obj->inflated_header_p()) return obj->inflated_header();

    InflatedHeader* header = inflated_headers_->allocate(obj);
    obj->set_inflated_header(header);
    return header;
  }

  void ObjectMemory::validate_handles(capi::Handles* handles) {
#ifndef NDEBUG
    capi::Handle* handle = handles->front();
    capi::Handle* current;

    while(handle) {
      current = handle;
      handle = static_cast<capi::Handle*>(handle->next());

      Object* obj = current->object();

      assert(obj->inflated_header_p());
      InflatedHeader* ih = obj->inflated_header();

      assert(ih->handle() == current);
      assert(ih->object() == obj);
    }
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
        assert(obj->inflated_header()->object() == obj);
      } else if(check_forwards) {
        if(obj->young_object_p()) {

          // A weakref pointing to a valid young object
          //
          // TODO this only works because we run prune_handles right after
          // a collection. In this state, valid objects are only in current.
          if(young_->in_current_p(obj)) {
            continue;

          // A weakref pointing to a forwarded young object
          } else if(obj->forwarded_p()) {
            current->set_object(obj->forward());
            assert(current->object()->inflated_header_p());
            assert(current->object()->inflated_header()->object() == current->object());

          // A weakref pointing to a dead young object
          } else {
            count++;
            handles->remove(current);
            delete current;
          }
        }

      // A weakref pointing to a dead mature object
      } else if(!obj->marked_p(mark())) {
        count++;
        handles->remove(current);
        delete current;
      } else {
        assert(obj->inflated_header()->object() == obj);
      }
    }

    // std::cout << "Pruned " << count << " handles, " << total << "/" << handles->size() << " total.\n";
  }

  int ObjectMemory::mature_bytes_allocated() {
    return immix_->bytes_allocated() + mark_sweep_->allocated_bytes;
  }

  void ObjectMemory::add_type_info(TypeInfo* ti) {
    if(TypeInfo* current = type_info[ti->type]) {
      delete current;
    }
    type_info[ti->type] = ti;
  }

  Object* ObjectMemory::allocate_object(size_t bytes) {
    objects_allocated++;
    bytes_allocated += bytes;

    Object* obj;

    if(unlikely(bytes > large_object_threshold)) {
      obj = mark_sweep_->allocate(bytes, &collect_mature_now);
      if(unlikely(!obj)) return NULL;

      if(collect_mature_now) {
        state->interrupts.set_perform_gc();
      }

#ifdef RBX_GC_STATS
    stats::GCStats::get()->large_objects++;
#endif

    } else {
      obj = young_->allocate(bytes);
      if(unlikely(obj == NULL)) {
        collect_young_now = true;
        state->interrupts.set_perform_gc();

        obj = immix_->allocate(bytes);
        if(collect_mature_now) {
          state->interrupts.set_perform_gc();
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
    objects_allocated++;
    bytes_allocated += bytes;

    Object* obj;

    if(bytes > large_object_threshold) {
      obj = mark_sweep_->allocate(bytes, &collect_mature_now);
      if(unlikely(!obj)) return NULL;

      if(collect_mature_now) {
        state->interrupts.set_perform_gc();
      }

#ifdef RBX_GC_STATS
    stats::GCStats::get()->large_objects++;
#endif

    } else {
      obj = immix_->allocate(bytes);
      if(collect_mature_now) {
        state->interrupts.set_perform_gc();
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
    if(unlikely(!obj)) return NULL;

    obj->klass(this, cls);

    obj->set_obj_type(type);
    obj->set_requires_cleanup(type_info[type]->instances_need_cleanup);

    return obj;
  }

  Object* ObjectMemory::new_object_typed_mature(Class* cls, size_t bytes, object_type type) {
    Object* obj;

#ifdef RBX_GC_STATS
    stats::GCStats::get()->mature_object_types[type]++;
#endif

    obj = allocate_object_mature(bytes);
    if(unlikely(!obj)) return NULL;

    obj->klass(this, cls);

    obj->set_obj_type(type);
    obj->set_requires_cleanup(type_info[type]->instances_need_cleanup);

    return obj;
  }

  /* ONLY use to create Class, the first object. */
  Object* ObjectMemory::allocate_object_raw(size_t bytes) {
    objects_allocated++;
    bytes_allocated += bytes;

    Object* obj = mark_sweep_->allocate(bytes, &collect_mature_now);
    obj->clear_fields(bytes);
    return obj;
  }

  Object* ObjectMemory::new_object_typed_enduring(Class* cls, size_t bytes, object_type type) {
#ifdef RBX_GC_STATS
    stats::GCStats::get()->mature_object_types[type]++;
#endif

    objects_allocated++;
    bytes_allocated += bytes;

    Object* obj = mark_sweep_->allocate(bytes, &collect_mature_now);
    if(collect_mature_now) {
      state->interrupts.set_perform_gc();
    }

#ifdef ENABLE_OBJECT_WATCH
    if(watched_p(obj)) {
      std::cout << "detected " << obj << " during enduring allocation\n";
    }
#endif

    obj->clear_fields(bytes);

#ifdef RBX_GC_STATS
    stats::GCStats::get()->large_objects++;
#endif

    obj->klass(this, cls);

    obj->set_obj_type(type);
    obj->set_requires_cleanup(type_info[type]->instances_need_cleanup);

    return obj;
  }

  TypeInfo* ObjectMemory::find_type_info(Object* obj) {
    return type_info[obj->type_id()];
  }

  ObjectPosition ObjectMemory::validate_object(Object* obj) {
    ObjectPosition pos;

    pos = young_->validate_object(obj);
    if(pos != cUnknown) return pos;

    pos = immix_->validate_object(obj);
    if(pos != cUnknown) return pos;

    return mark_sweep_->validate_object(obj);
  }

  bool ObjectMemory::valid_young_object_p(Object* obj) {
    return obj->young_object_p() && young_->in_current_p(obj);
  }

  void ObjectMemory::add_code_resource(CodeResource* cr) {
    code_manager_.add_resource(cr);
  }

  void ObjectMemory::needs_finalization(Object* obj, FinalizerFunction func) {
    FinalizeObject fi;
    fi.object = obj;
    fi.status = FinalizeObject::eLive;
    fi.finalizer = func;

    // Makes a copy of fi.
    finalize_.push_back(fi);
  }

  void ObjectMemory::run_finalizers(STATE) {
    for(std::list<FinalizeObject*>::iterator i = to_finalize_.begin();
        i != to_finalize_.end(); ) {
      FinalizeObject* fi = *i;

      if(fi->finalizer) {
        (*fi->finalizer)(state, fi->object);
      } else {
        std::cerr << "Unsupported object to be finalized: "
                  << fi->object->to_s(state)->c_str() << "\n";
      }

      fi->status = FinalizeObject::eFinalized;

      i = to_finalize_.erase(i);
    }

  }

  size_t& ObjectMemory::loe_usage() {
    return mark_sweep_->allocated_bytes;
  }

  size_t& ObjectMemory::immix_usage() {
    return immix_->bytes_allocated();
  }

  size_t& ObjectMemory::code_usage() {
    return (size_t&)code_manager_.size();
  }

  void ObjectMemory::memstats() {
    int total = 0;

    int baker = state->shared.config.gc_bytes * 2;
    total += baker;

    int immix = immix_->bytes_allocated();
    total += immix;

    int large = mark_sweep_->allocated_bytes;
    total += large;

    int code = code_manager_.size();
    total += code;

    int shared = state->shared.size();
    total += shared;

    std::cout << "baker: " << baker << "\n";
    std::cout << "immix: " << immix << "\n";
    std::cout << "large: " << large << "\n"
              << "        objects: " << mark_sweep_->allocated_objects << "\n"
              << "        times: " << mark_sweep_->times_collected << "\n"
              << "        last_freed: " << mark_sweep_->last_freed << "\n";
    std::cout << " code: " << code << "\n";
    std::cout << "shared: " << shared << "\n";

    std::cout << "total: "
              << ((double)total / (1024 * 1024))
              << " M\n";

    std::cout << "CodeManager:\n";
    std::cout << "  total allocated: " << code_manager_.total_allocated() << "\n";
    std::cout << "      total freed: " << code_manager_.total_freed() << "\n";
  }

  class RefererFinder : public GarbageCollector {
    Object* target_;
    bool found_;

  public:
    RefererFinder(ObjectMemory* om, Object* obj)
      : GarbageCollector(om)
      , target_(obj)
      , found_(false)
    {}

    virtual Object* saw_object(Object* obj) {
      if(obj == target_) {
        found_ = true;
      }

      return obj;
    }

    void reset() {
      found_ = false;
    }

    bool found_p() {
      return found_;
    }
  };

  void ObjectMemory::find_referers(Object* target, ObjectArray& result) {
    ObjectMemory::GCInhibit inhibitor(state->om);

    ObjectWalker walker(state->om);
    GCData gc_data(state);

    // Seed it with the root objects.
    walker.seed(gc_data);

    Object* obj = walker.next();

    RefererFinder rf(this, target);

    while(obj) {
      rf.reset();

      rf.scan_object(obj);

      if(rf.found_p()) {
        result.push_back(obj);
      }

      obj = walker.next();
    }
  }

  void ObjectMemory::snapshot() {
    // Assign all objects an object id...
    ObjectMemory::GCInhibit inhibitor(state->om);

    // Walk the heap over and over until we don't create
    // any more objects...

    size_t last_seen = 0;

    while(last_object_id != last_seen) {
      last_seen = last_object_id;

      ObjectWalker walker(state->om);
      GCData gc_data(state);

      // Seed it with the root objects.
      walker.seed(gc_data);

      Object* obj = walker.next();

      while(obj) {
        obj->id(state);
        obj = walker.next();
      }
    }

    // Now, save the current value of last_object_id, since thats
    // so we can compare later to find all new objects.
    last_snapshot_id = last_object_id;

    std::cout << "Snapshot taken: " << last_snapshot_id << "\n";
  }

  void ObjectMemory::print_new_since_snapshot() {
    // Assign all objects an object id...
    ObjectMemory::GCInhibit inhibitor(state->om);

    ObjectWalker walker(state->om);
    GCData gc_data(state);

    // Seed it with the root objects.
    walker.seed(gc_data);

    Object* obj = walker.next();

    // All reference ids are shifted up
    native_int check_id = (native_int)last_snapshot_id << 1;

    int count = 0;
    int bytes = 0;

    while(obj) {
      if(!obj->has_id(state) || obj->id(state)->to_native() > check_id) {
        count++;
        bytes += obj->size_in_bytes(state);

        if(kind_of<String>(obj)) {
          std::cout << "#<String:" << obj << ">\n";
        } else {
          std::cout << obj->to_s(state, true)->c_str() << "\n";
        }
      }

      obj = walker.next();
    }

    std::cout << count << " objects since snapshot.\n";
    std::cout << bytes << " bytes since snapshot.\n";
  }

  void ObjectMemory::print_references(Object* obj) {
    ObjectArray ary;

    find_referers(obj, ary);

    int count = 0;

    std::cout << ary.size() << " total references:\n";
    for(ObjectArray::iterator i = ary.begin();
        i != ary.end();
        i++) {
      std::cout << "  " << (*i)->to_s(state, true)->c_str() << "\n";

      if(++count == 100) break;
    }
  }
};

// Used in gdb
void x_memstat() {
  rubinius::VM::current_state()->om->memstats();
}

void print_references(void* obj) {
  rubinius::VM::current_state()->om->print_references((rubinius::Object*)obj);
}

void x_snapshot() {
  rubinius::VM::current_state()->om->snapshot();
}

void x_print_snapshot() {
  rubinius::VM::current_state()->om->print_new_since_snapshot();
}

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

