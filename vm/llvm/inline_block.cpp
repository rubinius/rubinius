#ifdef ENABLE_LLVM

#include "llvm/state.hpp"

#include "llvm/inline_block.hpp"
#include "builtin/compiledcode.hpp"

namespace rubinius {

  using namespace llvm;

  JITInlineBlock::JITInlineBlock(LLVMState* ls,
                   CompiledCode* code, MachineCode* mcode,
                   JITMethodInfo* scope, int which)
      : block_break_result_(NULL)
      , block_break_loc_(NULL)
      , machine_code_(mcode)
      , method_(&ls->roots())
      , scope_(scope)
      , which_(which)
      , created_object_(false)
    {
      method_.set(code);
    }


  void JITInlineBlock::eraseBlockEmit() {
    if(!created_object_ && block_emit_loc_) {
      BasicBlock* before = block_emit_loc_->getSinglePredecessor();
      TerminatorInst* fromJump = before->getTerminator();
      TerminatorInst* toJump   = block_emit_loc_->getTerminator();

      fromJump->setSuccessor(0, toJump->getSuccessor(0));

      block_emit_loc_->eraseFromParent();
      block_emit_loc_ = NULL;
    }
  }
}

#endif
