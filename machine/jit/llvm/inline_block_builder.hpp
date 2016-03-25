#include "jit/llvm/inline_method_builder.hpp"

namespace rubinius {
  class JITStackArgs;

namespace jit {
  class InlineBlockBuilder : public InlineMethodBuilder {
  public:
    InlineBlockBuilder(Context* ctx, JITMethodInfo& info, jit::RuntimeData* rd)
      : InlineMethodBuilder(ctx, info, rd)
    {}

    llvm::BasicBlock* setup_inline_block(llvm::Value* self, llvm::Value* mod,
                                         JITStackArgs& stack_args);
    void assign_arguments(JITStackArgs& stack_args);
    void assign_fixed_arguments(JITStackArgs& stack_args,
        int local_start, int local_end, int arg_start);
  };
}
}
