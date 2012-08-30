#ifdef ENABLE_LLVM

#include "llvm/state.hpp"

#include "llvm/inline_block.hpp"
#include "builtin/compiledcode.hpp"

namespace rubinius {
  JITInlineBlock::JITInlineBlock(LLVMState* ls, llvm::PHINode* phi, llvm::BasicBlock* brk,
                   CompiledCode* code, MachineCode* mcode,
                   JITMethodInfo* scope, int which)
      : block_break_result_(phi)
      , block_break_loc_(brk)
      , machine_code_(mcode)
      , method_(&ls->roots())
      , scope_(scope)
      , which_(which)
      , created_object_(false)
    {
      method_.set(code);
    }
}

#endif
