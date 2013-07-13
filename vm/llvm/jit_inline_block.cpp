#ifdef ENABLE_LLVM

#include <sys/param.h>
#include "llvm/jit_inline_block.hpp"
#include "llvm/stack_args.hpp"
#include "llvm/method_info.hpp"
#include "version.h"

#include "call_frame.hpp"

using namespace llvm;

namespace rubinius {
namespace jit {
  BasicBlock* InlineBlockBuilder::setup_inline_block(Value* self, Value* mod,
                                    JITStackArgs& stack_args)
  {
    llvm::Value* prev = info_.parent_call_frame();
    llvm::Value* args = ConstantExpr::getNullValue(ctx_->ptr_type("Arguments"));

    BasicBlock* entry = BasicBlock::Create(ctx_->llvm_context(), "inline_entry", info_.function());
    b().SetInsertPoint(entry);

    info_.set_args(args);
    info_.set_previous(prev);
    info_.set_entry(entry);

    BasicBlock* body = BasicBlock::Create(ctx_->llvm_context(), "block_body", info_.function());
    pass_one(body);

    BasicBlock* alloca_block = &info_.function()->getEntryBlock();

    Value* cfstk = new AllocaInst(obj_type,
        cint((sizeof(CallFrame) / sizeof(Object*)) + machine_code_->stack_size),
        "cfstk", alloca_block->getTerminator());

    call_frame = b().CreateBitCast(
        cfstk,
        llvm::PointerType::getUnqual(cf_type), "call_frame");

    info_.set_call_frame(call_frame);

    stk = b().CreateConstGEP1_32(cfstk, sizeof(CallFrame) / sizeof(Object*), "stack");

    info_.set_stack(stk);

    Value* var_mem = new AllocaInst(obj_type,
        cint((sizeof(StackVariables) / sizeof(Object*)) + machine_code_->number_of_locals),
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
        b().CreateConstGEP2_32(info_.creator_info()->call_frame(), 0, offset::CallFrame::constant_scope, "constant_scope_pos"),
        "constant_scope");

    Value* constant_scope_gep = get_field(call_frame, offset::CallFrame::constant_scope);
    b().CreateStore(constant_scope, constant_scope_gep);

    // flags
    int flags = CallFrame::cInlineFrame | CallFrame::cInlineBlock;
    if(!use_full_scope_) flags |= CallFrame::cClosedScope;

    b().CreateStore(cint(flags),
        get_field(call_frame, offset::CallFrame::flags));

    // ip
    b().CreateStore(cint(0),
        get_field(call_frame, offset::CallFrame::ip));

    // scope
    b().CreateStore(vars, get_field(call_frame, offset::CallFrame::scope));

    nil_stack(machine_code_->stack_size, constant(cNil, obj_type));

    setup_inline_scope(self, constant(cNil, obj_type), mod);

    if(!LANGUAGE_18_ENABLED) {
      // We don't support splat in an block method!
      assert(machine_code_->splat_position < 0);

      if(stack_args.size() == 1 && machine_code_->total_args > 1) {
        Signature sig(ctx_, "Object");
        sig << "State";
        sig << "CallFrame";
        sig << "Object";
        sig << vars->getType();
        sig << ctx_->Int32Ty;

        Value* call_args[] = { info_.state(), info_.call_frame(),
                               stack_args.at(0), vars, cint(machine_code_->total_args) };

        Value* val = sig.call("rbx_destructure_inline_args", call_args, 5, "", b());
        Value* null = Constant::getNullValue(val->getType());
        Value* is_null = b().CreateICmpEQ(val, null);

        info_.add_return_value(null, b().GetInsertBlock());

        BasicBlock* cont = info_.new_block("continue");
        b().CreateCondBr(is_null, info_.return_pad(), cont);
        b().SetInsertPoint(cont);
      } else {
        // block logic has no arity checking, so we process
        // up to the minimum of stack_args.size and machine_code_->total_args;
        size_t limit = MIN((int)stack_args.size(), (int)machine_code_->total_args);

        for(size_t i = 0; i < limit; i++) {
          Value* int_pos = cint(i);

          Value* idx2[] = {
            cint(0),
            cint(offset::StackVariables::locals),
            int_pos
          };

          Value* pos = b().CreateGEP(vars, idx2, "local_pos");

          b().CreateStore(stack_args.at(i), pos);
        }
      }
    } else {
      // No argument handling, there are bytecodes in the body that
      // do that. We just have to make stack_args available.
    }

    b().CreateBr(body);
    b().SetInsertPoint(body);

    return entry;
  }


}
}

#endif
