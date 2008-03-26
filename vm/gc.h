#ifndef RBX_GC_H
#define RBX_GC_H

#include "object.hpp"

namespace rubinius {

  class ObjectMemory;

  class GarbageCollector {

    protected:

    ObjectMemory *object_memory;

    public:

    virtual OBJECT saw_object(OBJECT) = 0;
    virtual ~GarbageCollector()  { }
    GarbageCollector(ObjectMemory *om);
    void scan_object(OBJECT obj);
  };
}

#endif
