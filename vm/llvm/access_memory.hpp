#ifndef RBX_LLVM_ACCESS_MEMORY
#define RBX_LLVM_ACCESS_MEMORY

#include "llvm/jit.hpp"

namespace rubinius {
  class AccessManagedMemory {
    LLVMState* ls_;

  public:
    AccessManagedMemory(LLVMState* ls)
      : ls_(ls)
    {
      ls_->shared().gc_dependent();
    }

    ~AccessManagedMemory() {
      ls_->shared().gc_independent();
    }
  };
}

#endif
