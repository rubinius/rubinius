#ifndef RBX_VM_GC_HPP
#define RBX_VM_GC_HPP

#include <list>
#include <unordered_set>

#include "memory/header.hpp"

#include "shared_state.hpp"

#include "class/object.hpp"

namespace rubinius {
  struct CallFrame;
  struct MemoryHeader;

  class Memory;
  class VariableScope;
  class StackVariables;
  class LLVMState;

namespace memory {
  class ManagedThread;

  /**
   * Holds all the root pointers from which garbage collections will commence.
   * This includes the globally accessible Ruby objects such as class and
   * module instances, global variables, etc, but also various handles that
   * are used for FFI and CAPI.
   */

  class GCData {
    Roots& roots_;
    std::unordered_set<MemoryHeader*>& references_;
    ThreadNexus* thread_nexus_;
    unsigned int mark_;

  public:
    GCData(VM*);

    Roots& roots() {
      return roots_;
    }

    unsigned int mark() const {
      return mark_;
    }

    std::unordered_set<MemoryHeader*>& references() {
      return references_;
    }

    ThreadNexus* thread_nexus() {
      return thread_nexus_;
    }
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
    /// Reference to the Memory we are collecting
    Memory* memory_;

  public:
    /**
     * Constructor; takes a pointer to Memory.
     */
    GarbageCollector(Memory *om);

    virtual ~GarbageCollector() {
    }

    /**
     * Subclasses implement appropriate behaviour for handling a live object
     * encountered during garbage collection.
     */
    virtual Object* saw_object(Object*) = 0;
    virtual void scanned_object(Object*) = 0;
    virtual bool mature_gc_in_progress() = 0;

    // Scans the specified Object for references to other Objects.
    void scan_object(Object* obj);
    void delete_object(Object* obj);
    void walk_call_frame(CallFrame* call_frame, AddressDisplacement* offset=0);
    void verify_call_frame(CallFrame* call_frame, AddressDisplacement* offset=0);
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

    void clean_weakrefs(bool check_forwards=false);

    // Scans the thread for object references
    void scan(ManagedThread* thr, bool young_only);
    void scan(VariableRootBuffers& buffers, bool young_only, AddressDisplacement* offset=0);
    void scan(RootBuffers& rb, bool young_only);

    void verify(GCData* data);

    VM* vm();
    Memory* object_memory() {
      return memory_;
    }

    void reset_stats() {
    }

    friend class ObjectMark;
  };
}
}

#endif
