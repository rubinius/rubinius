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
  namespace memory {
    void MainHeap::collect_start(STATE, GCData* data) {
      state->memory()->collect_cycle();

      code_manager_.clear_marks();
      immix_->reset_stats();

      immix_->collect(data);
    }

    void MainHeap::collect_roots(STATE, std::function<Object* (STATE, Object*)> f) {
      for(Roots::Iterator i(state->globals().roots); i.more(); i.advance()) {
        if(Object* fwd = state->memory()->immix()->saw_object(0, i->get())) {
          i->set(fwd);
        }
      }

      {
        std::lock_guard<std::mutex> guard(state->vm()->thread_nexus()->threads_mutex());

        for(ThreadList::iterator i = state->vm()->thread_nexus()->threads()->begin();
            i != state->vm()->thread_nexus()->threads()->end();
            ++i)
        {
          ManagedThread* thr = (*i);

          for(Roots::Iterator ri(thr->roots()); ri.more(); ri.advance()) {
            if(Object* fwd = state->memory()->immix()->saw_object(0, ri->get())) {
              ri->set(fwd);
            }
          }

          //scan(thr->variable_root_buffers(), young_only);
          // VariableRootBuffer* vrb = displace(thr->variable_root_buffers().front(), offset);
          VariableRootBuffer* vrb = thr->variable_root_buffers().front();

          while(vrb) {
            Object*** buffer = vrb->buffer();
            for(int idx = 0; idx < vrb->size(); idx++) {
              Object** var = buffer[idx];
              Object* cur = *var;

              if(!cur) continue;

              if(cur->reference_p()) {
                if(Object* tmp = state->memory()->immix()->saw_object(vrb, cur)) {
                  *var = tmp;
                }
              } else if(cur->handle_p()) {
                // TODO: MemoryHeader mark MemoryHandle objects
              }
            }

            // vrb = displace((VariableRootBuffer*)vrb->next(), offset);
            vrb = (VariableRootBuffer*)vrb->next();
          }

          // scan(thr->root_buffers(), young_only);

          for(RootBuffers::Iterator i(thr->root_buffers());
              i.more();
              i.advance())
          {
            Object** buffer = i->buffer();
            for(int idx = 0; idx < i->size(); idx++) {
              Object* cur = buffer[idx];

              if(cur->reference_p()) {
                if(Object* tmp = state->memory()->immix()->saw_object(buffer, cur)) {
                  buffer[idx] = tmp;
                }
              } else if(cur->handle_p()) {
                // TODO: MemoryHeader mark MemoryHandle objects
              }
            }
          }

          if(VM* vm = thr->as_vm()) {
            vm->gc_scan(state->memory()->immix());
          }

          // scan(*i, false);
        }
      }

      for(auto i = state->memory()->references().begin();
          i != state->memory()->references().end();
          ++i)
      {
        if((*i)->reference_p()) {
          if((*i)->object_p()) {
            Object* obj = reinterpret_cast<Object*>(*i);

            if(obj->referenced() > 0) {
              if(Object* fwd = state->memory()->immix()->saw_object(0, obj)) {
                // TODO: MemoryHeader set new address
              }
            } else if(obj->memory_handle_p()) {
              MemoryHandle* handle = obj->extended_header()->get_handle();
              if(handle->accesses() > 0) {
                if(Object* fwd = state->memory()->immix()->saw_object(0, obj)) {
                  // TODO: MemoryHeader set new address
                }

                handle->unset_accesses();
              }
            } else if(!obj->finalizer_p() && !obj->weakref_p()) {
              if(Object* fwd = state->memory()->immix()->saw_object(0, obj)) {
                // TODO: MemoryHeader set new address
              }
            }
          } else if((*i)->data_p()) {
            // TODO: MemoryHeader process data object
          }
        } else {
          // TODO: MemoryHeader if not reference, remove or log?
          // is it an error to add a non-reference?
        }
      }
    }

    void MainHeap::collect_finish(STATE, GCData* data) {
      immix_->collect_finish(data);

      code_manager_.sweep();
      immix_->sweep(data);
      mark_sweep_->after_marked();

      state->shared().symbols.sweep(state);

      state->memory()->rotate_mark();

      diagnostics::GCMetrics* metrics = state->shared().gc_metrics();
      metrics->immix_count++;
      metrics->large_count++;
    }
  }

  void Memory::memory_error(STATE) {
    Exception::raise_memory_error(state);
  }

  Object* object_watch = 0;

  /* Memory methods */
  Memory::Memory(STATE)
    : collector_(new memory::Collector(state))
    , mark_sweep_(new memory::MarkSweepGC(this, state->shared().config))
    , immix_(new memory::ImmixGC(this))
    , code_manager_(&state->shared())
    , main_heap_(new memory::MainHeap(state, immix_, mark_sweep_, code_manager_))
    , cycle_(0)
    , mark_(0x1)
    , slab_size_(4096)
    , references_lock_()
    , references_set_()
    , shared_(state->shared())
    , vm_(state->vm())
    , last_object_id(1)
    , last_snapshot_id(0)
    , large_object_threshold(state->shared().config.memory_large_object)
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
      collector()->collect_requested(state,
          "collector: immix triggered collection request");
    }

    collect_flag = false;

    if(likely(obj = mark_sweep_->allocate(bytes, collect_flag))) {
      shared().memory_metrics()->large_objects++;
      shared().memory_metrics()->large_bytes += bytes;

      if(collect_flag) {
        collector()->collect_requested(state,
            "collector: large object space triggered collection request");
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
      collector()->collect_requested(state,
          "collector: code resource triggered collection request");
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
