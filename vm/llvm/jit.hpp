#ifndef RBX_LLVM_JIT_HPP
#define RBX_LLVM_JIT_HPP

#include <stdint.h>
#include <llvm/Module.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Function.h>
#include <llvm/Module.h>
#include <llvm/Instructions.h>
#include <llvm/Support/IRBuilder.h>
#include <llvm/ModuleProvider.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/ExecutionEngine/MachineCodeInfo.h>
#include <llvm/Pass.h>
#include <llvm/PassManager.h>

#include "vm.hpp"
#include "vmmethod.hpp"

namespace rubinius {

  class LLVMState {
    llvm::Module* module_;
    llvm::ExistingModuleProvider* mp_;
    llvm::ExecutionEngine* engine_;
    llvm::FunctionPassManager* passes_;

    const llvm::Type* object_;
  public:

    static LLVMState* get(STATE);

    LLVMState();

    llvm::Module* module() { return module_; }
    llvm::ExecutionEngine* engine() { return engine_; }
    llvm::FunctionPassManager* passes() { return passes_; }
    const llvm::Type* object() { return object_; }
  };

  class LLVMCompiler {
    llvm::Function* function_;
    llvm::MachineCodeInfo* mci_;

  public:
    LLVMCompiler()
      : function_(0)
      , mci_(0)
    {}

    ~LLVMCompiler() {
      delete mci_;
    }

    void initialize_call_frame(STATE, llvm::Function* func,
      llvm::BasicBlock* block, llvm::Value* call_frame,
      int stack_size, llvm::Value* stack, llvm::Value* vars);

    void compile(STATE, VMMethod* vmm);
    void* function_pointer(STATE);
    llvm::Function* llvm_function(STATE);
    void show_assembly(STATE);
  };
}

#endif
