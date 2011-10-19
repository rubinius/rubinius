#ifndef RBX_LLVM_JIT_METHOD_HPP
#define RBX_LLVM_JIT_METHOD_HPP

#include "llvm/jit_builder.hpp"

namespace rubinius {
namespace jit {
  class MethodBuilder : public Builder {
  public:
    MethodBuilder(LLVMState* ls, JITMethodInfo& info)
      : Builder(ls, info)
    {}

    void check_arity();
    void import_args();
    void import_args_19_style();
    void setup_scope();
    void initialize_frame(int);
    void return_value(llvm::Value* ret, llvm::BasicBlock* cont = 0);

    void setup();
  };
}
}

#endif
