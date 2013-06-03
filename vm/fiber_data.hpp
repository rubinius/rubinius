#ifndef RBX_VM_FIBER_DATA_HPP
#define RBX_VM_FIBER_DATA_HPP

#include "config.h"
#include "bug.hpp"
#include "prelude.hpp"

#include "gc/variable_buffer.hpp"

namespace rubinius {

#if defined(IS_X86)
#define FIBER_ASM_X8632
  struct fiber_context_t {
    void* eip;
    void* esp;
    void* ebp;
    void* ebx;
  };

#elif defined(IS_X8664)
#define FIBER_ASM_X8664
  struct fiber_context_t {
    void* rip;
    void* rsp;
    void* rbp;
    void* rbx;
    void* r12;
    void* r13;
    void* r14;
    void* r15;
  };

#else
  struct fiber_context_t {
    int dummy;
  };
#endif

  class FiberStack;
  class FiberStacks;

  class FiberData {
    fiber_context_t machine_;

    enum Status {
      eInitial = 1,
      eRunning,
      eOnStack,
      eOnHeap,
      eDead
    } status_;

    bool mark_;

    VM* thread_;
    FiberStack* stack_;

    void* heap_;
    size_t heap_size_;
    size_t heap_capacity_;

    VariableRootBuffers variable_root_buffers_;

    CallFrame* call_frame_;

    // Private constructor so only FiberStack can use it.

    FiberData(VM* thread, bool root=false)
      : status_(root ? eOnStack : eInitial)
      , mark_(true)
      , thread_(thread)
      , stack_(0)
      , heap_(0)
      , heap_size_(0)
      , heap_capacity_(0)
      , call_frame_(0)
    {}

    friend class FiberStacks;

  public:
    ~FiberData();

    bool dead_p() const {
      return status_ == eDead;
    }

    bool marked_p() const {
      return mark_;
    }

    void set_mark() {
      mark_ = true;
    }

    void clear_mark() {
      mark_ = false;
    }

    CallFrame* call_frame() const {
      return call_frame_;
    }

    void set_call_frame(CallFrame* cf) {
      call_frame_ = cf;
    }

    VM* thread() const {
      return thread_;
    }

    VariableRootBuffers& variable_root_buffers() {
      return variable_root_buffers_;
    }

    bool uninitialized_p() const {
      return status_ == eInitial && stack_ == 0;
    }

    fiber_context_t* machine() {
      return &machine_;
    }

    void* stack_address() const {
      return stack_ ? stack_->address() : 0;
    }

    size_t stack_size() const {
      return stack_ ? stack_->size() : 0;
    }

    void* stack_bottom() const {
#ifdef RBX_FIBER_ENABLED
#if defined(FIBER_ASM_X8632)
      return machine_.esp;
#elif defined(FIBER_ASM_X8664)
      return machine_.rsp;
#endif
#else
      rubinius::bug("Fibers not supported on this platform");
      return 0; // for the cute little dumb compiler
#endif
    }

    intptr_t data_offset() const {
      if(status_ != eOnHeap) return 0;
      return (intptr_t)heap_ - (intptr_t)stack_bottom();
    }

    intptr_t data_lower_bound() const {
      if(status_ != eOnHeap) return 0;
      return (intptr_t)stack_->address();
    }

    intptr_t data_upper_bound() const {
      if(status_ != eOnHeap) return 0;
      return (intptr_t)stack_->address() + stack_->size();
    }

    bool currently_on_stack_p(FiberData* cur) const {
      return stack_ && stack_->user() == cur;
    }

    FiberStack* allocate_stack(STATE);
    void take_stack(STATE);
    void copy_stack(STATE, void* addr, size_t size);
    void copy_to_heap(STATE);

    void orphan(STATE);

    void switch_to(STATE, FiberData* from);
    void switch_and_orphan(STATE, FiberData* from);

    void die();
  };
}

#endif
