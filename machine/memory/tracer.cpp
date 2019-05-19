#include "config.h"
#include "vm.hpp"
#include "state.hpp"

#include "memory.hpp"
#include "memory/tracer.hpp"

namespace rubinius {
  namespace memory {
    void MemoryTracer::trace_heap(STATE) {
      GCData data(state->vm());

      heap_->collect_start(state, &data);

      heap_->collect_references(state, [this](STATE, Object* obj){
          return trace_object(state, 0, obj);
        });

      heap_->collect_finish(state, &data);
    }

    Object* MemoryTracer::trace_object(STATE, void* parent, Object* child) {
      if(!child->reference_p()) return nullptr;

      return state->memory()->immix()->saw_object(parent, child);
    }
  }
}
