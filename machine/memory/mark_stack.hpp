#ifndef RBX_MEMORY_MARK_STACK_H
#define RBX_MEMORY_MARK_STACK_H

#include "config.h"

#include <memory>
#include <vector>

namespace rubinius {
  class Object;

namespace memory {
#ifdef RBX_GC_STACK_CHECK
  class MarkStackEntry {
    void* parent_;
    Object* child_;

  public:
    MarkStackEntry(void* parent, Object* child)
      : parent_(parent)
      , child_(child)
    {
    }

    ~MarkStackEntry() { }

    void* parent() {
      return parent_;
    }

    Object* child() {
      return child_;
    }
  };

  class MarkStack {
    typedef std::vector<MarkStackEntry> Stack;

    Stack stack_;

    Stack::size_type index_;

  public:
    MarkStack()
      : stack_()
      , index_(0)
    {
    }

    ~MarkStack() {
      finish();
    }

    void add(void* parent, Object* child) {
      stack_.push_back(MarkStackEntry(parent, child));
    }

    bool empty() {
      return index_ >= stack_.size();
    }

    MarkStackEntry& get() {
      return stack_.at(index_++);
    }

    void finish() {
      /*
      while(!stack_.empty()) {
        MarkStackEntry* entry = stack_.back();
        stack_.pop_back();
        delete entry;
      }
      */

      stack_.erase(stack_.begin(), stack_.end());
      index_ = 0;
    }
  };
#else
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

    bool empty() {
      return stack_.empty();
    }

    MarkStackEntry get() {
      MarkStackEntry entry = stack_.back();
      stack_.pop_back();

      return entry;
    }

    void finish() {
      // Do nothing
    }
  };
#endif  // RBX_GC_STACK_CHECK
};
};
#endif
