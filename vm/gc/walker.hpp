#ifndef RBX_VM_GC_WALKER_HPP
#define RBX_VM_GC_WALKER_HPP

#include <vector>

#include "gc/gc.hpp"

namespace rubinius {
  class ObjectWalker : public GarbageCollector {
    std::vector<Object*> stack_;

  public:
    ObjectWalker(ObjectMemory* om)
      : GarbageCollector(om)
    {}

    virtual ~ObjectWalker();

    virtual Object* saw_object(Object*);

    void seed(GCData& data);
    Object* next();
  };
}

#endif
