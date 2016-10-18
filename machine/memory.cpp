#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <sys/time.h>

#include "config.h"
#include "vm.hpp"
#include "state.hpp"
#include "memory.hpp"
#include "thread_phase.hpp"

#include "capi/tag.hpp"

#include "memory/mark_sweep.hpp"
#include "memory/immix_collector.hpp"
#include "memory/inflated_headers.hpp"
#include "memory/walker.hpp"

#include "environment.hpp"

#include "on_stack.hpp"

#include "config_parser.hpp"

#include "class/class.hpp"
#include "class/fixnum.hpp"
#include "class/tuple.hpp"
#include "class/io.hpp"
#include "class/fiber.hpp"
#include "class/string.hpp"
#include "class/lookup_table.hpp"
#include "class/ffi_pointer.hpp"
#include "class/data.hpp"
#include "class/dir.hpp"
#include "class/array.hpp"
#include "class/thread.hpp"
#include "class/exception.hpp"

#include "capi/handles.hpp"
#include "configuration.hpp"

#include "global_cache.hpp"

#include "instruments/timing.hpp"
#include "dtrace/dtrace.h"

#include "logger.hpp"

namespace rubinius {
  void Memory::memory_error(STATE) {
    Exception::raise_memory_error(state);
  }

  Object* object_watch = 0;

  /* Memory methods */
  Memory::Memory(VM* vm, SharedState& shared)
    /* : young_(new BakerGC(this, shared.config)) */
    : mark_sweep_(new memory::MarkSweepGC(this, shared.config))
    , immix_(new memory::ImmixGC(this))
    , immix_marker_(NULL)
    , inflated_headers_(new memory::InflatedHeaders)
    , capi_handles_(new capi::Handles)
    , code_manager_(&vm->shared)
    , cycle_(0)
    , mark_(2)
    , allow_gc_(true)
    , mature_mark_concurrent_(shared.config.gc_immix_concurrent)
    , mature_gc_in_progress_(false)
    , slab_size_(4096)
    , interrupt_flag_(false)
    , collect_young_flag_(false)
    , collect_full_flag_(false)
    , shared_(vm->shared)
    , vm_(vm)
    , last_object_id(1)
    , last_snapshot_id(0)
    , large_object_threshold(shared.config.gc_large_object)
  {
    // TODO Not sure where this code should be...
#ifdef ENABLE_OBJECT_WATCH
    if(char* num = getenv("RBX_WATCH")) {
      object_watch = reinterpret_cast<Object*>(strtol(num, NULL, 10));
      std::cout << "Watching for " << object_watch << "\n";
    }
#endif

    for(size_t i = 0; i < LastObjectType; i++) {
      type_info[i] = NULL;
    }

    TypeInfo::init(this);
  }

  Memory::~Memory() {
    mark_sweep_->free_objects();

    // @todo free immix data

    for(size_t i = 0; i < LastObjectType; i++) {
      if(type_info[i]) delete type_info[i];
    }

    delete immix_;
    delete mark_sweep_;
    /* delete young_; */

    for(std::list<capi::GlobalHandle*>::iterator i = global_capi_handle_locations_.begin();
          i != global_capi_handle_locations_.end(); ++i) {
      delete *i;
    }
    global_capi_handle_locations_.clear();

    delete capi_handles_;

    // Must be last
    delete inflated_headers_;
  }

  void Memory::after_fork_child(STATE) {
    contention_lock_.init();
    mature_gc_in_progress_ = false;
    vm_ = state->vm();
  }

  void Memory::assign_object_id(STATE, Object* obj) {
    // Double check we've got no id still after the lock.
    if(obj->object_id(state) > 0) return;

    obj->set_object_id(state, atomic::fetch_and_add(&last_object_id, (size_t)1));
  }

