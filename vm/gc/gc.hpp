#ifndef RBX_VM_GC_HPP
#define RBX_VM_GC_HPP

#include <list>

#include "oop.hpp"
#include "builtin/object.hpp"

namespace rubinius {

  class ObjectMemory;
  struct CallFrame;
  class VariableScope;
  class GlobalCache;
  class StackVariables;
  class ManagedThread;

  namespace capi {
    class Handles;
  }

  typedef std::vector<Object*> ObjectArray;

  class ObjectVisitor {
  public:
    virtual ~ObjectVisitor() { }
    virtual Object* call(Object*) = 0;
  };

  class GCData {
    Roots& roots_;
    CallFrameLocationList& call_frames_;
    VariableRootBuffers& variable_buffers_;
    RootBuffers* root_buffers_;
    capi::Handles* handles_;
    capi::Handles* cached_handles_;
    GlobalCache* global_cache_;
    std::list<ManagedThread*>* threads_;
    std::list<capi::Handle**>* global_handle_locations_;

  public:
    GCData(STATE);
    GCData(Roots& r, CallFrameLocationList& l, VariableRootBuffers& b,
           capi::Handles* handles = NULL, capi::Handles* cached_handles = NULL,
           GlobalCache *cache = NULL, std::list<ManagedThread*>* ths = NULL,
           std::list<capi::Handle**>* global_handle_locations = NULL,
           RootBuffers* rf = NULL)
      : roots_(r)
      , call_frames_(l)
      , variable_buffers_(b)
      , root_buffers_(rf)
      , handles_(handles)
      , cached_handles_(cached_handles)
      , global_cache_(cache)
      , threads_(ths)
      , global_handle_locations_(global_handle_locations)
    {}

    Roots& roots() {
      return roots_;
    }

    CallFrameLocationList& call_frames() {
      return call_frames_;
    }

    std::list<ManagedThread*>* threads() {
      return threads_;
    }

    VariableRootBuffers& variable_buffers() {
      return variable_buffers_;
    }

    RootBuffers* root_buffers() {
      return root_buffers_;
    }

    capi::Handles* handles() {
      return handles_;
    }

    capi::Handles* cached_handles() {
      return cached_handles_;
    }

    GlobalCache* global_cache() {
      return global_cache_;
    }

    std::list<capi::Handle**>* global_handle_locations() {
      return global_handle_locations_;
    }
  };

  class GarbageCollector {
  protected:
    ObjectMemory* object_memory_;

  private:
    ObjectArray* weak_refs_;

  public:
    GarbageCollector(ObjectMemory *om);

    virtual ~GarbageCollector() {
      if(weak_refs_) delete weak_refs_;
    }

    virtual Object* saw_object(Object*) = 0;
    void scan_object(Object* obj);
    void delete_object(Object* obj);
    void walk_call_frame(CallFrame* top_call_frame);
    void saw_variable_scope(CallFrame* call_frame, StackVariables* scope);

    Object* mark_object(Object* obj) {
      if(!obj || !obj->reference_p()) return obj;
      Object* tmp = saw_object(obj);
      if(tmp) return tmp;
      return obj;
    }

    void clean_weakrefs(bool check_forwards=false);

    VM* state();

    void add_weak_ref(Object* obj) {
      if(!weak_refs_) {
        weak_refs_ = new ObjectArray;
      }

      weak_refs_->push_back(obj);
    }

    void reset_stats() {
    }

    friend class ObjectMark;
  };


}

#endif
