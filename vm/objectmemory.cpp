#include <cstdlib>
#include <iostream>

#include "vm.hpp"
#include "objectmemory.hpp"
#include "gc/marksweep.hpp"
#include "gc/baker.hpp"
#include "gc/immix.hpp"
#include "gc/inflated_headers.hpp"
#include "gc/walker.hpp"
#include "on_stack.hpp"

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
#include "builtin/array.hpp"

#include "capi/handle.hpp"
#include "configuration.hpp"

#include "global_cache.hpp"

#include "instruments/profiler.hpp"

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
    , slab_size_(4096)
    , running_finalizers_(false)

    , collect_young_now(false)
    , collect_mature_now(false)
    , root_state_(state)

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

    // Must be last
    // delete inflated_headers_;

  }

  void ObjectMemory::assign_object_id(Object* obj) {
    LOCK_ME;

    // Double check we've got no id still after the lock.
    if(obj->object_id() > 0) return;

    obj->set_object_id(root_state_->om, ++last_object_id);
  }

  bool ObjectMemory::inflate_lock_count_overflow(STATE, ObjectHeader* obj, int count) {
    LOCK_ME;

    // Inflation always happens with the ObjectMemory lock held, so we don't
    // need to worry about another thread concurrently inflating it.
    //
    // But we do need to check that it's not already inflated.
    if(obj->inflated_header_p()) return false;

    InflatedHeader* ih = inflated_headers_->allocate(obj);
    ih->initialize_mutex(state->thread_id(), count);
    obj->set_inflated_header(ih);

    return true;
  }

  bool ObjectMemory::contend_for_lock(STATE, ObjectHeader* obj) {
    lock();

    // We want to lock obj, but someone else has it locked.
    //
    // If the lock is already inflated, no problem, just lock it!

    // Be sure obj is updated by the GC while we're waiting for it
    OnStack<1> os(state, obj);

step1:
    HeaderWord hdr = obj->header;

    // Only contend if the header is thin locked.
    if(hdr.f.meaning != eAuxWordLock) {
      std::cerr << "[LOCK " << state->thread_id()
                << " contend_for_lock error: not thin locked.]\n";
      unlock();
      return false;
    }

    // Ok, the header is not inflated, but we can't inflate it and take
    // the lock because the locking thread needs to do that, so indicate
    // that the object is being contended for and then wait on the
    // contention condvar until the object is unlocked.

    HeaderWord orig = obj->header;
    HeaderWord new_val = orig;

    new_val.f.LockContended = 1;

    if(!__sync_bool_compare_and_swap(&obj->header.flags64,
          orig.flags64,
          new_val.flags64)) {
      // Something changed since we started to down this path,
      // start over.
      goto step1;
    }

    // Ok, we've registered the lock contention, now spin and wait
    // for the us to be told to retry.

    if(cDebugThreading) {
      std::cerr << "[LOCK " << state->thread_id() << " waiting on contention]\n";
    }

    while(!obj->inflated_header_p()) {
      state->shared.gc_independent();

      contention_var_.wait(mutex());
      if(cDebugThreading) {
        std::cerr << "[LOCK " << state->thread_id() << " notified of contention breakage]\n";
      }

      state->shared.gc_dependent();
    }

    if(cDebugThreading) {
      std::cerr << "[LOCK " << state->thread_id() << " contention broken]\n";
    }

    // We lock the InflatedHeader here rather than returning
    // and letting ObjectHeader::lock because the GC might have run
    // and we've used OnStack<> specificly to deal with that.
    //
    // ObjectHeader::lock doesn't use OnStack<>, it just is sure to
    // not access this if there is chance that a call blocked and GC'd
    // (which is true in the case of this function).

    unlock();

    InflatedHeader* ih = obj->inflated_header();
    ih->lock_mutex(state);

    return true;
  }

  void ObjectMemory::release_contention() {
    LOCK_ME;
    contention_var_.broadcast();
  }

  bool ObjectMemory::inflate_and_lock(STATE, ObjectHeader* obj) {
    LOCK_ME;

    InflatedHeader* ih = 0;
    int initial_count = 0;

    HeaderWord orig = obj->header;
    HeaderWord tmp = orig;

    switch(tmp.f.meaning) {
    case eAuxWordEmpty:
      // ERROR, we can not be here because it's empty. This is only to
      // be called when the header is already in use.
      return false;
    case eAuxWordInflated:
      // Already inflated. ERROR, let the caller sort it out.
      return false;
    case eAuxWordObjID:
      // We could be have made a header before trying again, so
      // keep using the original one.
      ih = inflated_headers_->allocate(obj);
      ih->set_object_id(tmp.f.aux_word);
      break;
    case eAuxWordLock:
      // We have to locking the object to inflate it, thats the law.
      if(tmp.f.aux_word >> cAuxLockTIDShift != state->thread_id()) {
        return false;
      }

      ih = inflated_headers_->allocate(obj);
      initial_count = orig.f.aux_word & cAuxLockRecCountMask;
    }

    ih->initialize_mutex(state->thread_id(), initial_count + 1);

    tmp.all_flags = ih;
    tmp.f.meaning = eAuxWordInflated;

    while(!__sync_bool_compare_and_swap(&obj->header.flags64,
                                        orig.flags64,
                                        tmp.flags64)) {
      // The header can't have been inflated by another thread, the
      // inflation process holds the OM lock.
      //
      // So some other bits must have changed, so lets just spin and
      // keep trying to update it.

      // Sanity check that the meaning is still the same, if not, then
      // something is really wrong.
      orig = obj->header;
      if(orig.f.meaning != tmp.f.meaning) {
        std::cerr << "[LOCK object header consistence error detected.]\n";
        return false;
      }

      tmp.all_flags = ih;
      tmp.f.meaning = eAuxWordInflated;
    }

    return true;
  }

  bool ObjectMemory::inflate_for_contention(STATE, ObjectHeader* obj) {
    LOCK_ME;

    for(;;) {
      HeaderWord orig = obj->header;
      HeaderWord new_val = orig;

      InflatedHeader* ih = 0;

      switch(orig.f.meaning) {
      case eAuxWordEmpty:
        ih = inflated_headers_->allocate(obj);
        break;
      case eAuxWordObjID:
        // We could be have made a header before trying again, so
        // keep using the original one.
        ih = inflated_headers_->allocate(obj);
        ih->set_object_id(orig.f.aux_word);
        break;
      case eAuxWordLock:
        // We have to locking the object to inflate it, thats the law.
        if(new_val.f.aux_word >> cAuxLockTIDShift != state->thread_id()) {
          std::cerr << "[LOCK object locked by another thread while inflating for contention]\n";
          return false;
        } else {
          std::cerr << "[LOCK object locked while inflating for contention]\n";
          return false;
        }
      case eAuxWordInflated:
        std::cerr << "[LOCK asked to inflated already inflated lock]\n";
        return false;
      }

      ih->flags().LockContended = 0;

      new_val.all_flags = ih;
      new_val.f.meaning = eAuxWordInflated;

      if(!__sync_bool_compare_and_swap(&obj->header.flags64,
                                       orig.flags64,
                                       new_val.flags64)) {
        // Try it all over again.
        continue;
      }

      if(cDebugThreading) {
        std::cerr << "[LOCK " << state->thread_id() << " inflated lock for contention.]\n";
      }

      // Now inflated but not locked, which is what we want.
      return true;
    }
  }

  // WARNING: This returns an object who's body may not have been initialized.
  // It is the callers duty to initialize it.
  Object* ObjectMemory::new_object_fast(Class* cls, size_t bytes, object_type type) {
    LOCK_ME;

    if(Object* obj = young_->raw_allocate(bytes, &collect_young_now)) {
      objects_allocated++;
      bytes_allocated += bytes;

      if(collect_young_now) root_state_->interrupts.set_perform_gc();
      obj->init_header(cls, YoungObjectZone, type);
      return obj;
    } else {
      UNLOCK_ME;
      return new_object_typed(cls, bytes, type);
    }
  }

  bool ObjectMemory::refill_slab(gc::Slab& slab) {
    LOCK_ME;

    void* addr = young_->allocate_for_slab(slab_size_);

    if(!addr) return false;

    objects_allocated += slab.allocations();

    slab.refill(addr, slab_size_);

    return true;
  }

  void ObjectMemory::set_young_lifetime(size_t age) {
    LOCK_ME;

    young_->set_lifetime(age);
  }

  void ObjectMemory::debug_marksweep(bool val) {
    LOCK_ME;

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
    size_t sz = obj->size_in_bytes(root_state_);
    bytes_allocated += sz;

    Object* copy = immix_->allocate(sz);

    copy->set_obj_type(obj->type_id());
    copy->initialize_full_state(root_state_, obj, 0);

    if(watched_p(obj)) {
      std::cout << "detected object " << obj << " during promotion.\n";
    }

    return copy;
  }

  void ObjectMemory::collect(STATE, CallFrame* call_frame) {
    // Don't go any further unless we're allowed to GC.
    if(!can_gc()) return;

    lock();

    // If we were checkpointed, then someone else ran the GC, just return.
    if(state->shared.should_stop()) {
      unlock();
      state->shared.checkpoint();
      return;
    }

    if(cDebugThreading) {
      std::cerr << "\n[ " << state << " WORLD beginning GC.]\n";
    }

    // Stops all other threads, so we're only here by ourselves.
    //
    // First, ask them to stop.
    state->shared.ask_for_stopage();

    // Now unlock ObjectMemory so that they can spin to any checkpoints.
    unlock();

    // Wait for them all to check in.
    state->shared.stop_the_world();

    // Now we're alone, but we lock again just to safe.
    lock();

    GCData gc_data(state);

    collect_young(gc_data);
    collect_mature(gc_data);

    run_finalizers(state, call_frame);

    // Ok, we're good. Get everyone going again.
    state->shared.restart_world();
    unlock();
  }

  void ObjectMemory::collect_maybe(STATE, CallFrame* call_frame) {
    // Don't go any further unless we're allowed to GC.
    if(!can_gc()) return;

    lock();

    // If we were checkpointed, then someone else ran the GC, just return.
    if(state->shared.should_stop()) {
      unlock();
      state->shared.checkpoint();
      return;
    }

    if(cDebugThreading) {
      std::cerr << "\n[" << state << " WORLD beginning GC.]\n";
    }

    // Stops all other threads, so we're only here by ourselves.
    //
    // First, ask them to stop.
    state->shared.ask_for_stopage();

    // Now unlock ObjectMemory so that they can spin to any checkpoints.
    unlock();

    // Wait for them all to check in.
    state->shared.stop_the_world();

    // Now we're alone, but we lock again just to safe.
    lock();

    GCData gc_data(state);

    uint64_t start_time = 0;

    if(collect_young_now) {
      if(state->shared.config.gc_show) {
        start_time = get_current_time();
      }

      YoungCollectStats stats;

#ifdef RBX_PROFILER
      if(unlikely(state->shared.profiling())) {
        profiler::MethodEntry method(state, profiler::kYoungGC);
        collect_young(gc_data, &stats);
      } else {
        collect_young(gc_data, &stats);
      }
#else
      collect_young(gc_data, &stats);
#endif

      if(state->shared.config.gc_show) {
        uint64_t fin_time = get_current_time();
        int diff = (fin_time - start_time) / 1000000;

        fprintf(stderr, "[GC %0.1f%% %d/%d %d %2dms]\n",
                  stats.percentage_used,
                  stats.promoted_objects,
                  stats.excess_objects,
                  stats.lifetime,
                  diff);
      }
    }

    if(collect_mature_now) {
      int before_kb = 0;

      if(state->shared.config.gc_show) {
        start_time = get_current_time();
        before_kb = mature_bytes_allocated() / 1024;
      }

#ifdef RBX_PROFILER
      if(unlikely(state->shared.profiling())) {
        profiler::MethodEntry method(state, profiler::kMatureGC);
        collect_mature(gc_data);
      } else {
        collect_mature(gc_data);
      }
#else
      collect_mature(gc_data);
#endif

      if(state->shared.config.gc_show) {
        uint64_t fin_time = get_current_time();
        int diff = (fin_time - start_time) / 1000000;
        int kb = mature_bytes_allocated() / 1024;
        fprintf(stderr, "[Full GC %dkB => %dkB %2dms]\n",
            before_kb,
            kb,
            diff);
      }

    }

    // Count the finalizers toward running the mature gc. Not great,
    // but better than not seeing the time at all.
#ifdef RBX_PROFILER
    if(unlikely(state->shared.profiling())) {
      profiler::MethodEntry method(state, profiler::kMatureGC);
      run_finalizers(state, call_frame);
    } else {
      run_finalizers(state, call_frame);
    }
#else
    run_finalizers(state, call_frame);
#endif

    state->shared.restart_world();
    unlock();
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

    if(data.threads()) {
      for(std::list<ManagedThread*>::iterator i = data.threads()->begin();
          i != data.threads()->end();
          i++) {
        gc::Slab& slab = (*i)->local_slab();

        void* addr = young_->allocate_for_slab(slab_size_);
        assert(addr);

        objects_allocated += slab.allocations();

        slab.refill(addr, slab_size_);
      }
    }

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

    LOCK_ME;

    InflatedHeader* header = inflated_headers_->allocate(obj);
    obj->set_inflated_header(header);
    return header;
  }

  void ObjectMemory::inflate_for_id(ObjectHeader* obj, uint32_t id) {
    if(obj->inflated_header_p()) return;

    LOCK_ME;

    InflatedHeader* header = inflated_headers_->allocate(obj);
    header->set_object_id(id);
    obj->set_inflated_header(header);
  }

  void ObjectMemory::validate_handles(capi::Handles* handles) {
#ifndef NDEBUG
    capi::Handle* handle = handles->front();
    capi::Handle* current;

    while(handle) {
      current = handle;
      handle = static_cast<capi::Handle*>(handle->next());

      if(!handle->in_use_p()) continue;

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

      if(!current->in_use_p()) {
        count++;
        handles->remove(current);
        delete current;
        continue;
      }

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
    LOCK_ME;

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
        root_state_->interrupts.set_perform_gc();
      }

#ifdef RBX_GC_STATS
    stats::GCStats::get()->large_objects++;
#endif

    } else {
      obj = young_->allocate(bytes, &collect_young_now);
      if(unlikely(obj == NULL)) {
        collect_young_now = true;
        root_state_->interrupts.set_perform_gc();

        obj = immix_->allocate(bytes);
        if(collect_mature_now) {
          root_state_->interrupts.set_perform_gc();
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
        root_state_->interrupts.set_perform_gc();
      }

#ifdef RBX_GC_STATS
    stats::GCStats::get()->large_objects++;
#endif

    } else {
      obj = immix_->allocate(bytes);
      if(collect_mature_now) {
        root_state_->interrupts.set_perform_gc();
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
    LOCK_ME;

    Object* obj;

#ifdef RBX_GC_STATS
    stats::GCStats::get()->young_object_types[type]++;
#endif

    obj = allocate_object(bytes);
    if(unlikely(!obj)) return NULL;

    obj->klass(this, cls);

    obj->set_obj_type(type);

    return obj;
  }

  Object* ObjectMemory::new_object_typed_mature(Class* cls, size_t bytes, object_type type) {
    LOCK_ME;

    Object* obj;

#ifdef RBX_GC_STATS
    stats::GCStats::get()->mature_object_types[type]++;
#endif

    obj = allocate_object_mature(bytes);
    if(unlikely(!obj)) return NULL;

    obj->klass(this, cls);

    obj->set_obj_type(type);

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
    LOCK_ME;

#ifdef RBX_GC_STATS
    stats::GCStats::get()->mature_object_types[type]++;
#endif

    objects_allocated++;
    bytes_allocated += bytes;

    Object* obj = mark_sweep_->allocate(bytes, &collect_mature_now);
    if(collect_mature_now) {
      root_state_->interrupts.set_perform_gc();
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
    LOCK_ME;

    code_manager_.add_resource(cr);
  }

  void* ObjectMemory::young_start() {
    return young_->start_address();
  }

  void* ObjectMemory::yound_end() {
    return young_->last_address();
  }

  void ObjectMemory::needs_finalization(Object* obj, FinalizerFunction func) {
    LOCK_ME;

    FinalizeObject fi;
    fi.object = obj;
    fi.status = FinalizeObject::eLive;
    fi.finalizer = func;

    // Makes a copy of fi.
    finalize_.push_back(fi);
  }

  void ObjectMemory::set_ruby_finalizer(Object* obj, Object* fin) {
    LOCK_ME;

    // See if there already one.
    for(std::list<FinalizeObject>::iterator i = finalize_.begin();
        i != finalize_.end(); i++)
    {
      if(i->object == obj) {
        if(fin->nil_p()) {
          finalize_.erase(i);
        } else {
          i->ruby_finalizer = fin;
        }
        return;
      }
    }

    // Ok, create it.

    FinalizeObject fi;
    fi.object = obj;
    fi.status = FinalizeObject::eLive;

    // Rubinius specific API. If the finalizer is the object, we're going to send
    // the object __finalize__. We mark that the user wants this by putting Qtrue
    // as the ruby_finalizer.
    if(obj == fin) {
      fi.ruby_finalizer = Qtrue;
    } else {
      fi.ruby_finalizer = fin;
    }

    // Makes a copy of fi.
    finalize_.push_back(fi);
  }

  void ObjectMemory::run_finalizers(STATE, CallFrame* call_frame) {
    if(running_finalizers_) return;
    running_finalizers_ = true;

    for(std::list<FinalizeObject*>::iterator i = to_finalize_.begin();
        i != to_finalize_.end(); ) {
      FinalizeObject* fi = *i;

      if(fi->finalizer) {
        (*fi->finalizer)(state, fi->object);
        // Unhook any handle used by fi->object so that we don't accidentally
        // try and mark it later (after we've finalized it)
        if(fi->object->inflated_header_p()) {
          InflatedHeader* ih = fi->object->inflated_header();

          if(capi::Handle* handle = ih->handle()) {
            handle->forget_object();
            ih->set_handle(0);
          }
        }

        // If the object was remembered, unremember it.
        if(fi->object->remembered_p()) {
          unremember_object(fi->object);
        }
      } else if(fi->ruby_finalizer) {
        // Rubinius specific code. If the finalizer is Qtrue, then
        // send the object the finalize message
        if(fi->ruby_finalizer == Qtrue) {
          fi->object->send(state, call_frame, state->symbol("__finalize__"), true);
        } else {
          Array* ary = Array::create(state, 1);
          ary->set(state, 0, fi->object->id(state));

          OnStack<1> os(state, ary);

          fi->ruby_finalizer->send(state, call_frame, state->symbol("call"), ary, Qnil, true);
        }
      } else {
        std::cerr << "Unsupported object to be finalized: "
                  << fi->object->to_s(state)->c_str() << "\n";
      }

      fi->status = FinalizeObject::eFinalized;

      i = to_finalize_.erase(i);
    }

    running_finalizers_ = false;
  }

  void ObjectMemory::run_all_finalizers(STATE) {
    if(running_finalizers_) return;
    running_finalizers_ = true;

    for(std::list<FinalizeObject>::iterator i = finalize_.begin();
        i != finalize_.end(); )
    {
      FinalizeObject& fi = *i;

      // Only finalize things that haven't been finalized.
      if(fi.status != FinalizeObject::eFinalized) {
        if(fi.finalizer) {
          (*fi.finalizer)(state, fi.object);
        } else if(fi.ruby_finalizer) {
          // Rubinius specific code. If the finalizer is Qtrue, then
          // send the object the finalize message
          if(fi.ruby_finalizer == Qtrue) {
            fi.object->send(state, 0, state->symbol("__finalize__"), true);
          } else {
            Array* ary = Array::create(state, 1);
            ary->set(state, 0, fi.object->id(state));

            OnStack<1> os(state, ary);

            fi.ruby_finalizer->send(state, 0, state->symbol("call"), ary, Qnil, true);
          }
        } else {
          std::cerr << "During shutdown, unsupported object to be finalized: "
                    << fi.object->to_s(state)->c_str() << "\n";
        }
      }

      fi.status = FinalizeObject::eFinalized;

      i = finalize_.erase(i);
    }

    running_finalizers_ = false;
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

    int baker = root_state_->shared.config.gc_bytes * 2;
    total += baker;

    int immix = immix_->bytes_allocated();
    total += immix;

    int large = mark_sweep_->allocated_bytes;
    total += large;

    int code = code_manager_.size();
    total += code;

    int shared = root_state_->shared.size();
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
    ObjectMemory::GCInhibit inhibitor(root_state_->om);

    ObjectWalker walker(root_state_->om);
    GCData gc_data(root_state_);

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
    ObjectMemory::GCInhibit inhibitor(root_state_->om);

    // Walk the heap over and over until we don't create
    // any more objects...

    size_t last_seen = 0;

    while(last_object_id != last_seen) {
      last_seen = last_object_id;

      ObjectWalker walker(root_state_->om);
      GCData gc_data(root_state_);

      // Seed it with the root objects.
      walker.seed(gc_data);

      Object* obj = walker.next();

      while(obj) {
        obj->id(root_state_);
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
    ObjectMemory::GCInhibit inhibitor(root_state_->om);

    ObjectWalker walker(root_state_->om);
    GCData gc_data(root_state_);

    // Seed it with the root objects.
    walker.seed(gc_data);

    Object* obj = walker.next();

    // All reference ids are shifted up
    native_int check_id = (native_int)last_snapshot_id << 1;

    int count = 0;
    int bytes = 0;

    while(obj) {
      if(!obj->has_id(root_state_) || obj->id(root_state_)->to_native() > check_id) {
        count++;
        bytes += obj->size_in_bytes(root_state_);

        if(kind_of<String>(obj)) {
          std::cout << "#<String:" << obj << ">\n";
        } else {
          std::cout << obj->to_s(root_state_, true)->c_str() << "\n";
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
      std::cout << "  " << (*i)->to_s(root_state_, true)->c_str() << "\n";

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

