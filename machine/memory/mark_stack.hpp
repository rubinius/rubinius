#ifndef RBX_MEMORY_MARK_STACK_H
#define RBX_MEMORY_MARK_STACK_H

#include "config.h"

#include <memory>
#include <vector>

namespace rubinius {
  class Object;

  namespace memory {
    typedef Object* MarkStackEntry;

    class MarkStack {
      typedef std::vector<MarkStackEntry> Stack;

      Stack stack_;

    public:
      MarkStack()
        : stack_()
      {
      }

      ~MarkStack() { }

      void add(void* parent, Object* child) {
        stack_.push_back(child);
      }

      size_t size() const {
        return stack_.size();
      }

      bool empty() {
        return stack_.empty();
      }

      MarkStackEntry get() {
        MarkStackEntry entry = stack_.back();
        stack_.pop_back();

        return entry;
      }
    };
  }
}
#endif
