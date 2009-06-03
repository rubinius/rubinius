#ifndef RBX_LLVM_JIT_HPP
#define RBX_LLVM_JIT_HPP

#include <stdint.h>
#include <unistd.h>

#include <llvm/Module.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Function.h>
#include <llvm/Module.h>
#include <llvm/Instructions.h>
#include <llvm/Support/IRBuilder.h>
#include <llvm/ModuleProvider.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/CodeGen/MachineCodeInfo.h>
#include <llvm/Pass.h>
#include <llvm/PassManager.h>

#include "vm.hpp"
#include "vmmethod.hpp"
#include "configuration.hpp"

#include "builtin/machine_method.hpp"

#include "instruments/timing.hpp"

namespace rubinius {

  enum JitDebug {
    cSimple = 1,
    cOptimized = 2,
    cMachineCode = 4
  };

  class BackgroundCompilerThread;

  class LLVMState {
    llvm::Module* module_;
    llvm::ExistingModuleProvider* mp_;
    llvm::ExecutionEngine* engine_;
    llvm::FunctionPassManager* passes_;

    const llvm::Type* object_;
    Configuration& config_;

    BackgroundCompilerThread* background_thread_;
    GlobalLock& global_lock_;
    SymbolTable& symbols_;

    int jitted_methods_;
    int queued_methods_;

    SharedState& shared_;

    bool include_profiling_;
    llvm::GlobalVariable* profiling_;

    int code_bytes_;

  public:

    uint64_t time_spent;

    static LLVMState* get(STATE);
    static void shutdown(STATE);
    static void on_fork(STATE);
    static void pause(STATE);
    static void unpause(STATE);

    LLVMState(STATE);
    void add_internal_functions();

    int jit_dump_code() {
      return config_.jit_dump_code;
    }

    Configuration& config() {
      return config_;
    }

    GlobalLock& global_lock() {
      return global_lock_;
    }

    llvm::GlobalVariable* profiling() {
      return profiling_;
    }

    bool include_profiling() {
      return include_profiling_;
    }

    llvm::Module* module() { return module_; }
    llvm::ExecutionEngine* engine() { return engine_; }
    llvm::FunctionPassManager* passes() { return passes_; }
    const llvm::Type* object() { return object_; }

    int jitted_methods() {
      return jitted_methods_;
    }

    int queued_methods() {
      return queued_methods_;
    }

    int add_jitted_method() {
      return ++jitted_methods_;
    }

    int code_bytes() {
      return code_bytes_;
    }

    void add_code_bytes(int bytes) {
      code_bytes_ += bytes;
    }

    SharedState& shared() { return shared_; }

    const llvm::Type* ptr_type(std::string name);

    void compile_soon(STATE, VMMethod* vmm);

    Symbol* symbol(const char* sym);

    void shutdown_i();
    void on_fork_i();
    void pause_i();
    void unpause_i();
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

    int code_bytes() {
      return mci_->size();
    }

    void initialize_call_frame(llvm::Function* func,
      llvm::BasicBlock* block, llvm::Value* call_frame,
      int stack_size, llvm::Value* stack, llvm::Value* vars);

    void compile(LLVMState*, VMMethod* vmm);
    void* function_pointer(STATE);
    void* function_pointer();
    llvm::Function* llvm_function(STATE);
    void show_assembly(STATE);
    void* generate_function(LLVMState* ls);
    void show_machine_code();

    void import_args(LLVMState* ls, llvm::Function* func,
                   llvm::BasicBlock*& block, VMMethod* vmm,
                   llvm::Value* vars, llvm::Value* call_frame);
  };

  class Signature {
  protected:
    LLVMState* ls_;
    std::vector<const llvm::Type*> types_;
    const llvm::Type* ret_type_;

  public:
    Signature(LLVMState* ls, const llvm::Type* rt)
      : ls_(ls)
      , ret_type_(rt)
    {}

    Signature(LLVMState* ls, const char* rt)
      : ls_(ls)
      , ret_type_(ls->ptr_type(rt))
    {}

    std::vector<const llvm::Type*>& types() {
      return types_;
    }

    Signature& operator<<(const char* name) {
      types_.push_back(ls_->ptr_type(name));

      return *this;
    }

    Signature& operator<<(const llvm::Type* type) {
      types_.push_back(type);

      return *this;
    }

    llvm::FunctionType* type() {
      return llvm::FunctionType::get(ret_type_, types_, false);
    }

    operator llvm::FunctionType*() { return type(); }

    llvm::Function* function(const char* name) {
      return llvm::cast<llvm::Function>(ls_->module()->getOrInsertFunction(name, type()));
    }

    llvm::CallInst* call(const char* name, llvm::Value** start, int size,
                      const char* inst_name, llvm::BasicBlock* block) {
      return llvm::CallInst::Create(function(name), start, start+size, inst_name, block);
    }

  };

  class BackgroundCompileRequest {
    VMMethod* vmm_;
    TypedRoot<MachineMethod*> mm_;

  public:
    BackgroundCompileRequest(STATE, VMMethod* vmm, MachineMethod* mm)
      : vmm_(vmm)
      , mm_(state)
    {
      mm_.set(mm);
    }

    VMMethod* vmmethod() {
      return vmm_;
    }

    MachineMethod* machine_method() {
      return mm_.get();
    }
  };

}

#endif
