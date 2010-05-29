#ifdef ENABLE_LLVM

#include "llvm/jit_inline_method.hpp"
#include "call_frame.hpp"

using namespace llvm;

namespace rubinius {
namespace jit {
  BasicBlock* InlineMethodBuilder::setup_inline(Value* self, Value* blk,
      std::vector<Value*>& stack_args)
  {
    func = info_.function();
    vm = info_.vm();
    prev = info_.parent_call_frame();
    args = ConstantExpr::getNullValue(ls_->ptr_type("Arguments"));

    BasicBlock* entry = BasicBlock::Create(ls_->ctx(), "inline_entry", func);
    b().SetInsertPoint(entry);

    info_.set_args(args);
    info_.set_previous(prev);
    info_.set_entry(entry);

    BasicBlock* body = BasicBlock::Create(ls_->ctx(), "method_body", func);
    pass_one(body);

    BasicBlock* alloca_block = &func->getEntryBlock();

    Value* cfstk = new AllocaInst(obj_type,
        ConstantInt::get(ls_->Int32Ty,
          (sizeof(CallFrame) / sizeof(Object*)) + vmm_->stack_size),
        "cfstk", alloca_block->getTerminator());

    call_frame = b().CreateBitCast(
        cfstk,
        llvm::PointerType::getUnqual(cf_type), "call_frame");

    stk = b().CreateConstGEP1_32(cfstk, sizeof(CallFrame) / sizeof(Object*), "stack");

    info_.set_call_frame(call_frame);
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

    Value* mod = b().CreateLoad(
        b().CreateConstGEP2_32(rd, 0, offset::runtime_data_module, "module_pos"),
        "module");

    setup_inline_scope(self, blk, mod);

    // We know the right arguments are present, so we just need to put them
    // in the right place.
    //
    // We don't support splat in an inlined method!
    assert(vmm_->splat_position < 0);

    assert(stack_args.size() <= (size_t)vmm_->total_args);

    for(size_t i = 0; i < stack_args.size(); i++) {
      Value* int_pos = ConstantInt::get(ls_->Int32Ty, i);

      Value* idx2[] = {
        ConstantInt::get(ls_->Int32Ty, 0),
        ConstantInt::get(ls_->Int32Ty, offset::vars_tuple),
        int_pos
      };

      Value* pos = b().CreateGEP(vars, idx2, idx2+3, "local_pos");

      b().CreateStore(stack_args[i], pos);
    }

    b().CreateBr(body);
    b().SetInsertPoint(body);

    return entry;
  }

  void InlineMethodBuilder::setup_inline_scope(Value* self, Value* blk, Value* mod) {
    Value* heap_null = ConstantExpr::getNullValue(llvm::PointerType::getUnqual(vars_type));
    Value* heap_pos = get_field(vars, offset::vars_on_heap);
    b().CreateStore(heap_null, heap_pos);

    b().CreateStore(self, get_field(vars, offset::vars_self));
    b().CreateStore(mod, get_field(vars, offset::vars_module));

    b().CreateStore(blk, get_field(vars, offset::vars_block));

    b().CreateStore(Constant::getNullValue(ls_->ptr_type("VariableScope")),
        get_field(vars, offset::vars_parent));
    b().CreateStore(constant(Qnil, obj_type), get_field(vars, offset::vars_last_match));

    nil_locals();
  }

}
}

#endif
