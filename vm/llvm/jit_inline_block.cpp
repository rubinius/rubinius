#ifdef ENABLE_LLVM

#include "llvm/jit_inline_block.hpp"
#include "call_frame.hpp"

using namespace llvm;

namespace rubinius {
namespace jit {
  BasicBlock* InlineBlockBuilder::setup_inline_block(Value* self, Value* mod) {
    func = info_.function();
    vm = info_.vm();
    prev = info_.parent_call_frame();
    args = ConstantExpr::getNullValue(ls_->ptr_type("Arguments"));

    BasicBlock* entry = BasicBlock::Create(ls_->ctx(), "inline_entry", func);
    b().SetInsertPoint(entry);

    info_.set_args(args);
    info_.set_previous(prev);
    info_.set_entry(entry);

    BasicBlock* body = BasicBlock::Create(ls_->ctx(), "block_body", func);
    pass_one(body);

    BasicBlock* alloca_block = &func->getEntryBlock();

    Value* cfstk = new AllocaInst(obj_type,
        ConstantInt::get(ls_->Int32Ty,
          (sizeof(CallFrame) / sizeof(Object*)) + vmm_->stack_size),
        "cfstk", alloca_block->getTerminator());

    call_frame = b().CreateBitCast(
        cfstk,
        llvm::PointerType::getUnqual(cf_type), "call_frame");

    info_.set_call_frame(call_frame);

    stk = b().CreateConstGEP1_32(cfstk, sizeof(CallFrame) / sizeof(Object*), "stack");

    info_.set_stack(stk);

    Value* var_mem = new AllocaInst(obj_type,
        ConstantInt::get(ls_->Int32Ty,
          (sizeof(StackVariables) / sizeof(Object*)) + vmm_->number_of_locals),
        "var_mem", alloca_block->getTerminator());

    vars = b().CreateBitCast(
        var_mem,
        llvm::PointerType::getUnqual(stack_vars_type), "vars");

    info_.set_variables(vars);

    Value* rd = constant(runtime_data_, ls_->ptr_type("jit::RuntimeData"));

    //  Setup the CallFrame
    //
    // previous
    b().CreateStore(prev, get_field(call_frame, offset::cf_previous));

    // msg
    b().CreateStore(
        b().CreatePointerCast(rd, ls_->Int8PtrTy),
        get_field(call_frame, offset::cf_msg));

    // cm
    method = b().CreateLoad(
        b().CreateConstGEP2_32(rd, 0, offset::runtime_data_method, "method_pos"),
        "cm");

    Value* cm_gep = get_field(call_frame, offset::cf_cm);
    b().CreateStore(method, cm_gep);

    // flags
    int flags = CallFrame::cInlineFrame;
    if(!use_full_scope_) flags |= CallFrame::cClosedScope;

    b().CreateStore(ConstantInt::get(ls_->Int32Ty, flags),
        get_field(call_frame, offset::cf_flags));

    // ip
    b().CreateStore(ConstantInt::get(ls_->Int32Ty, 0),
        get_field(call_frame, offset::cf_ip));

    // scope
    b().CreateStore(vars, get_field(call_frame, offset::cf_scope));

    nil_stack(vmm_->stack_size, constant(Qnil, obj_type));

    setup_inline_scope(self, constant(Qnil, obj_type), mod);

    // No argument handling, there are bytecodes in the body that
    // do that. We just have to make stack_args available.

    b().CreateBr(body);
    b().SetInsertPoint(body);

    return entry;
  }


}
}

#endif
