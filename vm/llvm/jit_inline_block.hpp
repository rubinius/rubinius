#include "llvm/jit_inline_method.hpp"

namespace rubinius {
  class JITStackArgs;

namespace jit {
  class InlineBlockBuilder : public InlineMethodBuilder {
  public:
    InlineBlockBuilder(LLVMState* ls, JITMethodInfo& info, jit::RuntimeData* rd)
      : InlineMethodBuilder(ls, info, rd)
    {}

    llvm::BasicBlock* setup_inline_block(llvm::Value* self, llvm::Value* mod,
                                         JITStackArgs& stack_args);
  };
}
}
