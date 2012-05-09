#ifndef RBX_LLVM_JIT_COMPILER_HPP
#define RBX_LLVM_JIT_COMPILER_HPP

#include <llvm/CodeGen/MachineCodeInfo.h>

#include "llvm/jit_context.hpp"

namespace llvm {
  class Function;
  class MachineCodeInfo;
  class BasicBlock;
  class Value;
}

namespace rubinius {
  class LLVMState;
  class JITMethodInfo;
  class VMMethod;
  class BackgroundCompileRequest;


namespace jit {
  class Context;
  class Builder;

  class Compiler {
    llvm::Function* function_;
    llvm::MachineCodeInfo* mci_;
    jit::Context ctx_;

  public:
    Compiler(LLVMState* ls)
      : function_(0)
      , mci_(0)
      , ctx_(ls)
    {}

    ~Compiler() {
      delete mci_;
    }

    int code_bytes() {
      return mci_->size();
    }

    llvm::Function* llvm_function() {
      return function_;
    }

    jit::Context& context() {
      return ctx_;
    }

    JITMethodInfo* info() {
      return ctx_.root();
    }

    void initialize_call_frame(llvm::Function* func,
      llvm::BasicBlock* block, llvm::Value* call_frame,
      int stack_size, llvm::Value* stack, llvm::Value* vars);

    void compile(LLVMState* state, BackgroundCompileRequest* req);
    void compile_method(LLVMState*, BackgroundCompileRequest* req);
    void compile_block(LLVMState*, CompiledMethod* cm, VMMethod* vmm);
    void compile_builder(jit::Context& ctx, LLVMState*, JITMethodInfo&, rubinius::jit::Builder&);

    void* function_pointer();
    void* generate_function(LLVMState* ls, bool indy=true);
    void show_machine_code();

    void import_args(LLVMState* ls, llvm::Function* func,
                   llvm::BasicBlock*& block, VMMethod* vmm,
                   llvm::Value* vars, llvm::Value* call_frame);
  };
}
}

#endif
