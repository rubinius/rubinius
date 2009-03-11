#ifndef RBX_VM_GC_HPP
#define RBX_VM_GC_HPP

#include "oop.hpp"
#include "builtin/object.hpp"
#include "gc/object_mark.hpp"

namespace rubinius {

  class ObjectMemory;
  class CallFrame;
  class VariableScope;

  typedef std::vector<Object*> ObjectArray;

  class ObjectVisitor {
  public:
    virtual ~ObjectVisitor() { }
    virtual Object* call(Object*) = 0;
  };

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

    void visit_variable_scope(VariableScope* scope, ObjectVisitor& visit);
    void visit_call_frame(CallFrame* top, ObjectVisitor& visit);

    Object* mark_object(Object* obj) {
      if(!obj || !obj->reference_p()) return obj;
      Object* tmp = saw_object(obj);
      if(tmp) return tmp;
      return obj;
    }

    void visit_roots(Roots& roots, ObjectVisitor& visit);
    void visit_call_frames_list(CallFrameLocationList& call_frames, ObjectVisitor& visit);
    void unmark_all(Roots &roots, CallFrameLocationList& call_frames);
    void clean_weakrefs(bool check_forwards=false);
  };

}

#endif