  bool Memory::inflate_lock_count_overflow(STATE, ObjectHeader* obj,
                                                 int count)
  {
    utilities::thread::SpinLock::LockGuard guard(inflation_lock_);

    HeaderWord orig = obj->header;

    if(orig.f.meaning == eAuxWordInflated) {
      return false;
    }

    uint32_t ih_header = 0;
    InflatedHeader* ih = inflated_headers_->allocate(state, obj, &ih_header);
    ih->update(state, orig);
    ih->initialize_mutex(state->vm()->thread_id(), count);
    ih->mark(this, mark_);

    while(!obj->set_inflated_header(state, ih_header, orig)) {
      orig = obj->header;

      if(orig.f.meaning == eAuxWordInflated) {
        return false;
      }
      ih->update(state, orig);
      ih->initialize_mutex(state->vm()->thread_id(), count);
    }
    return true;
  }

  LockStatus Memory::contend_for_lock(STATE,
                                            ObjectHeader* obj, size_t us, bool interrupt)
  {
    bool timed = false;
    bool timeout = false;
    struct timespec ts = {0,0};

    OnStack<1> os(state, obj);

    {
      MutexLockWaiting lock_waiting(state, contention_lock_);

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

      HeaderWord orig         = obj->header;
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

      while(!obj->inflated_header_p()) {
        UnmanagedPhase unmanaged(state);

        state->vm()->set_sleeping();
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
        if(interrupt && state->check_local_interrupts()) {
          state->vm()->clear_check_local_interrupts();

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

    InflatedHeader* ih = obj->inflated_header(state);

    if(timed) {
      return ih->lock_mutex_timed(state, obj, &ts, interrupt);
    } else {
      return ih->lock_mutex(state, obj, 0, interrupt);
    }
  }

  void Memory::release_contention(STATE) {
    MutexLockWaiting lock_waiting(state, contention_lock_);
    contention_var_.broadcast();
  }

  bool Memory::inflate_and_lock(STATE, ObjectHeader* obj) {
    utilities::thread::SpinLock::LockGuard guard(inflation_lock_);

    InflatedHeader* ih = 0;
    uint32_t ih_index = 0;
    int initial_count = 0;

    HeaderWord orig = obj->header;

    switch(orig.f.meaning) {
    case eAuxWordEmpty:
      // ERROR, we can not be here because it's empty. This is only to
      // be called when the header is already in use.
      return false;
    case eAuxWordObjID:
      // We could be have made a header before trying again, so
      // keep using the original one.
      ih = inflated_headers_->allocate(state, obj, &ih_index);
      ih->set_object_id(orig.f.aux_word);
      break;
    case eAuxWordLock:
      // We have to locking the object to inflate it, thats the law.
      if(orig.f.aux_word >> cAuxLockTIDShift != state->vm()->thread_id()) {
        return false;
      }

      ih = inflated_headers_->allocate(state, obj, &ih_index);
      initial_count = orig.f.aux_word & cAuxLockRecCountMask;
      break;
    case eAuxWordHandle:
      // Handle in use so inflate and update handle
      ih = inflated_headers_->allocate(state, obj, &ih_index);
      ih->set_handle(state, obj->handle(state));
      break;
    case eAuxWordInflated:
      // Already inflated. ERROR, let the caller sort it out.
      if(cDebugThreading) {
        std::cerr << "[LOCK " << state->vm()->thread_id() << " asked to inflated already inflated lock]" << std::endl;
      }
      return false;
    }

    ih->initialize_mutex(state->vm()->thread_id(), initial_count);
    ih->mark(this, mark_);

    while(!obj->set_inflated_header(state, ih_index, orig)) {
      // The header can't have been inflated by another thread, the
      // inflation process holds the OM lock.
      //
      // So some other bits must have changed, so lets just spin and
      // keep trying to update it.

      // Sanity check that the meaning is still the same, if not, then
      // something is really wrong.
      if(orig.f.meaning != obj->header.f.meaning) {
        if(cDebugThreading) {
          std::cerr << "[LOCK object header consistence error detected.]" << std::endl;
        }
        return false;
      }
      orig = obj->header;
      if(orig.f.meaning == eAuxWordInflated) {
        return false;
      }
    }

    return true;
  }

  bool Memory::inflate_for_contention(STATE, ObjectHeader* obj) {
    utilities::thread::SpinLock::LockGuard guard(inflation_lock_);

    for(;;) {
      HeaderWord orig = obj->header;

      InflatedHeader* ih = 0;
      uint32_t ih_header = 0;

      switch(orig.f.meaning) {
      case eAuxWordEmpty:
        ih = inflated_headers_->allocate(state, obj, &ih_header);
        break;
      case eAuxWordObjID:
        // We could be have made a header before trying again, so
        // keep using the original one.
        ih = inflated_headers_->allocate(state, obj, &ih_header);
        ih->set_object_id(orig.f.aux_word);
        break;
      case eAuxWordHandle:
        ih = inflated_headers_->allocate(state, obj, &ih_header);
        ih->set_handle(state, obj->handle(state));
        break;
      case eAuxWordLock:
        // We have to be locking the object to inflate it, thats the law.
        if(orig.f.aux_word >> cAuxLockTIDShift != state->vm()->thread_id()) {
          if(cDebugThreading) {
            std::cerr << "[LOCK " << state->vm()->thread_id() << " object locked by another thread while inflating for contention]" << std::endl;
          }
          return false;
        }
        if(cDebugThreading) {
          std::cerr << "[LOCK " << state->vm()->thread_id() << " being unlocked and inflated atomicly]" << std::endl;
        }

        ih = inflated_headers_->allocate(state, obj, &ih_header);
        break;
      case eAuxWordInflated:
        if(cDebugThreading) {
          std::cerr << "[LOCK " << state->vm()->thread_id() << " asked to inflated already inflated lock]" << std::endl;
        }
        return false;
      }

      ih->mark(this, mark_);

      // Try it all over again if it fails.
      if(!obj->set_inflated_header(state, ih_header, orig)) {
        ih->clear();
        continue;
      }

      obj->clear_lock_contended();

      if(cDebugThreading) {
        std::cerr << "[LOCK " << state->vm()->thread_id() << " inflated lock for contention.]" << std::endl;
      }

      // Now inflated but not locked, which is what we want.
      return true;
    }
  }

  bool Memory::refill_slab(STATE, memory::Slab& slab) {
    utilities::thread::SpinLock::LockGuard guard(allocation_lock_);

    memory::Address addr = memory::Address::null(); /* young_->allocate_for_slab(slab_size_); */

    metrics::MetricsData& metrics = state->vm()->metrics();
    metrics.memory.young_objects += slab.allocations();
    metrics.memory.young_bytes += slab.bytes_used();

    if(addr) {
      slab.refill(addr, slab_size_);
      metrics.memory.slab_refills++;
      return true;
    } else {
      slab.refill(0, 0);
      metrics.memory.slab_refills_fails++;
      return false;
    }
  }

  bool Memory::valid_object_p(Object* obj) {
    if(obj->true_p()) {
      return true;
    } else if(obj->false_p()) {
      return true;
    } else if(obj->nil_p()) {
      return true;
    } else if(obj->undef_p()) {
      return true;
    } else if(obj->fixnum_p()) {
      return true;
    } else if(obj->symbol_p()) {
      return true;
    } else if(obj->young_object_p()) {
      return false; /* young_->validate_object(obj) == cValid; */
    } else if(obj->mature_object_p()) {
      if(obj->in_immix_p()) {
        if(immix_->validate_object(obj) == cInImmix) {
          return true;
        }
      } else if(obj->large_object_p()) {
        if(mark_sweep_->validate_object(obj) == cMatureObject) {
          return true;
        }
      }
    }

    return false;
  }

  /* Garbage collection */

  Object* Memory::promote_object(Object* obj) {
    size_t sz = obj->size_in_bytes(vm());

    bool collect_flag = false;
    Object* copy = immix_->move_object(obj, sz, collect_flag);

    if(collect_flag) {
      schedule_full_collection("promote object");
    }

    vm()->metrics().memory.promoted_objects++;
    vm()->metrics().memory.promoted_bytes += sz;

    if(unlikely(!copy)) {
      collect_flag = false;
      copy = mark_sweep_->move_object(obj, sz, collect_flag);

      if(collect_flag) {
        schedule_full_collection("promote object to large space");
      }
    }

#ifdef ENABLE_OBJECT_WATCH
    if(watched_p(obj)) {
      std::cout << "detected object " << obj << " during promotion.\n";
    }
#endif

    copy->clear_mark();
    return copy;
  }

  void Memory::collect_maybe(STATE) {
    /* Don't go any further unless we're allowed to GC. We also reset the
     * flags so that we don't thrash constantly trying to GC. When the GC
     * prohibition lifts, a GC will eventually be triggered again.
     */
    if(!can_gc()) {
      collect_young_flag_ = false;
      collect_full_flag_ = false;
      interrupt_flag_ = false;
      return;
    }

    if(!collect_young_flag_ && !collect_full_flag_) return;

    if(cDebugThreading) {
      std::cerr << std::endl << "[" << state
                << " WORLD beginning GC.]" << std::endl;
    }

    if(collect_young_flag_) {
      memory::GCData gc_data(state->vm());

      RUBINIUS_GC_BEGIN(0);
      collect_young(state, &gc_data);

      if(!collect_full_flag_) interrupt_flag_ = false;

      RUBINIUS_GC_END(0);
    }

    if(collect_full_flag_) {
      RUBINIUS_GC_BEGIN(1);
      collect_full(state);
    }
  }

  void Memory::collect_young(STATE, memory::GCData* data) {
    timer::StopWatch<timer::milliseconds> timerx(
        state->vm()->metrics().gc.young_ms);

    /* 
    young_->collect(data);

    prune_handles(data->handles(), data->cached_handles(), young_);

    metrics::MetricsData& metrics = state->vm()->metrics();
    metrics.gc.young_count++;

    data->global_cache()->prune_young();

    {
      std::lock_guard<std::mutex> guard(data->thread_nexus()->threads_mutex());

      for(ThreadList::iterator i = data->thread_nexus()->threads()->begin();
          i != data->thread_nexus()->threads()->end();
          ++i)
      {
        memory::Slab& slab = (*i)->local_slab();

        // Reset the slab to a size of 0 so that the thread has to do
        // an allocation to get a proper refill. This keeps the number
        // of threads in the system from starving the available
        // number of slabs.
        slab.refill(0, 0);
      }
    }

    young_->reset();
#ifdef RBX_GC_DEBUG
    young_->verify(data);
#endif
    */

    collect_young_flag_ = false;
  }

  void Memory::collect_full(STATE) {
    // If we're already collecting, ignore this request
    if(mature_gc_in_progress_) return;

    timer::StopWatch<timer::milliseconds> timerx(
        state->vm()->metrics().gc.immix_stop_ms);

    if(state->shared().config.memory_collection_log.value) {
      logger::write("memory: full collection");
    }

    code_manager_.clear_marks();

    immix_->reset_stats();

    {
      memory::GCData data(state->vm());

      immix_->collect(&data);
      collect_full_finish(state, &data);
    }

    if(mature_mark_concurrent_) {
      memory::GCData* data = new memory::GCData(state->vm());

      mature_gc_in_progress_ = true;

      immix_->collect_start(data);
      immix_->start_marker(state, data);
    }
  }

  void Memory::collect_full_restart(STATE, memory::GCData* data) {
    if(state->shared().config.memory_collection_log.value) {
      logger::write("memory: concurrent collection restart");
    }

    code_manager_.clear_marks();

    immix_->reset_stats();

    immix_->collect_start(data);
  }

  void Memory::collect_full_finish(STATE, memory::GCData* data) {
    cycle_++;

    immix_->collect_finish(data);

    code_manager_.sweep();

    data->global_cache()->prune_unmarked(mark());

    prune_handles(data->handles(), data->cached_handles(), NULL);

    // Have to do this after all things that check for mark bits is
    // done, as it free()s objects, invalidating mark bits.
    mark_sweep_->after_marked();

    inflated_headers_->deallocate_headers(mark());

#ifdef RBX_GC_DEBUG
    immix_->verify(data);
#endif
    immix_->sweep();

    rotate_mark();

    metrics::MetricsData& metrics = state->vm()->metrics();
    metrics.gc.immix_count++;
    metrics.gc.large_count++;

    collect_full_flag_ = false;
    interrupt_flag_ = false;

    RUBINIUS_GC_END(1);
  }

  memory::MarkStack& Memory::mature_mark_stack() {
    return immix_->mark_stack();
  }

  void Memory::inflate_for_id(STATE, ObjectHeader* obj, uint32_t id) {
    utilities::thread::SpinLock::LockGuard guard(inflation_lock_);

    HeaderWord orig = obj->header;

    if(orig.f.meaning == eAuxWordInflated) {
      obj->inflated_header(state)->set_object_id(id);
      return;
    }

    uint32_t ih_index = 0;
    InflatedHeader* ih = inflated_headers_->allocate(state, obj, &ih_index);
    ih->update(state, orig);
    ih->set_object_id(id);
    ih->mark(this, mark_);

    while(!obj->set_inflated_header(state, ih_index, orig)) {
      orig = obj->header;

      if(orig.f.meaning == eAuxWordInflated) {
        obj->inflated_header(state)->set_object_id(id);
        ih->clear();
        return;
      }
      ih->update(state, orig);
      ih->set_object_id(id);
    }

  }

  void Memory::inflate_for_handle(STATE, ObjectHeader* obj, capi::Handle* handle) {
    utilities::thread::SpinLock::LockGuard guard(inflation_lock_);

    HeaderWord orig = obj->header;

    if(orig.f.meaning == eAuxWordInflated) {
      obj->inflated_header(state)->set_handle(state, handle);
      return;
    }

    uint32_t ih_index = 0;
    InflatedHeader* ih = inflated_headers_->allocate(state, obj, &ih_index);
    ih->update(state, orig);
    ih->set_handle(state, handle);
    ih->mark(this, mark_);

    while(!obj->set_inflated_header(state, ih_index, orig)) {
      orig = obj->header;

      if(orig.f.meaning == eAuxWordInflated) {
        obj->inflated_header(state)->set_handle(state, handle);
        ih->clear();
        return;
      }
      ih->update(state, orig);
      ih->set_handle(state, handle);
    }

  }

  void Memory::prune_handles(capi::Handles* handles, std::list<capi::Handle*>* cached, /* BakerGC */ void* young) {
    handles->deallocate_handles(cached, mark(), young);
  }

  void Memory::add_type_info(TypeInfo* ti) {
    utilities::thread::SpinLock::LockGuard guard(shared_.type_info_lock());

    if(TypeInfo* current = type_info[ti->type]) {
      delete current;
    }
    type_info[ti->type] = ti;
  }

  Object* Memory::new_object(STATE, native_int bytes) {
    utilities::thread::SpinLock::LockGuard guard(allocation_lock_);

    Object* obj = 0;
    bool collect_flag = false;

    // TODO: check if immix_ needs to trigger GC
    if(likely(obj = immix_->allocate(bytes, collect_flag))) {
      vm()->metrics().memory.immix_objects++;
      vm()->metrics().memory.immix_bytes += bytes;

      return obj;
    }

    if(collect_flag) {
      schedule_full_collection(
          "mature region allocate object",
          state->vm()->metrics().gc.immix_set);

      if(mature_mark_concurrent_) {
        /* Spilling allocations to the Large Object Space can be costly
         * because that region and collector are less efficient. To mitigate
         * spilling, we sleep for a very small random interval to allow the
         * concurrent marking thread to catch up and complete the GC cycle.
         */
        state->vm()->blocking_suspend(state, state->vm()->metrics().memory.suspend_ms);
      }
    }

    collect_flag = false;

    if(likely(obj = mark_sweep_->allocate(bytes, collect_flag))) {
      vm()->metrics().memory.large_objects++;
      vm()->metrics().memory.large_bytes += bytes;

      if(collect_flag) {
        schedule_full_collection(
            "large region allocate object",
            state->vm()->metrics().gc.large_set);
      }

      return obj;
    }

    Exception::raise_memory_error(state);
    return NULL;
  }

  TypeInfo* Memory::find_type_info(Object* obj) {
    return type_info[obj->type_id()];
  }

  ObjectPosition Memory::validate_object(Object* obj) {
    ObjectPosition pos;

    /*
    pos = young_->validate_object(obj);
    if(pos != cUnknown) return pos;
    */

    pos = immix_->validate_object(obj);
    if(pos != cUnknown) return pos;

    return mark_sweep_->validate_object(obj);
  }

  void Memory::add_code_resource(STATE, memory::CodeResource* cr) {
    utilities::thread::SpinLock::LockGuard guard(shared_.code_resource_lock());

    state->vm()->metrics().memory.code_bytes += cr->size();

    bool collect_flag = false;
    code_manager_.add_resource(cr, &collect_flag);

    if(collect_flag) {
      schedule_full_collection(
          "add code resource",
          state->vm()->metrics().gc.resource_set);
    }
  }

  capi::Handle* Memory::add_capi_handle(STATE, Object* obj) {
    if(!obj->reference_p()) {
      rubinius::bug("Trying to add a handle for a non reference");
    }
    state->vm()->metrics().memory.capi_handles++;
    uintptr_t handle_index = capi_handles_->allocate_index(state, obj);
    obj->set_handle_index(state, handle_index);
    return obj->handle(state);
  }

  void Memory::add_global_capi_handle_location(STATE, capi::Handle** loc,
                                               const char* file, int line) {
    utilities::thread::SpinLock::LockGuard guard(state->shared().global_capi_handle_lock());

    if(*loc && REFERENCE_P(*loc)) {
      if(!capi_handles_->validate(*loc)) {
        std::cerr << std::endl << "==================================== ERROR ====================================" << std::endl;
        std::cerr << "| An extension is trying to add an invalid handle at the following location:  |" << std::endl;
        std::ostringstream out;
        out << file << ":" << line;
        std::cerr << "| " << std::left << std::setw(75) << out.str() << " |" << std::endl;
        std::cerr << "|                                                                             |" << std::endl;
        std::cerr << "| An invalid handle means that it points to an invalid VALUE. This can happen |" << std::endl;
        std::cerr << "| when you haven't initialized the VALUE pointer yet, in which case we        |" << std::endl;
        std::cerr << "| suggest either initializing it properly or otherwise first initialize it to |" << std::endl;
        std::cerr << "| NULL if you can only set it to a proper VALUE pointer afterwards. Consider  |" << std::endl;
        std::cerr << "| the following example that could cause this problem:                        |" << std::endl;
        std::cerr << "|                                                                             |" << std::endl;
        std::cerr << "| VALUE ptr;                                                                  |" << std::endl;
        std::cerr << "| rb_gc_register_address(&ptr);                                               |" << std::endl;
        std::cerr << "| ptr = rb_str_new(\"test\");                                                   |" << std::endl;
        std::cerr << "|                                                                             |" << std::endl;
        std::cerr << "| Either change this register after initializing                              |" << std::endl;
        std::cerr << "|                                                                             |" << std::endl;
        std::cerr << "| VALUE ptr;                                                                  |" << std::endl;
        std::cerr << "| ptr = rb_str_new(\"test\");                                                   |" << std::endl;
        std::cerr << "| rb_gc_register_address(&ptr);                                               |" << std::endl;
        std::cerr << "|                                                                             |" << std::endl;
        std::cerr << "| Or initialize it with NULL:                                                 |" << std::endl;
        std::cerr << "|                                                                             |" << std::endl;
        std::cerr << "| VALUE ptr = NULL;                                                           |" << std::endl;
        std::cerr << "| rb_gc_register_address(&ptr);                                               |" << std::endl;
        std::cerr << "| ptr = rb_str_new(\"test\");                                                   |" << std::endl;
        std::cerr << "|                                                                             |" << std::endl;
        std::cerr << "| Please note that this is NOT a problem in Rubinius, but in the extension    |" << std::endl;
        std::cerr << "| that contains the given file above. A very common source of this problem is |" << std::endl;
        std::cerr << "| using older versions of therubyracer before 0.11.x. Please upgrade to at    |" << std::endl;
        std::cerr << "| least version 0.11.x if you're using therubyracer and encounter this        |" << std::endl;
        std::cerr << "| problem. For some more background information on why this is a problem      |" << std::endl;
        std::cerr << "| with therubyracer, you can read the following blog post:                    |" << std::endl;
        std::cerr << "|                                                                             |" << std::endl;
        std::cerr << "| http://blog.thefrontside.net/2012/12/04/therubyracer-rides-again/           |" << std::endl;
        std::cerr << "|                                                                             |" << std::endl;
        std::cerr << "================================== ERROR ======================================" << std::endl;
        rubinius::bug("Halting due to invalid handle");
      }
    }

    capi::GlobalHandle* global_handle = new capi::GlobalHandle(loc, file, line);
    global_capi_handle_locations_.push_back(global_handle);
  }

  void Memory::del_global_capi_handle_location(STATE, capi::Handle** loc) {
    utilities::thread::SpinLock::LockGuard guard(state->shared().global_capi_handle_lock());

    for(std::list<capi::GlobalHandle*>::iterator i = global_capi_handle_locations_.begin();
        i != global_capi_handle_locations_.end(); ++i) {
      if((*i)->handle() == loc) {
        delete *i;
        global_capi_handle_locations_.erase(i);
        return;
      }
    }
    rubinius::bug("Removing handle not in the list");
  }

  void Memory::make_capi_handle_cached(STATE, capi::Handle* handle) {
    utilities::thread::SpinLock::LockGuard guard(state->shared().capi_handle_cache_lock());
    cached_capi_handles_.push_back(handle);
  }

  ObjectArray* Memory::weak_refs_set() {
    return immix_->weak_refs_set();
  }
};

void* XMALLOC(size_t bytes) {
  rubinius::VM::current()->metrics().system.malloc++;
  rubinius::VM::current()->metrics().system.allocated_bytes += bytes;
  return malloc(bytes);
}

void XFREE(void* ptr) {
  rubinius::VM::current()->metrics().system.freed++;
  free(ptr);
}

void* XREALLOC(void* ptr, size_t bytes) {
  rubinius::VM::current()->metrics().system.realloc++;
  rubinius::VM::current()->metrics().system.freed++;
  rubinius::VM::current()->metrics().system.allocated_bytes += bytes;
  return realloc(ptr, bytes);
}

void* XCALLOC(size_t items, size_t bytes_per) {
  size_t bytes = bytes_per * items;

  rubinius::VM::current()->metrics().system.calloc++;
  rubinius::VM::current()->metrics().system.allocated_bytes += bytes;
  return calloc(items, bytes_per);
}
