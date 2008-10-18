#ifndef RBX_VM_GC_HPP
#define RBX_VM_GC_HPP

#include "builtin/object.hpp"
#include "vm/gc_object_mark.hpp"


namespace rubinius {

  class ObjectMemory;

  class GarbageCollector {
    public:

    ObjectMemory* object_memory;
    ObjectArray* weak_refs;

    virtual OBJECT saw_object(OBJECT) = 0;
    virtual ~GarbageCollector()  { }
    GarbageCollector(ObjectMemory *om);
    void scan_object(OBJECT obj);
    void delete_object(OBJECT obj);
  };

}

#endif
