#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <sys/time.h>

#include "config.h"
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
#include "builtin/thread.hpp"

#include "capi/handles.hpp"
#include "configuration.hpp"

#include "global_cache.hpp"

#include "instruments/tooling.hpp"

// Used by XMALLOC at the bottom
static long gc_malloc_threshold = 0;
static long bytes_until_collection = 0;

namespace rubinius {

  Object* object_watch = 0;

  /* ObjectMemory methods */
  ObjectMemory::ObjectMemory(VM* state, Configuration& config)
    : young_(new BakerGC(this, config.gc_bytes))
    , mark_sweep_(new MarkSweepGC(this, config))
    , immix_(new ImmixGC(this))
    , inflated_headers_(new InflatedHeaders(state))
    , mark_(1)
    , code_manager_(&state->shared)
    , allow_gc_(true)
    , slab_size_(4096)
    , running_finalizers_(false)
    , added_finalizers_(false)
    , finalizer_thread_(state, nil<Thread>())
    , shared_(state->shared)

    , collect_young_now(false)
    , collect_mature_now(false)
    , root_state_(state)
    , last_object_id(1)
  {
    // TODO Not sure where this code should be...
    if(char* num = getenv("RBX_WATCH")) {
      object_watch = reinterpret_cast<Object*>(strtol(num, NULL, 10));
      std::cout << "Watching for " << object_watch << "\n";
    }

    large_object_threshold = config.gc_large_object;
    young_->set_lifetime(config.gc_lifetime);

    if(config.gc_autotune) young_->set_autotune();

    for(size_t i = 0; i < LastObjectType; i++) {
      type_info[i] = NULL;
    }

    TypeInfo::init(this);

    gc_malloc_threshold = config.gc_malloc_threshold;
    bytes_until_collection = gc_malloc_threshold;
  }

  ObjectMemory::~ObjectMemory() {
    mark_sweep_->free_objects();

    // @todo free immix data

    for(size_t i = 0; i < LastObjectType; i++) {
      if(type_info[i]) delete type_info[i];
    }

    delete immix_;
    delete mark_sweep_;
    delete young_;

    // Must be last
    delete inflated_headers_;

  }

  void ObjectMemory::on_fork(STATE) {
    lock_init(state->vm());
    contention_lock_.init();
    finalizer_lock_.init();
    finalizer_var_.init();
    finalizer_thread_.set(nil<Thread>());
  }

  void ObjectMemory::assign_object_id(STATE, Object* obj) {
    // Double check we've got no id still after the lock.
    if(obj->object_id() > 0) return;

    obj->set_object_id(state, state->memory(), atomic::fetch_and_add(&last_object_id, (size_t)1));
  }

  Integer* ObjectMemory::assign_object_id_ivar(STATE, Object* obj) {
    SYNC(state);
    Object* id = obj->get_ivar(state, G(sym_object_id));
    if(id->nil_p()) {
      /* All references have an even object_id. last_object_id starts out at 0
       * but we don't want to use 0 as an object_id, so we just add before using */
      id = Integer::from(state, ++state->memory()->last_object_id << TAG_REF_WIDTH);
      obj->set_ivar(state, G(sym_object_id), id);
    }
    return as<Integer>(id);
  }

  bool ObjectMemory::inflate_lock_count_overflow(STATE, ObjectHeader* obj,
                                                 int count)
  {
    utilities::thread::SpinLock::LockGuard guard(inflation_lock_);

    HeaderWord orig = obj->header;

    if(orig.f.inflated) {
      return false;
    }

    InflatedHeader* header = inflated_headers_->allocate(obj);
    header->update(state, orig);
    header->initialize_mutex(state->vm()->thread_id(), count);

    while(!obj->set_inflated_header(state, header, orig)) {
      orig = obj->header;

      if(orig.f.inflated) {
        return false;
      }
      header->update(state, orig);
      header->initialize_mutex(state->vm()->thread_id(), count);
    }
    return true;
  }

