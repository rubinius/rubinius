#include "llvm/jit_inline_method.hpp"

namespace rubinius {
namespace jit {
  class InlineBlockBuilder : public InlineMethodBuilder {
  public:
    InlineBlockBuilder(LLVMState* ls, JITMethodInfo& info)
      : InlineMethodBuilder(ls, info)
    {}

    llvm::BasicBlock* setup_inline_block(llvm::Value* self, llvm::Value* mod);
  };
}
}
