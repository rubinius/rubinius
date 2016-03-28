#ifndef RBX_LLVM_JIT_METHOD_HPP
#define RBX_LLVM_JIT_METHOD_HPP

#include "jit/llvm/builder.hpp"

namespace rubinius {
namespace jit {
  class MethodBuilder : public Builder {
  public:
    MethodBuilder(Context* ctx, JITMethodInfo& info)
      : Builder(ctx, info)
    {}

    void check_arity();
    void assign_fixed_args(llvm::Value*, int, int);
    void import_args();
    void setup_scope();
    void initialize_frame(int);
    void return_value(llvm::Value* ret, llvm::BasicBlock* cont = 0);

    void setup();
  };
}
}

#endif
