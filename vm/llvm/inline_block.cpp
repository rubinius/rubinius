#ifdef ENABLE_LLVM

#include "llvm/state.hpp"

#include "llvm/inline_block.hpp"
#include "builtin/compiledmethod.hpp"

namespace rubinius {
  JITInlineBlock::JITInlineBlock(LLVMState* ls, llvm::PHINode* phi, llvm::BasicBlock* brk,
                   CompiledMethod* cm, VMMethod* code,
                   JITMethodInfo* scope, int which)
      : block_break_result_(phi)
      , block_break_loc_(brk)
      , code_(code)
      , method_(&ls->roots())
      , scope_(scope)
      , which_(which)
      , created_object_(false)
    {
      method_.set(cm);
    }
}

#endif