  LockStatus ObjectMemory::contend_for_lock(STATE, GCToken gct, ObjectHeader* obj,
                                            size_t us, bool interrupt)
  {
    bool timed = false;
    bool timeout = false;
    struct timespec ts = {0,0};

    OnStack<1> os(state, obj);

    {
      GCLockGuard lg(state, gct, contention_lock_);

      // We want to lock obj, but someone else has it locked.
      //
      // If the lock is already inflated, no problem, just lock it!

      // Be sure obj is updated by the GC while we're waiting for it

step1:
      // Only contend if the header is thin locked.
      // Ok, the header is not inflated, but we can't inflate it and take
      // the lock because the locking thread needs to do that, so indicate
      // that the object is being contended for and then wait on the
      // contention condvar until the object is unlocked.

      HeaderWord orig = obj->header;
      orig.f.inflated = 0;

      HeaderWord new_val      = orig;
      orig.f.meaning          = eAuxWordLock;
      new_val.f.LockContended = 1;

      if(!obj->header.atomic_set(orig, new_val)) {
        if(obj->inflated_header_p()) {
          if(cDebugThreading) {
            std::cerr << "[LOCK " << state->vm()->thread_id()
              << " contend_for_lock error: object has been inflated.]" << std::endl;
          }
          return eLockError;
        }
        if(new_val.f.meaning != eAuxWordLock) {
          if(cDebugThreading) {
            std::cerr << "[LOCK " << state->vm()->thread_id()
              << " contend_for_lock error: not thin locked.]" << std::endl;
          }
          return eLockError;
        }

        // Something changed since we started to down this path,
        // start over.
        goto step1;
      }

      // Ok, we've registered the lock contention, now spin and wait
      // for the us to be told to retry.

      if(cDebugThreading) {
        std::cerr << "[LOCK " << state->vm()->thread_id() << " waiting on contention]" << std::endl;
      }

      if(us > 0) {
        timed = true;
        struct timeval tv;
        gettimeofday(&tv, NULL);

        ts.tv_sec = tv.tv_sec + (us / 1000000);
        ts.tv_nsec = (us % 1000000) * 1000;
      }

      state->vm()->set_sleeping();

      while(!obj->inflated_header_p()) {
        GCIndependent gc_guard(state);

        if(timed) {
          timeout = (contention_var_.wait_until(contention_lock_, &ts) == utilities::thread::cTimedOut);
          if(timeout) break;
        } else {
          contention_var_.wait(contention_lock_);
        }

        if(cDebugThreading) {
          std::cerr << "[LOCK " << state->vm()->thread_id() << " notified of contention breakage]" << std::endl;
        }

        // Someone is interrupting us trying to lock.
        if(interrupt && state->vm()->check_local_interrupts) {
          state->vm()->check_local_interrupts = false;

          if(!state->vm()->interrupted_exception()->nil_p()) {
            if(cDebugThreading) {
              std::cerr << "[LOCK " << state->vm()->thread_id() << " detected interrupt]" << std::endl;
            }

            state->vm()->clear_sleeping();
            return eLockInterrupted;
          }
        }
      }

      state->vm()->clear_sleeping();

      if(cDebugThreading) {
        std::cerr << "[LOCK " << state->vm()->thread_id() << " contention broken]" << std::endl;
      }

      if(timeout) {
        if(cDebugThreading) {
          std::cerr << "[LOCK " << state->vm()->thread_id() << " contention timed out]" << std::endl;
        }

        return eLockTimeout;
      }
    } // contention_lock_ guard

    // We lock the InflatedHeader here rather than returning
    // and letting ObjectHeader::lock because the GC might have run
    // and we've used OnStack<> specificly to deal with that.
    //
    // ObjectHeader::lock doesn't use OnStack<>, it just is sure to
    // not access this if there is chance that a call blocked and GC'd
    // (which is true in the case of this function).

    InflatedHeader* ih = obj->inflated_header();

    if(timed) {
      return ih->lock_mutex_timed(state, gct, &ts, interrupt);
    } else {
      return ih->lock_mutex(state, gct, 0, interrupt);
    }
  }

  void ObjectMemory::release_contention(STATE, GCToken gct) {
    GCLockGuard lg(state, gct, contention_lock_);
    contention_var_.broadcast();
  }

