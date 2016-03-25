#ifdef ENABLE_LLVM

#include "jit/llvm/state.hpp"

#include "jit/llvm/method_info.hpp"
#include "jit/llvm/context.hpp"
#include "jit/llvm/runtime.hpp"

#include "builtin/compiled_code.hpp"
#include "object_utils.hpp"

namespace rubinius {
  JITMethodInfo::JITMethodInfo(Context* ctx, CompiledCode* code, MachineCode* mcode,
                  JITMethodInfo* parent)
    : context_(ctx)
    , entry_(0)
    , call_frame_(0)
    , stack_(0)
    , args_(0)
    , variables_(0)
    , previous_(0)
    , profiling_entry_(0)
    , parent_info_(parent)
    , creator_info_(0)
    , use_full_scope_(false)
    , inline_block_(0)
    , block_info_(0)
    , method_(ctx->llvm_state()->vm())
    , return_pad_(0)
    , return_phi_(0)
    , self_class_(ctx->llvm_state()->vm())

    , machine_code(mcode)
    , is_block(false)
    , inline_return(0)
    , return_value(0)
    , inline_policy(0)
    , fin_block(0)
    , called_args(-1)
    , hits(0)
    , stack_args(0)
    , root(0)
  {
    method_.set(code);
    self_class_.set(nil<Object>());
  }

  void JITMethodInfo::setup_return() {
    return_pad_ = llvm::BasicBlock::Create(context_->llvm_context(), "return_pad", function());
    return_phi_ = llvm::PHINode::Create(
       context_->ptr_type("Object"), 0, "return_phi", return_pad_);
  }

  llvm::BasicBlock* JITMethodInfo::new_block(const char* name) {
    return llvm::BasicBlock::Create(context_->llvm_context(), name, function());
  }

}

#endif
