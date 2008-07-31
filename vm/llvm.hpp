#ifndef RBX_LLVM_HPP

#include "vmmethod.hpp"
#include <llvm/Function.h>
#include <llvm/Module.h>
#include <llvm/Instructions.h>

struct jit_state;
namespace rubinius {
  typedef void (*CompiledFunction)(Task*, struct jit_state* const, int*);

  class VMLLVMMethod : public VMMethod {
  public:
    llvm::Function* function;
    CompiledFunction c_func;

    VMLLVMMethod(STATE, CompiledMethod* meth) :
      VMMethod(state, meth), function(NULL), c_func(NULL) { }
    static void init(const char* path);
    llvm::CallInst* call_operation(Opcode* op, llvm::Value* state,
        llvm::Value*task, llvm::BasicBlock* block);
    virtual void compile();
    virtual void resume(Task* task, MethodContext* ctx);
  };

  class VMLLVMMethodUncompiled : public VMMethod {
  public:
    STATE;

    VMLLVMMethodUncompiled(STATE, CompiledMethod* meth):
      VMMethod(state, meth) {
        this->execute = VMLLVMMethodUncompiled::executor;
      }

    static bool executor(STATE, VMExecutable* exec, Task* task, Message& msg);
  };
}

#endif