  bool ObjectMemory::inflate_and_lock(STATE, ObjectHeader* obj) {
    utilities::thread::SpinLock::LockGuard guard(inflation_lock_);

    InflatedHeader* ih = 0;
    int initial_count = 0;

    HeaderWord orig = obj->header;
    HeaderWord new_val = orig;

    if(orig.f.inflated) {
      // Already inflated. ERROR, let the caller sort it out.
      return false;
    }

    switch(new_val.f.meaning) {
    case eAuxWordEmpty:
      // ERROR, we can not be here because it's empty. This is only to
      // be called when the header is already in use.
      return false;
    case eAuxWordObjID:
      // We could be have made a header before trying again, so
      // keep using the original one.
      ih = inflated_headers_->allocate(obj);
      ih->set_object_id(new_val.f.aux_word);
      break;
    case eAuxWordLock:
      // We have to locking the object to inflate it, thats the law.
      if(new_val.f.aux_word >> cAuxLockTIDShift != state->vm()->thread_id()) {
        return false;
      }

      ih = inflated_headers_->allocate(obj);
      initial_count = orig.f.aux_word & cAuxLockRecCountMask;
      break;
    case eAuxWordHandle:
      // Handle in use so inflate and update handle
      ih = inflated_headers_->allocate(obj);
      ih->set_handle(state, obj->handle(state));
      break;
    }

    ih->initialize_mutex(state->vm()->thread_id(), initial_count);

    new_val.all_flags = ih;
    new_val.f.inflated = 1;

    while(!obj->header.atomic_set(orig, new_val)) {
      // The header can't have been inflated by another thread, the
      // inflation process holds the OM lock.
      //
      // So some other bits must have changed, so lets just spin and
      // keep trying to update it.

      // Sanity check that the meaning is still the same, if not, then
      // something is really wrong.
      orig = obj->header;
      if(orig.f.inflated) {
        return false;
      }
      if(orig.f.meaning != new_val.f.meaning) {
        if(cDebugThreading) {
          std::cerr << "[LOCK object header consistence error detected.]" << std::endl;
        }
        return false;
      }

      new_val = orig;
      new_val.all_flags = ih;
      new_val.f.inflated = 1;
    }

    return true;
  }

