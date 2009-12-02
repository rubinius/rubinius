#ifndef RBX_LLVM_INLINE_METHOD_HPP
#define RBX_LLVM_INLINE_METHOD_HPP

#include "llvm/jit_method.hpp"

namespace rubinius {
namespace jit {
  class InlineMethodBuilder : public MethodBuilder {
  public:
    InlineMethodBuilder(LLVMState* ls, JITMethodInfo& info)
      : MethodBuilder(ls, info)
    {}

    llvm::BasicBlock* setup_inline(llvm::Value* self, llvm::Value* blk,
        llvm::Value* mod, std::vector<llvm::Value*>& args);
    void setup_inline_scope(llvm::Value* self, llvm::Value* blk, llvm::Value* mod);
  };
}
}

#endif
