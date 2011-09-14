#ifndef VM_LLVM_INLINE_BLOCK_HPP
#define VM_LLVM_INLINE_BLOCK_HPP

#include "llvm/state.hpp"

namespace rubinius {
  class JITMethodInfo;
  class VMMethod;

  class JITInlineBlock {
    llvm::PHINode* block_break_result_;
    llvm::BasicBlock* block_break_loc_;
    VMMethod* code_;
    TypedRoot<CompiledMethod*> method_;
    JITMethodInfo* scope_;
    int which_;
    bool created_object_;

  public:
    JITInlineBlock(LLVMState* ls, llvm::PHINode* phi, llvm::BasicBlock* brk,
                   CompiledMethod* cm, VMMethod* code,
                   JITMethodInfo* scope, int which);

    llvm::PHINode* block_break_result() {
      return block_break_result_;
    }

    llvm::BasicBlock* block_break_loc() {
      return block_break_loc_;
    }

    VMMethod* code() {
      return code_;
    }

    CompiledMethod* method() {
      return method_.get();
    }

    JITMethodInfo* creation_scope() {
      return scope_;
    }

    int which() {
      return which_;
    }

    bool created_object_p() {
      return created_object_;
    }

    void set_created_object() {
      created_object_ = true;
    }
  };
}

#endif
