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
  class LLVMState;

  namespace capi {
    class Handles;
  }

  /**
   * Holds all the root pointers from which garbage collections will commence.
   * This includes the globally accessible Ruby objects such as class and
   * module instances, global variables, etc, but also various handles that
   * are used for FFI and CAPI.
   */

  class GCData {
    Roots& roots_;
    capi::Handles* handles_;
    std::list<capi::Handle*>* cached_handles_;
    GlobalCache* global_cache_;
    std::list<ManagedThread*>* threads_;
    std::list<capi::GlobalHandle*>* global_handle_locations_;
    GCTokenImpl* gc_token_;
#ifdef ENABLE_LLVM
    LLVMState* llvm_state_;
#endif
    size_t young_bytes_allocated_;
    size_t mature_bytes_allocated_;
    size_t code_bytes_allocated_;
    size_t symbol_bytes_allocated_;

  public:
    GCData(VM*, GCToken gct);
    GCData(VM*);

    GCData(Roots& r,
           capi::Handles* handles = NULL, std::list<capi::Handle*>* cached_handles = NULL,
           GlobalCache *cache = NULL, std::list<ManagedThread*>* ths = NULL,
           std::list<capi::GlobalHandle*>* global_handle_locations = NULL)
      : roots_(r)
      , handles_(handles)
      , cached_handles_(cached_handles)
      , global_cache_(cache)
      , threads_(ths)
      , global_handle_locations_(global_handle_locations)
      , gc_token_(NULL)
#ifdef ENABLE_LLVM
      , llvm_state_(0)
#endif
      , young_bytes_allocated_(0)
      , mature_bytes_allocated_(0)
      , code_bytes_allocated_(0)
      , symbol_bytes_allocated_(0)
    {}

    Roots& roots() {
      return roots_;
    }

    std::list<ManagedThread*>* threads() {
      return threads_;
    }

    capi::Handles* handles() {
      return handles_;
    }

    std::list<capi::Handle*>* cached_handles() {
      return cached_handles_;
    }

    GlobalCache* global_cache() {
      return global_cache_;
    }

    std::list<capi::GlobalHandle*>* global_handle_locations() {
      return global_handle_locations_;
    }

    GCTokenImpl* gc_token() {
      return gc_token_;
    }

#ifdef ENABLE_LLVM
    LLVMState* llvm_state() {
      return llvm_state_;
    }
#endif

    size_t young_bytes_allocated() const {
      return young_bytes_allocated_;
    }

    size_t mature_bytes_allocated() const {
      return mature_bytes_allocated_;
    }

    size_t code_bytes_allocated() const {
      return code_bytes_allocated_;
    }

    size_t symbol_bytes_allocated() const {
      return symbol_bytes_allocated_;
    }

    size_t jit_bytes_allocated() const;
  };

  class AddressDisplacement {
    intptr_t offset_;
    intptr_t lower_bound_;
    intptr_t upper_bound_;

  public:
    AddressDisplacement(intptr_t o, intptr_t l, intptr_t u)
      : offset_(o)
      , lower_bound_(l)
      , upper_bound_(u)
    {}

    template <typename T>
      T displace(T ptr) const {
        intptr_t addr = (intptr_t)ptr;
        if(addr < lower_bound_) return ptr;
        if(addr >= upper_bound_) return ptr;

        return (T)((char*)ptr + offset_);
      }
  };


  /**
   * Abstract base class for the various garbage collector implementations.
   * Defines the interface the VM will use to perform garbage collections,
   * as well as providing implementations of common methods such as
   * mark_object and scan_object.
   */

  class GarbageCollector {
  protected:
    /// Reference to the ObjectMemory we are collecting
    ObjectMemory* object_memory_;

  private:
    /// Array of weak references
    ObjectArray* weak_refs_;

  public:
    /**
     * Constructor; takes a pointer to ObjectMemory.
     */
    GarbageCollector(ObjectMemory *om);

    virtual ~GarbageCollector() {
      if(weak_refs_) delete weak_refs_;
    }

    /**
     * Subclasses implement appropriate behaviour for handling a live object
     * encountered during garbage collection.
     */
    virtual Object* saw_object(Object*) = 0;
    virtual void scanned_object(Object*) = 0;
    // Scans the specified Object for references to other Objects.
    void scan_object(Object* obj);
    void delete_object(Object* obj);
    void walk_call_frame(CallFrame* top_call_frame, AddressDisplacement* offset=0);
    void verify_call_frame(CallFrame* top_call_frame, AddressDisplacement* offset=0);
    void saw_variable_scope(CallFrame* call_frame, StackVariables* scope);
    void verify_variable_scope(CallFrame* call_frame, StackVariables* scope);

    /**
     * Marks the specified Object +obj+ as live.
     */
    Object* mark_object(Object* obj) {
      if(!obj || !obj->reference_p()) return obj;
      Object* tmp = saw_object(obj);
      if(tmp) return tmp;
      return obj;
    }

    void scan_fibers(GCData* data, bool marked_only = true);
    void clean_weakrefs(bool check_forwards=false);
    void clean_locked_objects(ManagedThread* thr, bool young_only);

    // Scans the thread for object references
    void scan(ManagedThread* thr, bool young_only);
    void scan(VariableRootBuffers& buffers, bool young_only, AddressDisplacement* offset=0);
    void scan(RootBuffers& rb, bool young_only);

    void verify(GCData* data);

    VM* state();

    /**
     * Adds a weak reference to the specified object.
     *
     * A weak reference provides a way to hold a reference to an object without
     * that reference being sufficient to keep the object alive. If no other
     * reference to the weak-referenced object exists, it can be collected by
     * the garbage collector, with the weak-reference subsequently returning
     * null.
     */
    void add_weak_ref(Object* obj) {
      if(!weak_refs_) {
        weak_refs_ = new ObjectArray;
      }

      weak_refs_->push_back(obj);
    }

    void reset_stats() {
    }

    ObjectArray* weak_refs_set() {
      return weak_refs_;
    }

    friend class ObjectMark;
  };


}

#endif
