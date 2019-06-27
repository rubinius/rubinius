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

    public:
      MemoryVisitor(STATE)
        : mark_stack_()
      { }

      virtual ~MemoryVisitor() { }

      virtual void visit_heap(STATE, std::function<void (STATE, Object**)> f);
      virtual void visit_mark_stack(STATE, std::function<void (STATE, Object**)> f);
      virtual void visit_object(STATE, Object** obj, std::function<void (STATE, Object**)> f);
    };
  }
}

#endif
