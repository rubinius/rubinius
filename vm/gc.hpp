#ifndef RBX_VM_GC_HPP
#define RBX_VM_GC_HPP

#include "builtin/object.hpp"
#include "vm/gc_object_mark.hpp"


namespace rubinius {

  class ObjectMemory;
  class CallFrame;
  class VariableScope;

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
    void walk_call_frame(CallFrame* top_call_frame);
    void saw_variable_scope(VariableScope* scope);
  };

}

#endif
