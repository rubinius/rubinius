#ifdef ENABLE_LLVM

#include "llvm/jit_inline_method.hpp"
#include "llvm/method_info.hpp"

#include "call_frame.hpp"

using namespace llvm;

namespace rubinius {
namespace jit {
  BasicBlock* InlineMethodBuilder::setup_inline(Value* self, Value* blk,
      std::vector<Value*>& stack_args)
  {
    llvm::Value* prev = info_.parent_call_frame();
    llvm::Value* args = ConstantExpr::getNullValue(ctx_->ptr_type("Arguments"));

    BasicBlock* entry = BasicBlock::Create(ctx_->llvm_context(), "inline_entry", info_.function());
    b().SetInsertPoint(entry);

    info_.set_args(args);
    info_.set_previous(prev);
    info_.set_entry(entry);

    BasicBlock* body = BasicBlock::Create(ctx_->llvm_context(), "method_body", info_.function());
    pass_one(body);

    BasicBlock* alloca_block = &info_.function()->getEntryBlock();

    Value* cfstk = new AllocaInst(obj_type,
        ConstantInt::get(ctx_->Int32Ty,
          (sizeof(CallFrame) / sizeof(Object*)) + machine_code_->stack_size),
        "cfstk", alloca_block->getTerminator());

    call_frame = b().CreateBitCast(
        cfstk,
        llvm::PointerType::getUnqual(cf_type), "call_frame");

    stk = b().CreateConstGEP1_32(cfstk, sizeof(CallFrame) / sizeof(Object*), "stack");

    info_.set_call_frame(call_frame);
    info_.set_stack(stk);

    Value* var_mem = new AllocaInst(obj_type,
        ConstantInt::get(ctx_->Int32Ty,
          (sizeof(StackVariables) / sizeof(Object*)) + machine_code_->number_of_locals),
        "var_mem", alloca_block->getTerminator());

    vars = b().CreateBitCast(
        var_mem,
        llvm::PointerType::getUnqual(stack_vars_type), "vars");

    info_.set_variables(vars);

    Value* rd = constant(runtime_data_, ctx_->ptr_type("jit::RuntimeData"));

    //  Setup the CallFrame
    //
    // previous
    b().CreateStore(prev, get_field(call_frame, offset::CallFrame::previous));

    // msg
    b().CreateStore(
        b().CreatePointerCast(rd, ctx_->Int8PtrTy),
        get_field(call_frame, offset::CallFrame::dispatch_data));

    // compiled_code
    method = b().CreateLoad(
        b().CreateConstGEP2_32(rd, 0, offset::jit_RuntimeData::method, "method_pos"),
        "compiled_code");

    Value* code_gep = get_field(call_frame, offset::CallFrame::compiled_code);
    b().CreateStore(method, code_gep);

    // constant_scope
    Value* constant_scope = b().CreateLoad(
        b().CreateConstGEP2_32(method, 0, offset::CompiledCode::scope, "constant_scope_pos"),
        "constant_scope");

    Value* constant_scope_gep = get_field(call_frame, offset::CallFrame::constant_scope);
    b().CreateStore(constant_scope, constant_scope_gep);

    // flags
    int flags = CallFrame::cInlineFrame;
    if(!use_full_scope_) flags |= CallFrame::cClosedScope;

    b().CreateStore(cint(flags),
        get_field(call_frame, offset::CallFrame::flags));

    // ip
    b().CreateStore(cint(0),
        get_field(call_frame, offset::CallFrame::ip));

    // scope
    b().CreateStore(vars, get_field(call_frame, offset::CallFrame::scope));

    nil_stack(machine_code_->stack_size, constant(cNil, obj_type));

    Value* mod = b().CreateLoad(
        b().CreateConstGEP2_32(rd, 0, offset::jit_RuntimeData::module, "module_pos"),
        "module");

    setup_inline_scope(self, blk, mod);

    // We know the right arguments are present, so we just need to put them
    // in the right place.
    //
    // We don't support splat in an inlined method!
    assert(machine_code_->splat_position < 0);

    assert(stack_args.size() <= (size_t)machine_code_->total_args);

    for(size_t i = 0; i < stack_args.size(); i++) {
      Value* int_pos = cint(i);

      Value* idx2[] = {
        cint(0),
        cint(offset::StackVariables::locals),
        int_pos
      };

      Value* pos = b().CreateGEP(vars, idx2, "local_pos");

      Value* arg_val = stack_args.at(i);

      LocalInfo* li = info_.get_local(i);
      li->make_argument();

      if(ctx_->llvm_state()->type_optz()) {
        if(type::KnownType::has_hint(ctx_, arg_val)) {
          type::KnownType kt = type::KnownType::extract(ctx_, arg_val);
          li->set_known_type(kt);
        }
      }

      b().CreateStore(arg_val, pos);
    }

    b().CreateBr(body);
    b().SetInsertPoint(body);

    return entry;
  }

  void InlineMethodBuilder::setup_inline_scope(Value* self, Value* blk, Value* mod) {
    Value* heap_null = ConstantExpr::getNullValue(llvm::PointerType::getUnqual(vars_type));
    Value* heap_pos = get_field(vars, offset::StackVariables::on_heap);
    b().CreateStore(heap_null, heap_pos);

    b().CreateStore(self, get_field(vars, offset::StackVariables::self));
    b().CreateStore(mod, get_field(vars, offset::StackVariables::module));

    b().CreateStore(blk, get_field(vars, offset::StackVariables::block));

    b().CreateStore(Constant::getNullValue(ctx_->ptr_type("VariableScope")),
        get_field(vars, offset::StackVariables::parent));
    b().CreateStore(constant(cNil, obj_type), get_field(vars, offset::StackVariables::last_match));

    nil_locals();
  }

}
}

#endif
