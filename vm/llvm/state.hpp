#ifndef VM_LLVM_STATE_HPP
#define VM_LLVM_STATE_HPP

#include <stdint.h>
#include <unistd.h>

#include <llvm/Module.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Function.h>
#include <llvm/Module.h>
#include <llvm/Instructions.h>
#include <llvm/Support/IRBuilder.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/CodeGen/MachineCodeInfo.h>
#include <llvm/Pass.h>
#include <llvm/PassManager.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/LLVMContext.h>

#include "llvm/local_info.hpp"

#include "gc/managed.hpp"
#include "gc/write_barrier.hpp"
#include "configuration.hpp"
#include "util/thread.hpp"

namespace rubinius {
  typedef std::map<int, LocalInfo> LocalMap;
  class SymbolTable;
  class CompiledMethod;

  namespace jit {
    class Builder;
    class Context;
  }

  class BackgroundCompilerThread;
  class BlockEnvironment;

  enum JitDebug {
    cSimple = 1,
    cOptimized = 2,
    cMachineCode = 4
  };

  class LLVMState : public ManagedThread {
    llvm::LLVMContext& ctx_;
    llvm::Module* module_;
    llvm::ExecutionEngine* engine_;
    llvm::FunctionPassManager* passes_;

    const llvm::Type* object_;
    Configuration& config_;

    BackgroundCompilerThread* background_thread_;
    SymbolTable& symbols_;

    int jitted_methods_;
    int queued_methods_;
    int accessors_inlined_;
    int uncommons_taken_;

    SharedState& shared_;
    bool include_profiling_;
    llvm::GlobalVariable* profiling_;

    int code_bytes_;

    std::ostream* log_;

    gc::WriteBarrier write_barrier_;
    unsigned int metadata_id_;

    int fixnum_class_id_;
    int symbol_class_id_;
    int string_class_id_;

    bool type_optz_;

    thread::SpinLock method_update_lock_;

  public:

    uint64_t time_spent;

    const llvm::Type* VoidTy;

    const llvm::Type* Int1Ty;
    const llvm::Type* Int8Ty;
    const llvm::Type* Int16Ty;
    const llvm::Type* Int32Ty;
    const llvm::Type* Int64Ty;
    const llvm::Type* IntPtrTy;
    const llvm::Type* VoidPtrTy;

    const llvm::Type* FloatTy;
    const llvm::Type* DoubleTy;

    const llvm::Type* Int8PtrTy;

    llvm::Value* Zero;
    llvm::Value* One;

    static LLVMState* get(STATE);
    static void shutdown(STATE);
    static void start(STATE);
    static void on_fork(STATE);
    static void pause(STATE);
    static void unpause(STATE);

    LLVMState(STATE);
    ~LLVMState();

    void add_internal_functions();

    int jit_dump_code() {
      return config_.jit_dump_code;
    }

    bool debug_p();

    Configuration& config() {
      return config_;
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

    void add_accessor_inlined() {
      accessors_inlined_++;
    }

    int accessors_inlined() {
      return accessors_inlined_;
    }

    void add_uncommons_taken() {
      uncommons_taken_++;
    }

    int uncommons_taken() {
      return uncommons_taken_;
    }

    SharedState& shared() { return shared_; }

    llvm::LLVMContext& ctx() { return ctx_; }

    std::ostream& log() {
      return *log_;
    }

    gc::WriteBarrier* write_barrier() {
      return &write_barrier_;
    }

    unsigned int metadata_id() {
      return metadata_id_;
    }

    int fixnum_class_id() {
      return fixnum_class_id_;
    }

    int symbol_class_id() {
      return symbol_class_id_;
    }

    int string_class_id() {
      return string_class_id_;
    }

    bool type_optz() {
      return type_optz_;
    }

    llvm::Value* cint(int num) {
      switch(num) {
      case 0:
        return Zero;
      case 1:
        return One;
      default:
        return llvm::ConstantInt::get(Int32Ty, num);
      }
    }

    void start_method_update() {
      method_update_lock_.lock();
    }

    void end_method_update() {
      method_update_lock_.unlock();
    }

    const llvm::Type* ptr_type(std::string name);
    const llvm::Type* type(std::string name);

    void compile_soon(STATE, CompiledMethod* cm, Object* extra, bool is_block=false);
    void remove(llvm::Function* func);

    CallFrame* find_candidate(STATE, CompiledMethod* start, CallFrame* call_frame);
    void compile_callframe(STATE, CompiledMethod* start, CallFrame* call_frame,
                           int primitive = -1);

    Symbol* symbol(const std::string sym);
    std::string symbol_debug_str(const Symbol* sym);

    std::string enclosure_name(CompiledMethod* cm);

    void shutdown_i();
    void start_i();
    void on_fork_i();
    void pause_i();
    void unpause_i();

    static void show_machine_code(void* impl, size_t bytes);
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

    void setDoesNotCapture(const char* name, int which) {
      function(name)->setDoesNotCapture(which, true);
    }

    llvm::CallInst* call(const char* name, llvm::Value** start, int size,
                      const char* inst_name, llvm::BasicBlock* block) {
      return llvm::CallInst::Create(function(name), start, start+size, inst_name, block);
    }

    llvm::CallInst* call(const char* name, llvm::Value** start, int size,
                      const char* inst_name, llvm::IRBuilder<>& builder) {
      return builder.CreateCall(function(name), start, start+size, inst_name);
    }

    llvm::CallInst* call(const char* name, std::vector<llvm::Value*> args,
                      const char* inst_name, llvm::IRBuilder<>& builder) {
      return builder.CreateCall(function(name), args.begin(), args.end(), inst_name);
    }

  };

}

#endif