  bool ObjectMemory::inflate_for_contention(STATE, ObjectHeader* obj) {
    utilities::thread::SpinLock::LockGuard guard(inflation_lock_);

    for(;;) {
      HeaderWord orig = obj->header;
      HeaderWord new_val = orig;

      InflatedHeader* ih = 0;

      if(orig.f.inflated) {
        if(cDebugThreading) {
          std::cerr << "[LOCK " << state->vm()->thread_id() << " asked to inflated already inflated lock]" << std::endl;
        }
        return false;
      }

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
      case eAuxWordHandle:
        ih = inflated_headers_->allocate(obj);
        ih->set_handle(state, obj->handle(state));
        break;
      case eAuxWordLock:
        // We have to be locking the object to inflate it, thats the law.
        if(new_val.f.aux_word >> cAuxLockTIDShift != state->vm()->thread_id()) {
          if(cDebugThreading) {
            std::cerr << "[LOCK " << state->vm()->thread_id() << " object locked by another thread while inflating for contention]" << std::endl;
          }
          return false;
        }
        if(cDebugThreading) {
          std::cerr << "[LOCK " << state->vm()->thread_id() << " being unlocked and inflated atomicly]" << std::endl;
        }

        ih = inflated_headers_->allocate(obj);
        break;
      }

      new_val.all_flags = ih;
      new_val.f.inflated = 1;

      // Try it all over again if it fails.
      if(!obj->header.atomic_set(orig, new_val)) continue;

      obj->clear_lock_contended();

      if(cDebugThreading) {
        std::cerr << "[LOCK " << state->vm()->thread_id() << " inflated lock for contention.]" << std::endl;
      }

      // Now inflated but not locked, which is what we want.
      return true;
    }
  }

  bool ObjectMemory::refill_slab(STATE, gc::Slab& slab) {
    utilities::thread::SpinLock::LockGuard guard(allocation_lock_);

    Address addr = young_->allocate_for_slab(slab_size_);

    gc_stats.slab_allocated(slab.allocations(), slab.byte_used());

    if(addr) {
      slab.refill(addr, slab_size_);
      return true;
    } else {
      slab.refill(0, 0);
      return false;
    }
  }

  void ObjectMemory::set_young_lifetime(size_t age) {
    SYNC_TL;

    young_->set_lifetime(age);
  }

  void ObjectMemory::debug_marksweep(bool val) {
    SYNC_TL;

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

    size_t sz = obj->size_in_bytes(root_state_);

    Object* copy = immix_->move_object(obj, sz);

    gc_stats.promoted_object_allocated(sz);
    if(unlikely(!copy)) {
      copy = mark_sweep_->move_object(obj, sz, &collect_mature_now);
    }

#ifdef ENABLE_OBJECT_WATCH
    if(watched_p(obj)) {
      std::cout << "detected object " << obj << " during promotion.\n";
    }
#endif

    return copy;
  }

  void ObjectMemory::collect_maybe(STATE, GCToken gct, CallFrame* call_frame) {
    // Don't go any further unless we're allowed to GC.
    if(!can_gc()) return;

    while(!state->stop_the_world()) {
      state->checkpoint(gct, call_frame);

      // Someone else got to the GC before we did! No problem, all done!
      if(!collect_young_now && !collect_mature_now) return;
    }

    // Ok, everyone in stopped! LET'S GC!

    SYNC(state);

    if(cDebugThreading) {
      std::cerr << std::endl << "[" << state
                << " WORLD beginning GC.]" << std::endl;
    }

    GCData gc_data(state->vm(), gct);

    uint64_t start_time = 0;

    if(collect_young_now) {
      if(state->shared().config.gc_show) {
        start_time = get_current_time();
      }

      YoungCollectStats stats;

#ifdef RBX_PROFILER
      if(unlikely(state->vm()->tooling())) {
        tooling::GCEntry method(state, tooling::GCYoung);
        collect_young(gc_data, &stats);
      } else {
        collect_young(gc_data, &stats);
      }
#else
      collect_young(gc_data, &stats);
#endif

      if(state->shared().config.gc_show) {
        uint64_t fin_time = get_current_time();
        int diff = (fin_time - start_time) / 1000000;

        std::cerr << "[GC " << std::fixed << std::setprecision(1) << stats.percentage_used << "% "
                  << stats.promoted_objects << "/" << stats.excess_objects << " "
                  << stats.lifetime << " " << diff << "ms]" << std::endl;

        if(state->shared().config.gc_noisy) {
          std::cerr << "\a" << std::flush;
        }
      }
    }

    if(collect_mature_now) {
      size_t before_kb = 0;

      if(state->shared().config.gc_show) {
        start_time = get_current_time();
        before_kb = mature_bytes_allocated() / 1024;
      }

#ifdef RBX_PROFILER
      if(unlikely(state->vm()->tooling())) {
        tooling::GCEntry method(state, tooling::GCMature);
        collect_mature(gc_data);
      } else {
        collect_mature(gc_data);
      }
#else
      collect_mature(gc_data);
#endif

      if(state->shared().config.gc_show) {
        uint64_t fin_time = get_current_time();
        int diff = (fin_time - start_time) / 1000000;
        size_t kb = mature_bytes_allocated() / 1024;
        std::cerr << "[Full GC " << before_kb << "kB => " << kb << "kB " << diff << "ms]" << std::endl;

        if(state->shared().config.gc_noisy) {
          std::cerr << "\a\a" << std::flush;
        }
      }
    }

    state->restart_world();
    bool added = added_finalizers_;
    added_finalizers_ = false;

    UNSYNC;

    if(added) {
      if(finalizer_thread_.get() == cNil) {
        start_finalizer_thread(state);
      }
      finalizer_var_.signal();
    }
  }

  void ObjectMemory::collect_young(GCData& data, YoungCollectStats* stats) {
    collect_young_now = false;

    timer::Running<1000000> timer(gc_stats.total_young_collection_time,
                                  gc_stats.last_young_collection_time);

    young_->reset_stats();

    young_->collect(data, stats);

    prune_handles(data.handles(), data.cached_handles(), young_);
    gc_stats.young_collection_count++;

    data.global_cache()->prune_young();

    if(data.threads()) {
      for(std::list<ManagedThread*>::iterator i = data.threads()->begin();
          i != data.threads()->end();
          ++i) {
        gc::Slab& slab = (*i)->local_slab();

        gc_stats.slab_allocated(slab.allocations(), slab.byte_used());

        // Reset the slab to a size of 0 so that the thread has to do
        // an allocation to get a proper refill. This keeps the number
        // of threads in the system from starving the available
        // number of slabs.
        slab.refill(0, 0);
      }
    }

  }

  void ObjectMemory::collect_mature(GCData& data) {

    timer::Running<1000000> timer(gc_stats.total_full_collection_time,
                                  gc_stats.last_full_collection_time);

    collect_mature_now = false;

    code_manager_.clear_marks();

    immix_->reset_stats();

    immix_->collect(data);

    immix_->clean_weakrefs();

    code_manager_.sweep();

    data.global_cache()->prune_unmarked(mark());

    prune_handles(data.handles(), data.cached_handles(), NULL);


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
    gc_stats.full_collection_count++;

  }

  void ObjectMemory::inflate_for_id(STATE, ObjectHeader* obj, uint32_t id) {
    utilities::thread::SpinLock::LockGuard guard(inflation_lock_);

    HeaderWord orig = obj->header;

    if(orig.f.inflated) {
      obj->inflated_header()->set_object_id(id);
      return;
    }

    InflatedHeader* header = inflated_headers_->allocate(obj);
    header->update(state, orig);
    header->set_object_id(id);

    while(!obj->set_inflated_header(state, header, orig)) {
      orig = obj->header;

      if(orig.f.inflated) {
        obj->inflated_header()->set_object_id(id);
        return;
      }
      header->update(state, orig);
      header->set_object_id(id);
    }

  }

  void ObjectMemory::inflate_for_handle(STATE, ObjectHeader* obj, capi::Handle* handle) {
    utilities::thread::SpinLock::LockGuard guard(inflation_lock_);

    HeaderWord orig = obj->header;

    if(orig.f.inflated) {
      obj->inflated_header()->set_handle(state, handle);
      return;
    }

    InflatedHeader* header = inflated_headers_->allocate(obj);
    header->update(state, orig);
    header->set_handle(state, handle);

    while(!obj->set_inflated_header(state, header, orig)) {
      orig = obj->header;

      if(orig.f.inflated) {
        obj->inflated_header()->set_handle(state, handle);
        return;
      }
      header->update(state, orig);
      header->set_handle(state, handle);
    }

  }

  void ObjectMemory::prune_handles(capi::Handles* handles, std::list<capi::Handle*>* cached, BakerGC* young) {
    handles->deallocate_handles(cached, mark(), young);
  }

  size_t ObjectMemory::mature_bytes_allocated() {
    return immix_->bytes_allocated() + mark_sweep_->allocated_bytes;
  }

  void ObjectMemory::add_type_info(TypeInfo* ti) {
    SYNC_TL;

    if(TypeInfo* current = type_info[ti->type]) {
      delete current;
    }
    type_info[ti->type] = ti;
  }

  Object* ObjectMemory::allocate_object(size_t bytes) {

    Object* obj;

    if(unlikely(bytes > large_object_threshold)) {
      obj = mark_sweep_->allocate(bytes, &collect_mature_now);
      if(unlikely(!obj)) return NULL;

      gc_stats.mature_object_allocated(bytes);

      if(collect_mature_now) shared_.gc_soon();

    } else {
      obj = young_->allocate(bytes, &collect_young_now);
      if(unlikely(obj == NULL)) {
        collect_young_now = true;
        shared_.gc_soon();

        obj = immix_->allocate(bytes);

        if(unlikely(!obj)) {
          obj = mark_sweep_->allocate(bytes, &collect_mature_now);
        }

        gc_stats.mature_object_allocated(bytes);

        if(collect_mature_now) shared_.gc_soon();
      } else {
        gc_stats.young_object_allocated(bytes);
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
      obj = mark_sweep_->allocate(bytes, &collect_mature_now);
      if(unlikely(!obj)) return NULL;
    } else {
      obj = immix_->allocate(bytes);

      if(unlikely(!obj)) {
        obj = mark_sweep_->allocate(bytes, &collect_mature_now);
      }

      gc_stats.mature_object_allocated(bytes);
    }

    if(collect_mature_now) shared_.gc_soon();

#ifdef ENABLE_OBJECT_WATCH
    if(watched_p(obj)) {
      std::cout << "detected " << obj << " during mature allocation\n";
    }
#endif

    obj->clear_fields(bytes);
    return obj;
  }

  Object* ObjectMemory::new_object_typed(STATE, Class* cls, size_t bytes, object_type type) {
    utilities::thread::SpinLock::LockGuard guard(allocation_lock_);

    Object* obj;

    obj = allocate_object(bytes);
    if(unlikely(!obj)) return NULL;

    obj->klass(this, cls);

    obj->set_obj_type(type);

    return obj;
  }

  Object* ObjectMemory::new_object_typed_mature(STATE, Class* cls, size_t bytes, object_type type) {
    utilities::thread::SpinLock::LockGuard guard(allocation_lock_);

    Object* obj;

    obj = allocate_object_mature(bytes);
    if(unlikely(!obj)) return NULL;

    obj->klass(this, cls);

    obj->set_obj_type(type);

    return obj;
  }

  /* ONLY use to create Class, the first object. */
  Object* ObjectMemory::allocate_object_raw(size_t bytes) {

    Object* obj = mark_sweep_->allocate(bytes, &collect_mature_now);
    gc_stats.mature_object_allocated(bytes);
    obj->clear_fields(bytes);
    return obj;
  }

  Object* ObjectMemory::new_object_typed_enduring(STATE, Class* cls, size_t bytes, object_type type) {
    utilities::thread::SpinLock::LockGuard guard(allocation_lock_);

    Object* obj = mark_sweep_->allocate(bytes, &collect_mature_now);
    gc_stats.mature_object_allocated(bytes);

    if(collect_mature_now) shared_.gc_soon();

#ifdef ENABLE_OBJECT_WATCH
    if(watched_p(obj)) {
      std::cout << "detected " << obj << " during enduring allocation\n";
    }
#endif

    obj->clear_fields(bytes);

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
    SYNC_TL;

    code_manager_.add_resource(cr);
  }

  void* ObjectMemory::young_start() {
    return young_->start_address();
  }

  void* ObjectMemory::yound_end() {
    return young_->last_address();
  }

  void ObjectMemory::needs_finalization(Object* obj, FinalizerFunction func) {
    SCOPE_LOCK(ManagedThread::current(), finalizer_lock_);

    FinalizeObject fi;
    fi.object = obj;
    fi.status = FinalizeObject::eLive;
    fi.finalizer = func;

    // Makes a copy of fi.
    finalize_.push_back(fi);
  }

  void ObjectMemory::set_ruby_finalizer(Object* obj, Object* fin) {
    SCOPE_LOCK(ManagedThread::current(), finalizer_lock_);

    // See if there already one.
    for(std::list<FinalizeObject>::iterator i = finalize_.begin();
        i != finalize_.end(); ++i)
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

    // Adding a nil finalizer is only used to delete an
    // existing finalizer, which we apparently don't have
    // if we get here.
    if(fin->nil_p()) {
      return;
    }

    // Ok, create it.

    FinalizeObject fi;
    fi.object = obj;
    fi.status = FinalizeObject::eLive;

    // Rubinius specific API. If the finalizer is the object, we're going to send
    // the object __finalize__. We mark that the user wants this by putting cTrue
    // as the ruby_finalizer.
    if(obj == fin) {
      fi.ruby_finalizer = cTrue;
    } else {
      fi.ruby_finalizer = fin;
    }

    // Makes a copy of fi.
    finalize_.push_back(fi);
  }

  void ObjectMemory::add_to_finalize(FinalizeObject* fi) {
    SCOPE_LOCK(ManagedThread::current(), finalizer_lock_);
    to_finalize_.push_back(fi);
    added_finalizers_ = true;
  }

  void ObjectMemory::in_finalizer_thread(STATE) {
    CallFrame* call_frame = 0;
    utilities::thread::Thread::set_os_name("rbx.finalizer");

    GCTokenImpl gct;
    state->vm()->thread->hard_unlock(state, gct);

    // Forever
    for(;;) {
      FinalizeObject* fi;

      // Take the lock, remove the first one from the list,
      // then process it.
      {
        SCOPE_LOCK(state->vm(), finalizer_lock_);

        state->vm()->set_call_frame(0);

        while(to_finalize_.empty()) {
          GCIndependent indy(state);
          finalizer_var_.wait(finalizer_lock_);
        }

        fi = to_finalize_.front();
        to_finalize_.pop_front();
      }

      if(fi->finalizer) {
        (*fi->finalizer)(state, fi->object);
        // Unhook any handle used by fi->object so that we don't accidentally
        // try and mark it later (after we've finalized it)
        if(capi::Handle* handle = fi->object->handle(state)) {
          handle->forget_object();
          fi->object->clear_handle(state);
        }

        // If the object was remembered, unremember it.
        if(fi->object->remembered_p()) {
          unremember_object(fi->object);
        }
      } else if(fi->ruby_finalizer) {
        // Rubinius specific code. If the finalizer is cTrue, then
        // send the object the finalize message
        if(fi->ruby_finalizer == cTrue) {
          fi->object->send(state, call_frame, state->symbol("__finalize__"));
        } else {
          Array* ary = Array::create(state, 1);
          ary->set(state, 0, fi->object->id(state));

          OnStack<1> os(state, ary);

          fi->ruby_finalizer->send(state, call_frame, G(sym_call), ary);
        }
      } else {
        std::cerr << "Unsupported object to be finalized: "
                  << fi->object->to_s(state)->c_str(state) << std::endl;
      }

      fi->status = FinalizeObject::eFinalized;
    }

    state->checkpoint(gct, 0);
  }

  void ObjectMemory::run_finalizers(STATE, CallFrame* call_frame) {
    {
      SCOPE_LOCK(state->vm(), finalizer_lock_);
      if(running_finalizers_) return;
      running_finalizers_ = true;
    }

    for(std::list<FinalizeObject*>::iterator i = to_finalize_.begin();
        i != to_finalize_.end(); ) {
      FinalizeObject* fi = *i;

      if(fi->finalizer) {
        (*fi->finalizer)(state, fi->object);
        // Unhook any handle used by fi->object so that we don't accidentally
        // try and mark it later (after we've finalized it)
        if(capi::Handle* handle = fi->object->handle(state)) {
          handle->forget_object();
          fi->object->clear_handle(state);
        }

        // If the object was remembered, unremember it.
        if(fi->object->remembered_p()) {
          unremember_object(fi->object);
        }
      } else if(fi->ruby_finalizer) {
        // Rubinius specific code. If the finalizer is cTrue, then
        // send the object the finalize message
        if(fi->ruby_finalizer == cTrue) {
          fi->object->send(state, call_frame, state->symbol("__finalize__"));
        } else {
          Array* ary = Array::create(state, 1);
          ary->set(state, 0, fi->object->id(state));

          OnStack<1> os(state, ary);

          fi->ruby_finalizer->send(state, call_frame, G(sym_call), ary);
        }
      } else {
        std::cerr << "Unsupported object to be finalized: "
                  << fi->object->to_s(state)->c_str(state) << std::endl;
      }

      fi->status = FinalizeObject::eFinalized;

      i = to_finalize_.erase(i);
    }

    running_finalizers_ = false;
  }

  void ObjectMemory::run_all_finalizers(STATE) {
    {
      SCOPE_LOCK(state->vm(), finalizer_lock_);
      if(running_finalizers_) return;
      running_finalizers_ = true;
    }

    for(std::list<FinalizeObject>::iterator i = finalize_.begin();
        i != finalize_.end(); )
    {
      FinalizeObject& fi = *i;

      // Only finalize things that haven't been finalized.
      if(fi.status != FinalizeObject::eFinalized) {
        if(fi.finalizer) {
          (*fi.finalizer)(state, fi.object);
        } else if(fi.ruby_finalizer) {
          // Rubinius specific code. If the finalizer is cTrue, then
          // send the object the finalize message
          if(fi.ruby_finalizer == cTrue) {
            fi.object->send(state, 0, state->symbol("__finalize__"));
          } else {
            Array* ary = Array::create(state, 1);
            ary->set(state, 0, fi.object->id(state));

            OnStack<1> os(state, ary);

            fi.ruby_finalizer->send(state, 0, G(sym_call), ary);
          }
        } else {
          std::cerr << "During shutdown, unsupported object to be finalized: "
                    << fi.object->to_s(state)->c_str(state) << std::endl;
        }
      }

      fi.status = FinalizeObject::eFinalized;

      i = finalize_.erase(i);
    }

    running_finalizers_ = false;
  }

  void ObjectMemory::run_all_io_finalizers(STATE) {
    {
      SCOPE_LOCK(state->vm(), finalizer_lock_);
      if(running_finalizers_) return;
      running_finalizers_ = true;
    }

    for(std::list<FinalizeObject>::iterator i = finalize_.begin();
        i != finalize_.end(); )
    {
      FinalizeObject& fi = *i;

      if(!kind_of<IO>(fi.object)) { ++i; continue; }

      // Only finalize things that haven't been finalized.
      if(fi.status != FinalizeObject::eFinalized) {
        if(fi.finalizer) {
          (*fi.finalizer)(state, fi.object);
        } else if(fi.ruby_finalizer) {
          // Rubinius specific code. If the finalizer is cTrue, then
          // send the object the finalize message
          if(fi.ruby_finalizer == cTrue) {
            fi.object->send(state, 0, state->symbol("__finalize__"));
          } else {
            Array* ary = Array::create(state, 1);
            ary->set(state, 0, fi.object->id(state));

            OnStack<1> os(state, ary);

            fi.ruby_finalizer->send(state, 0, G(sym_call), ary);
          }
        } else {
          std::cerr << "During shutdown, unsupported object to be finalized: "
                    << fi.object->to_s(state)->c_str(state) << std::endl;
        }
      }

      fi.status = FinalizeObject::eFinalized;

      i = finalize_.erase(i);
    }

    running_finalizers_ = false;
  }

  Object* in_finalizer(STATE) {
    state->shared().om->in_finalizer_thread(state);
    return cNil;
  }

  void ObjectMemory::start_finalizer_thread(STATE) {
    VM* vm = state->shared().new_vm();
    Thread* thr = Thread::create(state, vm, G(thread), in_finalizer, false, true);
    finalizer_thread_.set(thr);
    thr->fork(state);
  }

  size_t& ObjectMemory::loe_usage() {
    return mark_sweep_->allocated_bytes;
  }

  size_t& ObjectMemory::immix_usage() {
    return immix_->bytes_allocated();
  }

  size_t& ObjectMemory::code_usage() {
    return code_manager_.size();
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
    ObjectMemory::GCInhibit inhibitor(this);

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

  void ObjectMemory::snapshot(STATE) {
    // Assign all objects an object id...
    ObjectMemory::GCInhibit inhibitor(root_state_->om);

    // Walk the heap over and over until we don't create
    // any more objects...

    size_t last_seen = 0;

    while(last_object_id != last_seen) {
      last_seen = last_object_id;

      ObjectWalker walker(root_state_->om);
      GCData gc_data(state->vm());

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

  void ObjectMemory::print_new_since_snapshot(STATE) {
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
      if(!obj->has_id(state) || obj->id(state)->to_native() > check_id) {
        count++;
        bytes += obj->size_in_bytes(state->vm());

        if(kind_of<String>(obj)) {
          std::cout << "#<String:" << obj << ">\n";
        } else {
          std::cout << obj->to_s(state, true)->c_str(state) << "\n";
        }
      }

      obj = walker.next();
    }

    std::cout << count << " objects since snapshot.\n";
    std::cout << bytes << " bytes since snapshot.\n";
  }

  void ObjectMemory::print_references(STATE, Object* obj) {
    ObjectArray ary;

    find_referers(obj, ary);

    int count = 0;

    std::cout << ary.size() << " total references:\n";
    for(ObjectArray::iterator i = ary.begin();
        i != ary.end();
        ++i) {
      std::cout << "  " << (*i)->to_s(state, true)->c_str(state) << "\n";

      if(++count == 100) break;
    }
  }
};


// The following memory functions are defined in ruby.h for use by C-API
// extensions, and also used by library code lifted from MRI (e.g. Oniguruma).
// They provide some book-keeping around memory usage for non-VM code, so that
// the garbage collector is run periodically in response to memory allocations
// in non-VM code.
// Without these  checks, memory can become exhausted without the VM being aware
// there is a problem. As this memory may only be being used by Ruby objects
// that have become garbage, performing a garbage collection periodically after
// a significant amount of memory has been malloc-ed should keep non-VM memory
// usage from growing uncontrollably.


void* XMALLOC(size_t bytes) {
  bytes_until_collection -= bytes;
  if(bytes_until_collection <= 0) {
    rubinius::VM::current()->run_gc_soon();
    bytes_until_collection = gc_malloc_threshold;
  }
  return malloc(bytes);
}

void XFREE(void* ptr) {
  free(ptr);
}

void* XREALLOC(void* ptr, size_t bytes) {
  bytes_until_collection -= bytes;
  if(bytes_until_collection <= 0) {
    rubinius::VM::current()->run_gc_soon();
    bytes_until_collection = gc_malloc_threshold;
  }

  return realloc(ptr, bytes);
}

void* XCALLOC(size_t items, size_t bytes_per) {
  size_t bytes = bytes_per * items;

  bytes_until_collection -= bytes;
  if(bytes_until_collection <= 0) {
    rubinius::VM::current()->run_gc_soon();
    bytes_until_collection = gc_malloc_threshold;
  }

  return calloc(items, bytes_per);
}
