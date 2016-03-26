#ifndef RBX_LLVM_JIT_COMPILER_HPP
#define RBX_LLVM_JIT_COMPILER_HPP

// TODO: LLVM-3.6
// #include <llvm/CodeGen/MachineCodeInfo.h>

#include "jit/llvm/context.hpp"

namespace llvm {
  class Function;
  class MachineCodeInfo;
  class BasicBlock;
  class Value;
}

namespace rubinius {
  class LLVMState;
  class JITMethodInfo;
  class MachineCode;
  class JITCompileRequest;

namespace jit {
  class Builder;

  class Compiler {
    Context* ctx_;
    llvm::Function* function_;

  public:
    Compiler(Context* ctx)
      : ctx_(ctx)
      , function_(0)
    {}

    ~Compiler() {
      if(function_) delete function_;
    }

    int code_bytes() {
      return 0;
      // TODO: LLVM-3.6
      // return mci_->size();
    }

    llvm::Function* llvm_function() {
      return function_;
    }

    Context* context() {
      return ctx_;
    }

    JITMethodInfo* info() {
      return ctx_->root();
    }

    void initialize_call_frame(llvm::Function* func,
      llvm::BasicBlock* block, llvm::Value* call_frame,
      int stack_size, llvm::Value* stack, llvm::Value* vars);

    void compile(JITCompileRequest* req);
    void compile_method(JITCompileRequest* req);
    void compile_block(JITCompileRequest* req);
    void compile_builder(JITMethodInfo&, rubinius::jit::Builder&);

    void* generate_function(bool indy=true);
    void show_machine_code();

    void import_args(LLVMState* ls, llvm::Function* func,
                   llvm::BasicBlock*& block, MachineCode* mcode,
                   llvm::Value* vars, llvm::Value* call_frame);
  };
}
}

#endif
