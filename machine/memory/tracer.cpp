#include "config.h"
#include "vm.hpp"
#include "state.hpp"

#include "memory.hpp"

#include "memory/collector.hpp"
#include "memory/immix.hpp"
#include "memory/third_region.hpp"
#include "memory/tracer.hpp"

#include "logger.hpp"

namespace rubinius {
  namespace memory {
    size_t MemoryTracer::invocation_frame_size = 100 * sizeof(uintptr_t);
    size_t MemoryTracer::max_recursion_limit = 10000;

    void MemoryTracer::set_recursion_limit(STATE) {
      uintptr_t stack_var = 1;

      recursion_limit_ = state->vm()->stack_remaining(state, &stack_var)
        / invocation_frame_size;

      if(recursion_limit_ > max_recursion_limit) {
        recursion_limit_ = max_recursion_limit;
      }
    }

    void MemoryTracer::trace_heap(STATE) {
      set_recursion_limit(state);

      heap_->collect_start(state);

      heap_->collect_references(state, [this](STATE, Object** obj){
          trace_object(state, obj);
        });

      for(auto i = state->collector()->memory_handles().begin();
          i != state->collector()->memory_handles().end();
          ++i)
      {
        MemoryHeader* header = reinterpret_cast<MemoryHeader*>(*i);
        MemoryHandle* handle = header->extended_header()->get_handle();

        if(handle->accesses() > 0 || handle->cycles() < 3) {
          if(header->object_p()) {
            Object* obj = reinterpret_cast<Object*>(header);

            trace_object(state, &obj);
            // TODO: MemoryHeader set new address
          } else if(header->data_p()) {
            // DataHeader* data = reinterpret_cast<DataHeader*>(header);
            // TODO: process data (not C-API Data) instances
          }
        } else if(handle->rdata_p()) {
          // TODO: GC investigate why we are retaining these
          Object* obj = reinterpret_cast<Object*>(header);

          trace_object(state, &obj);
          // TODO: MemoryHeader set new address
        }

        handle->cycle();
        handle->unset_accesses();
      }

      trace_mark_stack(state);

      state->memory()->collector()->trace_finalizers(state, this);

      trace_mark_stack(state);

      for(auto i = state->collector()->references().begin();
          i != state->collector()->references().end();)
      {
        MemoryHeader* header = reinterpret_cast<MemoryHeader*>(*i);

        if(header->referenced() == 0) {
          i = state->collector()->references().erase(i);
        } else {
          ++i;
        }
      }

      for(auto i = state->collector()->memory_handles().begin();
          i != state->collector()->memory_handles().end();)
      {
        MemoryHeader* header = reinterpret_cast<MemoryHeader*>(*i);

        if(!header->marked_p(state->memory()->mark())) {
          i = state->collector()->memory_handles().erase(i);
        } else {
          ++i;
        }
      }

      for(auto i = state->collector()->weakrefs().begin();
          i != state->collector()->weakrefs().end();)
      {
        MemoryHeader* header = reinterpret_cast<MemoryHeader*>(*i);

        if(!header->marked_p(state->memory()->mark())) {
          i = state->collector()->weakrefs().erase(i);
        } else {
          Object* obj = reinterpret_cast<Object*>(header);
          TypeInfo* ti = state->memory()->type_info[obj->type_id()];

          ti->update_weakref(state, obj);

          ++i;
        }
      }

      heap_->collect_finish(state);

      logger::info("memory tracer: max recursion: %ld, max mark stack size: %ld",
          max_recursion_, max_mark_stack_size_);
    }

    void MemoryTracer::trace_mark_stack(STATE) {
      while(!mark_stack_.empty()) {
        Object* obj = mark_stack_.get();

        trace_object(state, &obj);
      }
    }

    void MemoryTracer::mark_object_region(STATE, Object** obj) {
      Object* object = *obj;

      switch(object->region()) {
        case eThreadRegion:
          // TODO: GC
          break;
        case eFirstRegion: {
          Object* copy = state->memory()->main_heap()->first_region()->mark_address_of_object(
              state, nullptr, object, state->memory()->main_heap()->first_region()->allocator());

          if(copy != object) *obj = copy;

          break;
        }
        case eSecondRegion:
          // TODO: GC
          break;
        case eThirdRegion:
          // Do nothing
          break;
      }
    }

    void MemoryTracer::trace_object(STATE, Object** obj, size_t recursion_count) {
      Object* object = *obj;

      if(!object || !object->reference_p()) return;

      if(object->marked_p(state->memory()->mark())) {
        if(object->scanned_p()) return;
      } else {
        mark_object_region(state, obj);

        object->set_marked(state->memory()->mark());
        object->unset_scanned();
      }

      if(!recurse_p(state, recursion_count)) {
        mark_stack_.add(0, object);

        if(mark_stack_.size() > max_mark_stack_size_) {
          max_mark_stack_size_ = mark_stack_.size();
        }

        return;
      }

      object->set_scanned();

      recursion_count++;

      if(recursion_count > max_recursion_) {
        max_recursion_ = recursion_count;
      }

      trace_object(state, object->p_klass(), recursion_count);
      trace_object(state, object->p_ivars(), recursion_count);

      TypeInfo* ti = state->memory()->type_info[object->type_id()];

      ti->mark(state, object, [&](STATE, Object** obj){
          trace_object(state, obj, recursion_count);
        });
    }
  }
}
