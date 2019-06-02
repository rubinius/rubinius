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
      size_t recursion_limit_;
      size_t max_recursion_;
      size_t max_mark_stack_size_;

      void set_recursion_limit(STATE);
      bool recurse_p(STATE, size_t count) {
        return count < recursion_limit_;
      }

      static size_t invocation_frame_size;
      static size_t max_recursion_limit;

    public:
      MemoryTracer(STATE, Heap* heap)
        : mark_stack_()
        , heap_(heap)
        , recursion_limit_(1)
        , max_recursion_(0)
        , max_mark_stack_size_(0)
      { }

      virtual ~MemoryTracer() { }

      virtual void trace_heap(STATE);
      virtual void trace_mark_stack(STATE);
      virtual void trace_object(STATE, Object** obj, size_t recursion_count=0);
      virtual void mark_object_region(STATE, Object** obj);
    };
  }
}

#endif
