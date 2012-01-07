#ifndef RBX_VM_FIBER_DATA_HPP
#define RBX_VM_FIBER_DATA_HPP

#include "vm/config.h"
#include "prelude.hpp"

namespace rubinius {

#if defined(IS_X86)
#define FIBER_ENABLED
#define FIBER_ASM_X8632
  struct fiber_context_t {
    void* eip;
    void* esp;
    void* ebp;
    void* ebx;
  };

#elif defined(IS_X8664)
#define FIBER_ENABLED
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
  }
#endif

  class FiberStack;

  class FiberData {
    fiber_context_t machine_;

    enum Status {
      eInitial = 1,
      eRunning,
      eOnStack,
      eOnHeap,
      eDead
    } status_;

    FiberStack* stack_;

    void* heap_;
    size_t heap_size_;
    size_t heap_capacity_;

  public:

    FiberData()
      : status_(eInitial)
      , stack_(0)
      , heap_(0)
      , heap_size_(0)
      , heap_capacity_(0)
    {}

    bool uninitialized_p() {
      return stack_ == 0;
    }

    fiber_context_t* machine() {
      return &machine_;
    }

    void* stack_address() {
      return stack_ ? stack_->address() : 0;
    }

    size_t stack_size() {
      return stack_ ? stack_->size() : 0;
    }

    void* stack_bottom() {
#if defined(FIBER_ASM_X8632)
      return machine_.esp;
#elif defined(FIBER_ASM_X8664)
      return machine_.rsp;
#else
#error "not supported, you shouldn't be here"
#endif
    }

    intptr_t data_offset() {
      if(status_ != eOnHeap) return 0;
      return (intptr_t)heap_ - (intptr_t)stack_bottom();
    }

    FiberStack* allocate_stack(STATE);
    void take_stack(STATE);
    void copy_stack(STATE, void* addr, size_t size);
    void copy_to_heap(STATE);

    void orphan(STATE);
  };

}
#endif
