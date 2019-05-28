#include "config.h"
#include "vm.hpp"
#include "state.hpp"

#include "memory.hpp"

#include "memory/collector.hpp"
#include "memory/immix.hpp"
#include "memory/large_region.hpp"
#include "memory/tracer.hpp"

namespace rubinius {
  namespace memory {
    void MemoryTracer::trace_heap(STATE) {
      heap_->collect_start(state);

      heap_->collect_references(state, [this](STATE, Object** obj){
          trace_p_object(state, obj);
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

            if(Object* fwd = trace_object(state, 0, obj)) {
              // TODO: MemoryHeader set new address
            }
          } else if(header->data_p()) {
            // DataHeader* data = reinterpret_cast<DataHeader*>(header);
            // TODO: process data (not C-API Data) instances
          }

          handle->cycle();
          handle->unset_accesses();
        } else if(handle->rdata_p()) {
          // TODO: GC investigate why we are retaining these
          Object* obj = reinterpret_cast<Object*>(header);

          if(Object* fwd = trace_object(state, 0, obj)) {
            // TODO: MemoryHeader set new address
          }
        }
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
          if(!header->marked_p(state->memory()->mark())) {
            // TODO: GC this would be a bug because we mark anything
            // referenced, so raise exception?
            i = state->collector()->references().erase(i);
          } else {
            ++i;
          }
        }
      }

      for(auto i = state->collector()->memory_handles().begin();
          i != state->collector()->memory_handles().end();)
      {
        MemoryHeader* header = reinterpret_cast<MemoryHeader*>(*i);

        if(!header->marked_p(state->memory()->mark())) {
          MemoryHandle* handle = header->extended_header()->get_handle();

          if(handle->accesses() == 0 && handle->cycles() >= 3) {
            i = state->collector()->memory_handles().erase(i);
          } else {
            ++i;
          }
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
      mark_stack_.finish();
    }

    void MemoryTracer::trace_mark_stack(STATE) {
      while(!mark_stack_.empty()) {
#ifdef RBX_GC_STACK_CHECK
        scan_object(state, mark_stack_.get().child());
#else
        scan_object(state, mark_stack_.get());
#endif
      }
    }

    void MemoryTracer::scan_object(STATE, Object* obj) {
      if(obj->marked_p(state->memory()->mark())) {
        if(obj->scanned_p()) return;

        obj->set_scanned();
      } else {
        // TODO: GC this should not be possible
        trace_object(state, 0, obj);
      }

      trace_p_object(state, obj->p_klass());

      if(obj->ivars()->reference_p()) {
        trace_p_object(state, obj->p_ivars());
      }

      TypeInfo* ti = state->memory()->type_info[obj->type_id()];

      /*
      ti->mark(state, obj, [this](STATE, Object* parent, Object* child){
          return trace_object(state, parent, child);
        });
        */
      ti->mark(state, obj, [this](STATE, Object** object){
          trace_p_object(state, object);
        });
    }

    void MemoryTracer::trace_p_object(STATE, Object** object) {
      Object* obj = *object;

      if(!obj || !obj->reference_p()) return;

      if(obj->marked_p(state->memory()->mark())) return;

      // Set the mark bits in the managed memory instance
      obj->unset_scanned();
      obj->set_marked(state->memory()->mark());

      // Set the accounting bits in the region containing the instance
      Object* copy = obj;

      switch(obj->region()) {
        case eThreadRegion:
          // TODO: GC
          break;
        case eFirstRegion:
          copy = state->memory()->main_heap()->first_region()->mark_address_of_object(state,
              nullptr, obj, state->memory()->main_heap()->first_region()->allocator());

          break;
        case eSecondRegion:
          // TODO: GC
          break;
        case eLargeRegion:
          // Do nothing
          break;
      }

      if(copy != obj) *object = copy;

      mark_stack_.add(nullptr, copy);
    }

    Object* MemoryTracer::trace_object(STATE, void* parent, Object* child) {
      if(!child || !child->reference_p()) return child;

      if(child->marked_p(state->memory()->mark())) return child;

      // Set the mark bits in the managed memory instance
      child->unset_scanned();
      child->set_marked(state->memory()->mark());

      // Set the accounting bits in the region containing the instance
      Object* copy = child;

      switch(child->region()) {
        case eThreadRegion:
          // TODO: GC
          break;
        case eFirstRegion:
          copy = state->memory()->main_heap()->first_region()->mark_address_of_object(state,
              parent, child, state->memory()->main_heap()->first_region()->allocator());

          break;
        case eSecondRegion:
          // TODO: GC
          break;
        case eLargeRegion:
          // Do nothing
          break;
      }

      mark_stack_.add(parent, copy);

      return copy == child ? child : copy;
    }
  }
}
