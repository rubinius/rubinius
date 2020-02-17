#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <sys/time.h>

#include "config.h"
#include "thread_state.hpp"
#include "memory.hpp"
#include "thread_phase.hpp"

#include "capi/tag.hpp"

#include "memory/code_resource.hpp"
#include "memory/collector.hpp"
#include "memory/immix.hpp"
#include "memory/third_region.hpp"
#include "memory/main_heap.hpp"

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

#include "diagnostics/collector.hpp"
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

  /* Memory methods */
  Memory::Memory(STATE, Configuration* configuration)
    : code_manager_()
    , main_heap_(new memory::MainHeap(configuration, code_manager_))
    , cycle_(0)
    , mark_(0x1)
    , visit_mark_(0x1)
    , class_count_(0)
    , global_serial_(0)
    , codedb_lock_()
    , type_info_lock_()
    , code_resource_lock_()
    , last_object_id(1)
    , last_snapshot_id(0)
    , large_object_threshold(configuration->memory_large_object)
  {
    for(size_t i = 0; i < LastObjectType; i++) {
      type_info[i] = NULL;
    }

    TypeInfo::init(this);
  }

  Memory::~Memory() {
    for(size_t i = 0; i < LastObjectType; i++) {
      if(type_info[i]) delete type_info[i];
    }

    delete main_heap_;
    main_heap_ = nullptr;
  }

  void Memory::after_fork_child(STATE) {
    new(&type_info_lock_) locks::spinlock_mutex;
    new(&code_resource_lock_) locks::spinlock_mutex;

    new(&codedb_lock_) std::recursive_mutex;
  }

  /* TODO: GC
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
    } else if(obj->region() == eThirdRegion) {
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
  */

  /* Garbage collection */

  void Memory::add_type_info(Memory* memory, TypeInfo* ti) {
    if(TypeInfo* current = type_info[ti->type]) {
      delete current;
    }
    type_info[ti->type] = ti;
  }

  Object* Memory::new_object(STATE, intptr_t bytes, object_type type) {
    return state->allocate_object(state, bytes, type);
  }

  Object* Memory::new_object_pinned(STATE, intptr_t bytes, object_type type) {
    std::lock_guard<locks::spinlock_mutex> guard(allocation_lock_);

    Object* obj = nullptr;

    if(likely(obj = main_heap_->third_region()->allocate(state, bytes))) {
      state->diagnostics()->memory_metrics()->large_objects++;
      state->diagnostics()->memory_metrics()->large_bytes += bytes;

      MemoryHeader::initialize(
          obj, state->thread_id(), eThirdRegion, type, false);

      return obj;
    }

    Memory::memory_error(state);
    return nullptr;
  }

  TypeInfo* Memory::find_type(int type) {
    return type_info[type];
  }

  TypeInfo* Memory::find_type_info(Object* obj) {
    return type_info[obj->type_id()];
  }

  /* TODO: GC
  ObjectPosition Memory::validate_object(Object* obj) {
    ObjectPosition pos;

    pos = young_->validate_object(obj);
    if(pos != cUnknown) return pos;

    pos = immix_->validate_object(obj);
    if(pos != cUnknown) return pos;

    return mark_sweep_->validate_object(obj);
  }
  */

  void Memory::add_code_resource(STATE, memory::CodeResource* cr) {
    std::lock_guard<locks::spinlock_mutex> guard(code_resource_lock());

    state->diagnostics()->memory_metrics()->code_bytes += cr->size();

    bool collect_flag = false;
    code_manager_.add_resource(cr, &collect_flag);

    if(collect_flag) {
      state->collector()->collect_requested(state,
          "collector: code resource triggered collection request");
    }
  }
}

void* XMALLOC(size_t bytes) {
  if(rubinius::ThreadState* state = rubinius::ThreadState::current()) {
    state->diagnostics()->memory_metrics()->malloc++;
    state->diagnostics()->memory_metrics()->allocated_bytes += bytes;
  }

  return malloc(bytes);
}

void XFREE(void* ptr) {
  if(rubinius::ThreadState* state = rubinius::ThreadState::current()) {
    state->diagnostics()->memory_metrics()->freed++;
  }

  free(ptr);
}

void* XREALLOC(void* ptr, size_t bytes) {
  if(rubinius::ThreadState* state = rubinius::ThreadState::current()) {
    state->diagnostics()->memory_metrics()->realloc++;
    state->diagnostics()->memory_metrics()->freed++;
    state->diagnostics()->memory_metrics()->allocated_bytes += bytes;
  }

  return realloc(ptr, bytes);
}

void* XCALLOC(size_t items, size_t bytes_per) {
  size_t bytes = bytes_per * items;

  if(rubinius::ThreadState* state = rubinius::ThreadState::current()) {
    state->diagnostics()->memory_metrics()->calloc++;
    state->diagnostics()->memory_metrics()->allocated_bytes += bytes;
  }

  return calloc(items, bytes_per);
}
