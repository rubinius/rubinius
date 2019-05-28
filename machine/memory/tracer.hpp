#ifndef RBX_MEMORY_TRACER_HPP
#define RBX_MEMORY_TRACER_HPP

#include "memory/header.hpp"
#include "memory/heap.hpp"
#include "memory/mark_stack.hpp"

namespace rubinius {
  namespace memory {
    class MemoryTracer {
      MarkStack mark_stack_;
      Heap* heap_;

    public:
      MemoryTracer(STATE, Heap* heap)
        : mark_stack_()
        , heap_(heap)
      { }

      virtual ~MemoryTracer() { }

      virtual void trace_heap(STATE);
      virtual void trace_mark_stack(STATE);
      virtual void scan_object(STATE, Object* obj);
      virtual Object* trace_object(STATE, void* parent, Object* child);
      virtual void trace_p_object(STATE, Object** obj);
    };
  }
}

#endif
