#ifndef RBX_VM_GC_WALKER_HPP
#define RBX_VM_GC_WALKER_HPP

#include <vector>

#include "gc/gc.hpp"

namespace rubinius {
  class ObjectWalker : public GarbageCollector {
    std::vector<Object*> stack_;
    std::map<Object*, bool> mark_bits_;

  public:
    ObjectWalker(ObjectMemory* om)
      : GarbageCollector(om)
    {}

    virtual ~ObjectWalker();

    virtual Object* saw_object(Object*);
    virtual void scanned_object(Object*) {}

    void seed(GCData& data);
    Object* next();
    size_t stack_size();
    Object** stack_buf();
  };
}

#endif
