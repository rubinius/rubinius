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

#include "memory/code_resource.hpp"
#include "memory/mark_sweep.hpp"
#include "memory/immix_collector.hpp"
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

#include "configuration.hpp"

#include "diagnostics/gc.hpp"
#include "diagnostics/memory.hpp"
#include "diagnostics/profiler.hpp"
#include "diagnostics/timing.hpp"

#include "dtrace/dtrace.h"

#include "logger.hpp"

#include <mutex>

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
    , code_manager_(&vm->shared)
    , cycle_(0)
    , mark_(0x1)
    , allow_gc_(true)
    , mature_gc_in_progress_(false)
    , slab_size_(4096)
    , interrupt_flag_(false)
    , collect_young_flag_(false)
    , collect_full_flag_(false)
    , references_lock_()
    , references_set_()
    , shared_(vm->shared)
    , vm_(vm)
    , last_object_id(1)
    , last_snapshot_id(0)
    , large_object_threshold(shared.config.memory_large_object)
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
  }

  void Memory::after_fork_child(STATE) {
    contention_lock_.init();
    mature_gc_in_progress_ = false;
    vm_ = state->vm();
  }

  bool Memory::refill_slab(STATE, memory::Slab& slab) {
    utilities::thread::SpinLock::LockGuard guard(allocation_lock_);

    memory::Address addr = memory::Address::null(); /* young_->allocate_for_slab(slab_size_); */

    diagnostics::MemoryMetrics* metrics = state->shared().memory_metrics();
    metrics->young_objects += slab.allocations();
    metrics->young_bytes += slab.bytes_used();

    if(addr) {
      slab.refill(addr, slab_size_);
      metrics->slab_refills++;
      return true;
    } else {
      slab.refill(0, 0);
      metrics->slab_refills_fails++;
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
    } else if(obj->region() == eLargeRegion) {
      if(mark_sweep_->validate_object(obj) == cMatureObject) {
        return true;
      }
    } else {
      if(immix_->validate_object(obj) == cInImmix) {
        return true;
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

    shared().memory_metrics()->promoted_objects++;
    shared().memory_metrics()->promoted_bytes += sz;

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

    std::atomic_thread_fence(std::memory_order_seq_cst);
  }

  void Memory::collect_young(STATE, memory::GCData* data) {
    timer::StopWatch<timer::milliseconds> timerx(
        state->shared().gc_metrics()->young_ms);

    /* 
    young_->collect(data);

    metrics::MetricsData& metrics = state->vm()->metrics();
    metrics.gc.young_count++;

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
        state->shared().gc_metrics()->immix_stop_ms);

    if(state->shared().config.log_gc_run.value) {
      logger::write("memory: full collection");
    }

    code_manager_.clear_marks();

    immix_->reset_stats();

    {
      memory::GCData data(state->vm());

      immix_->collect(&data);
      collect_full_finish(state, &data);
    }
  }

  void Memory::collect_full_restart(STATE, memory::GCData* data) {
    if(state->shared().config.log_gc_run.value) {
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

#ifdef RBX_GC_DEBUG
    immix_->verify(data);
#endif
    immix_->sweep(data);

    // Have to do this after all things that check for mark bits is
    // done, as it free()s objects, invalidating mark bits.
    mark_sweep_->after_marked();

    state->shared().symbols.sweep(state);

    rotate_mark();

    diagnostics::GCMetrics* metrics = state->shared().gc_metrics();
    metrics->immix_count++;
    metrics->large_count++;

    collect_full_flag_ = false;
    interrupt_flag_ = false;

    RUBINIUS_GC_END(1);
  }

  memory::MarkStack& Memory::mature_mark_stack() {
    return immix_->mark_stack();
  }

  void Memory::add_type_info(TypeInfo* ti) {
    utilities::thread::SpinLock::LockGuard guard(shared_.type_info_lock());

    if(TypeInfo* current = type_info[ti->type]) {
      delete current;
    }
    type_info[ti->type] = ti;
  }

  Object* Memory::new_object(STATE, native_int bytes, object_type type) {
    utilities::thread::SpinLock::LockGuard guard(allocation_lock_);

    Object* obj = 0;
    bool collect_flag = false;

    // TODO: check if immix_ needs to trigger GC
    if(likely(obj = immix_->allocate(bytes, collect_flag))) {
      shared().memory_metrics()->immix_objects++;
      shared().memory_metrics()->immix_bytes += bytes;

      MemoryHeader::initialize(
          obj, state->vm()->thread_id(), eFirstRegion, type, false);

      return obj;
    }

    if(collect_flag) {
      schedule_full_collection(
          "mature region allocate object",
          state->shared().gc_metrics()->immix_set);
    }

    collect_flag = false;

    if(likely(obj = mark_sweep_->allocate(bytes, collect_flag))) {
      shared().memory_metrics()->large_objects++;
      shared().memory_metrics()->large_bytes += bytes;

      if(collect_flag) {
        schedule_full_collection(
            "large region allocate object",
            state->shared().gc_metrics()->large_set);
      }

      MemoryHeader::initialize(
          obj, state->vm()->thread_id(), eLargeRegion, type, false);

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

    state->shared().memory_metrics()->code_bytes += cr->size();

    bool collect_flag = false;
    code_manager_.add_resource(cr, &collect_flag);

    if(collect_flag) {
      schedule_full_collection(
          "add code resource",
          state->shared().gc_metrics()->resource_set);
    }
  }
}

void* XMALLOC(size_t bytes) {
  if(rubinius::VM* vm = rubinius::VM::current()) {
    vm->shared.memory_metrics()->malloc++;
    vm->shared.memory_metrics()->allocated_bytes += bytes;
  }

  return malloc(bytes);
}

void XFREE(void* ptr) {
  if(rubinius::VM* vm = rubinius::VM::current()) {
    vm->shared.memory_metrics()->freed++;
  }

  free(ptr);
}

void* XREALLOC(void* ptr, size_t bytes) {
  if(rubinius::VM* vm = rubinius::VM::current()) {
    vm->shared.memory_metrics()->realloc++;
    vm->shared.memory_metrics()->freed++;
    vm->shared.memory_metrics()->allocated_bytes += bytes;
  }

  return realloc(ptr, bytes);
}

void* XCALLOC(size_t items, size_t bytes_per) {
  size_t bytes = bytes_per * items;

  if(rubinius::VM* vm = rubinius::VM::current()) {
    vm->shared.memory_metrics()->calloc++;
    vm->shared.memory_metrics()->allocated_bytes += bytes;
  }

  return calloc(items, bytes_per);
}
