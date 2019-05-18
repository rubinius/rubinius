#ifndef RBX_MEMORY_TRACER_HPP
#define RBX_MEMORY_TRACER_HPP

#include "memory/header.hpp"
#include "memory/heap.hpp"

namespace rubinius {
  namespace memory {
    class MemoryTracer {
      Heap* heap_;

    public:
      MemoryTracer(STATE, Heap* heap)
        : heap_(heap)
      { }

      virtual ~MemoryTracer() { }

      virtual void trace_heap(STATE);
      virtual Object* trace_object(STATE, Object* obj);
    };
  }
}

#endif
