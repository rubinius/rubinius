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

      heap_->collect_references(state, [this](STATE, void* parent, Object* obj){
          return trace_object(state, parent, obj);
        });

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
            i = state->collector()->references().erase(i);
          } else {
            ++i;
          }
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

      if(Object* klass = trace_object(state, obj, obj->klass())) {
        obj->klass(state->memory(), force_as<Class>(klass));
      }

      if(obj->ivars()->reference_p()) {
        if(Object* ivars = trace_object(state, obj, obj->ivars())) {
          obj->ivars(state->memory(), ivars);
        }
      }

      TypeInfo* ti = state->memory()->type_info[obj->type_id()];

      ti->mark(state, obj, [this](STATE, Object* parent, Object* child){
          return trace_object(state, parent, child);
        });
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
