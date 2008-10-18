#ifndef RBX_VM_GC_HPP
#define RBX_VM_GC_HPP

#include "builtin/object.hpp"
#include "vm/gc_object_mark.hpp"


namespace rubinius {

  class ObjectMemory;

  typedef std::vector<Object*> ObjectArray;


  class GarbageCollector {
    public:

    ObjectMemory* object_memory;
    ObjectArray* weak_refs;

    virtual Object* saw_object(Object*) = 0;
    virtual ~GarbageCollector()  { }
    GarbageCollector(ObjectMemory *om);
    void scan_object(Object* obj);
    void delete_object(Object* obj);
  };

}

#endif
