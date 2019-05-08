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
      stack_.erase(stack_.begin(), stack_.end());
      index_ = 0;
    }

    void find(void* obj) {
      for(auto i = stack_.begin(); i != stack_.end(); ++i) {
        MarkStackEntry& entry = *i;

        if(entry.child() == obj) {
          std::cerr << "obj: " << obj << " child of parent: " << entry.parent() << std::endl;
        } else if(entry.parent() == obj) {
          std::cerr << "obj: " << obj << " parent of child: " << entry.child() << std::endl;
        }
      }
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
