#ifndef RBX_MEMORY_VISITOR_HPP
#define RBX_MEMORY_VISITOR_HPP

#include "memory/header.hpp"
#include "memory/heap.hpp"
#include "memory/mark_stack.hpp"

#include <functional>

namespace rubinius {
  namespace memory {
    class MemoryVisitor {
      MarkStack mark_stack_;
      size_t recursion_limit_;

      void set_recursion_limit(STATE);
      bool recurse_p(STATE, size_t count) {
        return count < recursion_limit_;
      }

      static size_t invocation_frame_size;
      static size_t max_recursion_limit;

    public:
      MemoryVisitor(STATE)
        : mark_stack_()
        , recursion_limit_(1)
      { }

      virtual ~MemoryVisitor() { }

      virtual void visit_heap(STATE, std::function<void (STATE, Object**)> f);
      virtual void visit_mark_stack(STATE, std::function<void (STATE, Object**)> f);
      virtual void visit_object(STATE, Object** obj,
          std::function<void (STATE, Object**)> f, size_t recursion_count=0);
    };
  }
}

#endif
