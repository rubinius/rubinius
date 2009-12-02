#ifndef RBX_LLVM_JIT_COMPILER_HPP
#define RBX_LLVM_JIT_COMPILER_HPP

#include <llvm/CodeGen/MachineCodeInfo.h>

namespace llvm {
  class Function;
  class MachineCodeInfo;
  class BasicBlock;
  class Value;
}

namespace rubinius {
  class LLVMState;

namespace jit {
  class Compiler {
    llvm::Function* function_;
    llvm::MachineCodeInfo* mci_;

  public:
    Compiler()
      : function_(0)
      , mci_(0)
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

    void initialize_call_frame(llvm::Function* func,
      llvm::BasicBlock* block, llvm::Value* call_frame,
      int stack_size, llvm::Value* stack, llvm::Value* vars);

    void compile_method(LLVMState*, VMMethod* vmm);
    void compile_block(LLVMState*, VMMethod* vmm);
    void compile_builder(LLVMState*, JITMethodInfo&, rubinius::jit::Builder&);

    void* function_pointer();
    void* generate_function(LLVMState* ls);
    void show_machine_code();
    void show_assembly(LLVMState* ls);

    void import_args(LLVMState* ls, llvm::Function* func,
                   llvm::BasicBlock*& block, VMMethod* vmm,
                   llvm::Value* vars, llvm::Value* call_frame);
  };
}
}

#endif
