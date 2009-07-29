#ifndef RBX_LLVM_WORKHORSE_HPP
#define RBX_LLVM_WORKHORSE_HPP

#include "llvm/jit.hpp"
#include "llvm/offset.hpp"

#include <llvm/Support/IRBuilder.h>

// I know I know, shouldn't put these in header files...
using namespace llvm;

namespace rubinius {
  class InlinePolicy;

  class LLVMWorkHorse {
  public:
    LLVMState* ls_;
    const Type* cf_type;
    const Type* vars_type;
    const Type* stack_vars_type;
    const Type* obj_type;
    const Type* obj_ary_type;
    Function* func;

    Value* vm;
    Value* prev;
    Value* msg;
    Value* args;
    Value* block_env;
    Value* block_inv;
    Value* top_scope;

    BasicBlock* block;

    Value* call_frame;
    Value* stk;
    Value* vars;

    Value* stack_top;
    Value* method_entry_;
    Value* method;

    Value* arg_total;
    Value* valid_flag;

    llvm::IRBuilder<> builder_;

  public:

    llvm::IRBuilder<>& b() { return builder_; }

    LLVMWorkHorse(LLVMState* ls);

    void return_value(Value* ret, BasicBlock* cont = 0);

    void initialize_call_frame(int stack_size);

    void initialize_block_frame(int stack_size);

    void nil_stack(int size, Value* nil);

    void nil_locals(VMMethod* vmm);

    void setup_scope(VMMethod* vmm);

    void setup_inline_scope(Value* self, Value* mod, VMMethod* vmm);

    void setup_block_scope(VMMethod* vmm);

    void check_arity(VMMethod* vmm);

    void import_args(VMMethod* vmm);

    void setup_block(VMMethod* vmm);

    void setup(VMMethod* vmm);

    BasicBlock* setup_inline(VMMethod* vmm, Function* current, Value* vm_i, Value* previous,
        Value* self, Value* mod, std::vector<Value*>& args);

    bool generate_body(JITMethodInfo& info);

    Value* get_field(Value* val, int which);

    template <typename T>
    Value* constant(T obj, const Type* obj_type) {
      return b().CreateIntToPtr(
      ConstantInt::get(Type::Int32Ty, (intptr_t)obj),
      obj_type, "constant");
    }

  };
}

#endif
