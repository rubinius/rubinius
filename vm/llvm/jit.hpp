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
  public:

    static LLVMState* get(STATE);

    LLVMState();

    llvm::Module* module() { return module_; }
    llvm::ExecutionEngine* engine() { return engine_; }
    llvm::FunctionPassManager* passes() { return passes_; }
  };

  class LLVMCompiler {
    llvm::Function* function_;
    void* jit_entry_;

  public:
    LLVMCompiler()
      : function_(0)
      , jit_entry_(0)
    {}

    void compile(STATE, VMMethod* vmm);
    void* function_pointer(STATE);
    llvm::Function* llvm_function(STATE);
    static void show_assembly(STATE, llvm::Function* func);
  };
}

#endif
