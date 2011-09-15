#ifndef RBX_LLVM_JIT_CONTEXT
#define RBX_LLVM_JIT_CONTEXT

#include <list>

#include "llvm/state.hpp"

namespace rubinius {
  class JITMethodInfo;

namespace jit {
  class RuntimeData;
  class RuntimeDataHolder;

  // Represents compiling into a single llvm::Function. Because of
  // inlining, this Context spans multiple MethodInfo's.
  class Context {
    LLVMState* ls_;
    JITMethodInfo* root_info_;
    bool inlined_block_;
    int inline_depth_;

    RuntimeDataHolder* rds_;
    llvm::Function* function_;
    llvm::Value* vm_;
    llvm::Value* out_args_;
    llvm::Value* counter_;
    llvm::Value* unwind_info_;

  public:
    Context(LLVMState* ls);


    LLVMState* state() {
      return ls_;
    }

    void set_root(JITMethodInfo* info) {
      root_info_ = info;
    }

    JITMethodInfo* root() {
      return root_info_;
    }

    void set_inlined_block(bool val=true) {
      inlined_block_ = val;
    }

    int enter_inline() {
      return ++inline_depth_;
    }

    int leave_inline() {
      return --inline_depth_;
    }

    int inline_depth() {
      return inline_depth_;
    }

    bool inlined_block() {
      return inlined_block_;
    }

    RuntimeDataHolder* runtime_data_holder() {
      return rds_;
    }

    void set_function(llvm::Function* func) {
      function_ = func;
    }

    llvm::Function* function() {
      return function_;
    }

    llvm::Value* vm() {
      return vm_;
    }

    void set_vm(llvm::Value* vm) {
      vm_ = vm;
    }

    void set_out_args(llvm::Value* out_args) {
      out_args_ = out_args;
    }

    llvm::Value* out_args() {
      return out_args_;
    }

    llvm::Value* counter() {
      return counter_;
    }

    void set_counter(llvm::Value* counter) {
      counter_ = counter;
    }

    llvm::Value* unwind_info() {
      return unwind_info_;
    }

    void set_unwind_info(llvm::Value* unwind) {
      unwind_info_ = unwind;
    }

    void init_variables(llvm::IRBuilder<>& b);

    void add_runtime_data(jit::RuntimeData* rd);

    std::ostream& inline_log(const char* header);
    void info(const char* msg);
    std::ostream& info_log(const char* header);
  };

}}

#endif
