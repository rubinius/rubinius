#ifdef ENABLE_LLVM

#include "llvm/state.hpp"

#include "llvm/inline_block.hpp"
#include "builtin/compiled_code.hpp"

namespace rubinius {

  using namespace llvm;

  JITInlineBlock::JITInlineBlock(Context* ctx,
                   CompiledCode* code, MachineCode* mcode,
                   JITMethodInfo* scope, int which, Value* stack_ptr)
      : block_break_result_(NULL)
      , block_break_loc_(NULL)
      , block_emit_loc_(NULL)
      , machine_code_(mcode)
      , method_(&ctx->llvm_state()->roots())
      , scope_(scope)
      , which_(which)
      , stack_ptr_(stack_ptr)
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
