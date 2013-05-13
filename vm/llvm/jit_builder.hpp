#ifndef RBX_LLVM_BUILDER_HPP
#define RBX_LLVM_BUILDER_HPP

#include "unwind_info.hpp"
#include "machine_code.hpp"

#include "llvm/jit_context.hpp"
#include "llvm/basic_block.hpp"
#include "llvm/inline_block.hpp"
#include "llvm/offset.hpp"
#if RBX_LLVM_API_VER >= 303
#include <llvm/IR/IRBuilder.h>
#elif RBX_LLVM_API_VER >= 302
#include <llvm/IRBuilder.h>
#else
#include <llvm/Support/IRBuilder.h>
#endif
#if RBX_LLVM_API_VER > 301
#include <llvm/DIBuilder.h>
#else
#include <llvm/Analysis/DIBuilder.h>
#endif

namespace rubinius {
  class InlinePolicy;
  class MachineCode;
  class JITMethodInfo;

namespace jit {
  class RuntimeData;

  class Builder {
  public:
    Context* ctx_;
    MachineCode* machine_code_;
    llvm::Type* cf_type;
    llvm::Type* vars_type;
    llvm::Type* stack_vars_type;
    llvm::Type* obj_type;
    llvm::Type* obj_ary_type;

    llvm::Value* block_env;
    llvm::Value* block_inv;
    llvm::Value* top_scope;
    llvm::Value* module;
    llvm::Value* exec;

    llvm::Value* call_frame;
    llvm::Value* stk;
    llvm::Value* vars;

    llvm::Value* stack_top;
    llvm::Value* method_entry_;
    llvm::Value* method;

    llvm::Value* arg_total;
    llvm::Value* valid_flag;

    llvm::Value* counter;

    llvm::Value* check_global_interrupts_pos;

    IRBuilder builder_;

    llvm::Value* call_frame_flags;
    bool use_full_scope_;
    bool number_of_sends_;
    bool loops_;

    BlockMap block_map_;

    llvm::BasicBlock* import_args_;
    llvm::BasicBlock* body_;

    JITMethodInfo& info_;

  protected:
    llvm::Value* counter2_;
    jit::RuntimeData* runtime_data_;
    llvm::DIBuilder debug_builder_;

  public:

    IRBuilder& b() { return builder_; }

    llvm::DIBuilder& debug_builder() { return debug_builder_; }

    Builder(Context* ctx, JITMethodInfo& info);

    void set_definition_location();
    void set_current_location(opcode ip);

    void pass_one(llvm::BasicBlock* body);

    void nil_stack(int size, llvm::Value* nil);

    void nil_locals();

    void alloc_frame(const char* body_name);

    bool generate_body();
    void generate_hard_return();

    llvm::Value* get_field(llvm::Value* val, int which);

    template <typename T>
    llvm::Value* constant(T obj, llvm::Type* obj_type) {
      return b().CreateIntToPtr(
        llvm::ConstantInt::get(ctx_->IntPtrTy, (intptr_t)obj),
        obj_type, "constant");
    }

    llvm::Value* cint(int num) {
      switch(num) {
      case 0:
        return ctx_->Zero;
      case 1:
        return ctx_->One;
      default:
        return llvm::ConstantInt::get(ctx_->Int32Ty, num);
      }
    }

  };
}
}

#endif
