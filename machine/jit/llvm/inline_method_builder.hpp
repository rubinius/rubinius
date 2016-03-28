#ifndef RBX_LLVM_INLINE_METHOD_HPP
#define RBX_LLVM_INLINE_METHOD_HPP

#include "jit/llvm/method.hpp"

namespace rubinius {
namespace jit {
  class RuntimeData;

  class InlineMethodBuilder : public MethodBuilder {
  public:
    InlineMethodBuilder(Context* ctx, JITMethodInfo& info, jit::RuntimeData* rd)
      : MethodBuilder(ctx, info)
    {
      runtime_data_ = rd;
    }

    llvm::BasicBlock* setup_inline(llvm::Value* self, llvm::Value* blk,
        std::vector<llvm::Value*>& args);
    void setup_inline_scope(llvm::Value* self, llvm::Value* blk, llvm::Value* mod);
    void assign_arguments(std::vector<Value*>& stack_args);
    void assign_fixed_arguments(std::vector<Value*>& stack_args,
        int local_start, int local_end, int arg_start);
  };
}
}

#endif
