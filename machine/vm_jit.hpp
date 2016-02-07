#ifndef RBX_VM_JIT_H
#define RBX_VM_JIT_H

#include <inttypes.h>

namespace rubinius {

  class VM;

  class VMJIT {
    friend class VM;
    friend class State;

  private:
    uintptr_t stack_start_;
    uintptr_t stack_limit_;
    int stack_size_;

    bool interrupt_with_signal_;
    bool interrupt_by_kill_;
    bool check_local_interrupts_;
    bool thread_step_;

  public:
    VMJIT()
      : stack_start_(0)
      , stack_limit_(0)
      , stack_size_(0)
      , interrupt_with_signal_(false)
      , interrupt_by_kill_(false)
      , check_local_interrupts_(false)
      , thread_step_(false)
    {}

  };
}

#endif

