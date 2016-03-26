#ifndef RBX_LLVM_JIT_CONTEXT
#define RBX_LLVM_JIT_CONTEXT

#include <list>

#include <llvm/IR/LLVMContext.h>

#include "jit/llvm/state.hpp"
#include "jit/llvm/memory_manager.hpp"

namespace rubinius {
  class JITMethodInfo;

  namespace jit {
    class RuntimeData;
    class RuntimeDataHolder;
    class Builder;
  }


  class IRBuilderInserterWithDebug {
  public:
    IRBuilderInserterWithDebug(jit::Builder *builder) : builder_(builder) {}

  protected:
    void InsertHelper(llvm::Instruction *I, const llvm::Twine &Name,
                      llvm::BasicBlock *BB, llvm::BasicBlock::iterator InsertPt) const;

  private:
    jit::Builder *builder_;
  };

  typedef llvm::IRBuilder<true, llvm::ConstantFolder, IRBuilderInserterWithDebug> IRBuilder;

  // Represents compiling into a single llvm::Function. Because of
  // inlining, this Context spans multiple MethodInfo's.
  class Context {
    LLVMState* ls_;
    JITMethodInfo* root_info_;
    bool inlined_block_;
    int inline_depth_;

    jit::RuntimeDataHolder* rds_;
    llvm::LLVMContext ctx_;
    llvm::ExecutionEngine* engine_;
    llvm::PassManagerBuilder* builder_;
    llvm::FunctionPassManager* passes_;
    jit::RubiniusRequestJITMemoryManager* memory_;

    llvm::Module* module_;
    llvm::Function* function_;
    llvm::Value* state_;
    llvm::Value* out_args_;
    llvm::Value* counter_;

    unsigned int metadata_id_;

  public:

    llvm::Type* VoidTy;

    llvm::Type* Int1Ty;
    llvm::Type* Int8Ty;
    llvm::Type* Int16Ty;
    llvm::Type* Int32Ty;
    llvm::Type* Int64Ty;
    llvm::Type* IntPtrTy;
    llvm::Type* VoidPtrTy;
    llvm::Type* ObjTy;

    llvm::Type* FloatTy;
    llvm::Type* DoubleTy;

    llvm::Type* Int8PtrTy;

    llvm::Constant* Zero;
    llvm::Constant* One;

  public:
    Context(LLVMState* ls);
    ~Context();

    LLVMState* llvm_state() {
      return ls_;
    }

    llvm::LLVMContext& llvm_context() {
      return ctx_;
    }

    llvm::FunctionPassManager* passes() {
      return passes_;
    }

    llvm::ExecutionEngine* engine() {
      return engine_;
    }

    unsigned int metadata_id() {
      return metadata_id_;
    }

    void* native_function();

    llvm::Type* ptr_type(llvm::Type* type);
    llvm::Type* ptr_type(std::string name);
    llvm::Type* type(std::string name);

    llvm::Constant* cint(int num) {
      switch(num) {
      case 0:
        return Zero;
      case 1:
        return One;
      default:
        return llvm::ConstantInt::get(Int32Ty, num);
      }
    }

    llvm::Constant* clong(uintptr_t num) {
      return llvm::ConstantInt::get(IntPtrTy, num);
    }

    void set_root(JITMethodInfo* info) {
      root_info_ = info;
    }

    JITMethodInfo* root() {
      return root_info_;
    }

    llvm::Module* module() {
      return module_;
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

    jit::RuntimeDataHolder* runtime_data_holder() {
      return rds_;
    }

    void set_function(llvm::Function* func) {
      function_ = func;
    }

    llvm::Function* function() {
      return function_;
    }

    llvm::Value* state() {
      return state_;
    }

    void set_state(llvm::Value* state) {
      state_ = state;
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

    void profiling(IRBuilder& b, llvm::BasicBlock* prof, llvm::BasicBlock* cont);

    void init_variables(IRBuilder& b);

    void add_runtime_data(jit::RuntimeData* rd);

    std::ostream& inline_log(const char* header);
    void info(const char* msg);
    std::ostream& info_log(const char* header);
    std::ostream& log() {
      return ls_->log();
    }
  };


  class Signature {
  protected:
    Context* ctx_;
    std::vector<llvm::Type*> types_;
    llvm::Type* ret_type_;

  public:
    Signature(Context* ctx, llvm::Type* rt)
      : ctx_(ctx)
      , ret_type_(rt)
    {}

    Signature(Context* ctx, const char* rt)
      : ctx_(ctx)
      , ret_type_(ctx->ptr_type(rt))
    {}

    std::vector<llvm::Type*>& types() {
      return types_;
    }

    Signature& operator<<(const char* name) {
      types_.push_back(ctx_->ptr_type(name));

      return *this;
    }

    Signature& operator<<(llvm::Type* type) {
      types_.push_back(type);

      return *this;
    }

    llvm::FunctionType* type() {
      return llvm::FunctionType::get(ret_type_, types_, false);
    }

    operator llvm::FunctionType*() { return type(); }

    llvm::Function* function(const char* name) {
      return llvm::cast<llvm::Function>(ctx_->module()->getOrInsertFunction(name, type()));
    }

    void setDoesNotCapture(const char* name, int which) {
      function(name)->setDoesNotCapture(which);
    }

    llvm::CallInst* call(const char* name, llvm::Value** start, int size,
                      const char* inst_name, llvm::BasicBlock* block) {
      return llvm::CallInst::Create(function(name), llvm::ArrayRef<llvm::Value*>(start, size), inst_name, block);
    }

    llvm::CallInst* call(const char* name, llvm::Value** start, int size,
                      const char* inst_name, IRBuilder& builder) {
      return builder.CreateCall(function(name), llvm::ArrayRef<llvm::Value*>(start, size), inst_name);
    }

    llvm::CallInst* call(const char* name, std::vector<llvm::Value*> args,
                      const char* inst_name, IRBuilder& builder) {
      return builder.CreateCall(function(name), args, inst_name);
    }

  };


}

#endif
