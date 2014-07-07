#ifdef ENABLE_LLVM

#include <sys/param.h>
#include "llvm/jit_inline_block.hpp"
#include "llvm/stack_args.hpp"
#include "llvm/method_info.hpp"

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

    // We don't support splat in an block method!
    assert(machine_code_->splat_position < 0);

    assign_arguments(stack_args);

    b().CreateBr(body);
    b().SetInsertPoint(body);

    return entry;
  }

  void InlineBlockBuilder::assign_fixed_arguments(JITStackArgs& stack_args,
      int local_start, int local_end, int arg_start)
  {
    for(int l = local_start, a = arg_start; l < local_end; l++, a++) {
      Value* idx[] = {
        cint(0),
        cint(offset::StackVariables::locals),
        cint(l)
      };

      b().CreateStore(stack_args.at(a), b().CreateGEP(vars, idx));
    }
  }

  void InlineBlockBuilder::assign_arguments(JITStackArgs& stack_args) {
    const native_int T = machine_code_->total_args;
    const native_int M = machine_code_->required_args;
    const native_int P  = machine_code_->post_args;
    const native_int H  = M - P;

    if(stack_args.size() >= T && !machine_code_->keywords) {
      // head arguments
      if(H > 0) {
        assign_fixed_arguments(stack_args, 0, H, 0);
      }

      const native_int N = stack_args.size();
      const native_int O = T - M;
      const native_int PI = H + O;

      // optional arguments
      if(O > 0) {
        assign_fixed_arguments(stack_args, H, H + O, H);
      }

      // post arguments
      if(P > 0) {
        assign_fixed_arguments(stack_args, PI, PI + P, N - P);
      }

      return;
    }

    BasicBlock* alloca_block = &info_.function()->getEntryBlock();

    Value* N;
    Value* args_array;

    // Case: yield x -> m { |a, b, ...| }
    if(stack_args.size() == 1 && T > 1) {
      args_array = new AllocaInst(obj_type, cint(T),
          "args_array", alloca_block->getTerminator());

      Signature sig(ctx_, ctx_->Int32Ty);
      sig << "State";
      sig << "CallFrame";
      sig << "Object";
      sig << args_array->getType();
      sig << ctx_->Int32Ty;

      Value* call_args[] = {
        info_.state(),
        info_.call_frame(),
        stack_args.at(0),
        args_array,
        cint(T)
      };

      Value* size = sig.call("rbx_destructure_inline_args", call_args, 5, "", b());

      info_.add_return_value(Constant::getNullValue(obj_type),
          b().GetInsertBlock());

      BasicBlock* cont = info_.new_block("continue");

      Value* is_zero = b().CreateICmpEQ(size, cint(0));
      b().CreateCondBr(is_zero, info_.return_pad(), cont);

      N = b().CreateSelect(
          b().CreateICmpSLT(cint(T), size),
          cint(T), size);

      b().SetInsertPoint(cont);
    } else {
      args_array = new AllocaInst(obj_type, cint(stack_args.size()),
          "args_array", alloca_block->getTerminator());

      for(int i = 0; i < stack_args.size(); i++) {
        b().CreateStore(stack_args.at(i),
            b().CreateGEP(args_array, cint(i), "args_array_pos"));
      }

      N = b().CreateSelect(
          b().CreateICmpSLT(
            cint(T), cint(stack_args.size())),
          cint(T), cint(stack_args.size()));
    }

    Value* keyword_object = b().CreateAlloca(obj_type, 0, "keyword_object");
    Value* local_index = b().CreateAlloca(ctx_->Int32Ty, 0, "local_index");
    Value* arg_index = b().CreateAlloca(ctx_->Int32Ty, 0, "args_index");

    Value* null = Constant::getNullValue(obj_type);
    b().CreateStore(null, keyword_object);

    if(machine_code_->keywords) {
      BasicBlock* kw_check = info_.new_block("kw_check_object");
      BasicBlock* after_kw = info_.new_block("after_kw_check");

      b().CreateCondBr(
          b().CreateICmpSGT(N, cint(M)),
          kw_check, after_kw);

      b().SetInsertPoint(kw_check);

      Value* kw_arg = b().CreateLoad(
            b().CreateGEP(args_array,
              b().CreateSub(N, cint(1))));

      Signature sig(ctx_, "Object");
      sig << "State";
      sig << "CallFrame";
      sig << "Object";

      Value* call_args[] = {
        info_.state(),
        info_.previous(),
        kw_arg
      };

      Value* keyword_val = sig.call("rbx_check_keyword",
          call_args, 3, "keyword_val", b());

      b().CreateStore(keyword_val, keyword_object);

      b().CreateBr(after_kw);
      b().SetInsertPoint(after_kw);
    }

    // K = (keyword_object && N > M) ? 1 : 0;
    Value* K = b().CreateSelect(
        b().CreateAnd(
          b().CreateICmpNE(b().CreateLoad(keyword_object), null),
          b().CreateICmpSGT(N, cint(M))),
        cint(1), cint(0));

    // O = T - M - (mcode->keywords ? 1 : 0);
    Value* O = cint(T - M - (machine_code_->keywords ? 1 : 0));

    // E = N - M - K;
    Value* N_M_K = b().CreateSub(N, b().CreateAdd(cint(M), K));
    Value* E = b().CreateSelect(b().CreateICmpSGE(N_M_K, cint(0)), N_M_K, cint(0));

    // ON = (X = MIN(O, E)) > 0 ? X : 0;
    Value* X = b().CreateSelect(b().CreateICmpSLE(O, E), O, E);
    Value* ON = b().CreateSelect(b().CreateICmpSGT(X, cint(0)), X, cint(0));

    // head arguments
    {
      BasicBlock* pre = info_.new_block("pre_head_args");
      BasicBlock* top = info_.new_block("top_head_args");
      BasicBlock* body = info_.new_block("body_head_args");
      BasicBlock* after = info_.new_block("after_head_args");

      b().CreateCondBr(
          b().CreateICmpSGT(cint(H), cint(0)),
          pre, after);

      b().SetInsertPoint(pre);

      b().CreateStore(cint(0), local_index);
      b().CreateStore(cint(0), arg_index);

      b().CreateBr(top);
      b().SetInsertPoint(top);

      Value* local_i = b().CreateLoad(local_index, "local_index");
      Value* arg_i = b().CreateLoad(arg_index, "arg_index");

      // local_index < H + O && arg_index < H + ON
      Value* loop_test = b().CreateAnd(
          b().CreateICmpSLT(local_i, cint(H)),
          b().CreateICmpSLT(arg_i, N));

      b().CreateCondBr(loop_test, body, after);

      b().SetInsertPoint(body);

      Value* idx[] = {
        cint(0),
        cint(offset::StackVariables::locals),
        local_i
      };

      // locals[local_index] = args[local_index]
      b().CreateStore(
          b().CreateLoad(
            b().CreateGEP(args_array, arg_i)),
          b().CreateGEP(vars, idx));

      // local_index++
      b().CreateStore(b().CreateAdd(local_i, cint(1)), local_index);

      // arg_index++
      b().CreateStore(b().CreateAdd(arg_i, cint(1)), arg_index);

      b().CreateBr(top);

      b().SetInsertPoint(after);
    }

    // arg limit = H + ON
    Value* H_ON = b().CreateAdd(cint(H), ON);

    BasicBlock* pre_opts = info_.new_block("pre_optional_args");
    BasicBlock* post_opts = info_.new_block("post_optional_args");

    b().CreateCondBr(
        b().CreateAnd(
          b().CreateICmpSGT(O, cint(0)),
          b().CreateICmpSGT(ON, cint(0))),
        pre_opts, post_opts);

    // optional arguments
    // for(l = a = H; l < H + O && a < H + ON; l++, a++)

    b().SetInsertPoint(pre_opts);

    // local limit = H + O
    Value* H_O = b().CreateAdd(cint(H), O);

    {
      BasicBlock* top = info_.new_block("opt_arg_loop_top");
      BasicBlock* body = info_.new_block("opt_arg_loop_body");
      BasicBlock* after = info_.new_block("opt_arg_loop_cont");

      // local_index = arg_index = H
      b().CreateStore(cint(H), local_index);
      b().CreateStore(cint(H), arg_index);

      b().CreateBr(top);
      b().SetInsertPoint(top);

      Value* local_i = b().CreateLoad(local_index, "local_index");
      Value* arg_i = b().CreateLoad(arg_index, "arg_index");

      // local_index < H + O && arg_index < H + ON
      Value* loop_test = b().CreateAnd(
          b().CreateICmpSLT(local_i, H_O),
          b().CreateICmpSLT(arg_i, H_ON));

      b().CreateCondBr(loop_test, body, after);

      b().SetInsertPoint(body);

      Value* idx[] = {
        cint(0),
        cint(offset::StackVariables::locals),
        local_i
      };

      // locals[local_index] = args[local_index]
      b().CreateStore(
          b().CreateLoad(
            b().CreateGEP(args_array, arg_i)),
          b().CreateGEP(vars, idx));

      // local_index++
      b().CreateStore(b().CreateAdd(local_i, cint(1)), local_index);

      // arg_index++
      b().CreateStore(b().CreateAdd(arg_i, cint(1)), arg_index);

      b().CreateBr(top);

      b().SetInsertPoint(after);
    }

    // if ON < O : more optional parameters than arguments available
    {
      BasicBlock* pre = info_.new_block("missing_opt_arg_loop_pre");
      BasicBlock* top = info_.new_block("missing_opt_arg_loop_top");
      BasicBlock* body = info_.new_block("missing_opt_arg_loop_body");
      BasicBlock* after = info_.new_block("missing_opt_arg_loop_cont");

      b().CreateCondBr(
          b().CreateICmpSLT(ON, O),
          pre, post_opts);

      b().SetInsertPoint(pre);

      b().CreateStore(H_ON, local_index);

      Type* undef_type = llvm::PointerType::getUnqual(obj_type);
      Object** addr = ctx_->llvm_state()->shared().globals.undefined.object_address();
      Value* undef = b().CreateLoad(constant(addr, undef_type));

      b().CreateBr(top);
      b().SetInsertPoint(top);

      Value* local_i = b().CreateLoad(local_index, "local_index");

      b().CreateCondBr(
          b().CreateICmpSLT(local_i, H_O),
          body, after);

      b().SetInsertPoint(body);

      Value* idx[] = {
        cint(0),
        cint(offset::StackVariables::locals),
        local_i
      };

      // locals[local_index] = undefined
      b().CreateStore(
          undef,
          b().CreateGEP(vars, idx));

      // local_index++
      b().CreateStore(b().CreateAdd(local_i, cint(1)), local_index);

      b().CreateBr(top);

      b().SetInsertPoint(after);
      b().CreateBr(post_opts);
    }

    b().SetInsertPoint(post_opts);

    BasicBlock* post_args = info_.new_block("post_args");

    b().CreateBr(post_args);
    b().SetInsertPoint(post_args);

    BasicBlock* kw_args = info_.new_block("kw_args");

    if(P > 0) {
      // post arguments
      // PI = H + O
      Value* PI = H_O;

      // l = PI; l < PI + P && a < N - K
      Value* PI_P = b().CreateAdd(PI, cint(P));
      Value* N_K = b().CreateSub(N, K);

      // local_index = PI
      b().CreateStore(PI, local_index);

      // arg_index_ = N - P - K
      b().CreateStore(
          b().CreateSub(
            b().CreateSub(N, cint(P)), K),
          arg_index);

      {
        BasicBlock* top = info_.new_block("post_arg_loop_top");
        BasicBlock* body = info_.new_block("post_arg_loop_body");
        BasicBlock* after = info_.new_block("post_arg_loop_cont");

        b().CreateBr(top);
        b().SetInsertPoint(top);

        Value* local_i = b().CreateLoad(local_index, "local_index");
        Value* arg_i = b().CreateLoad(arg_index, "arg_index");

        // l < PI + P && a < N - K
        Value* loop_test = b().CreateAnd(
            b().CreateICmpSLT(local_i, PI_P),
            b().CreateICmpSLT(arg_i, N_K));

        b().CreateCondBr(loop_test, body, after);

        b().SetInsertPoint(body);

        Value* idx[] = {
          cint(0),
          cint(offset::StackVariables::locals),
          local_i
        };

        // locals[local_index] = args[local_index]
        b().CreateStore(
            b().CreateLoad(
              b().CreateGEP(args_array, arg_i)),
            b().CreateGEP(vars, idx));

        // local_index++
        b().CreateStore(b().CreateAdd(local_i, cint(1)), local_index);

        // arg_index++
        b().CreateStore(b().CreateAdd(arg_i, cint(1)), arg_index);

        b().CreateBr(top);

        b().SetInsertPoint(after);
      }
    }

    b().CreateBr(kw_args);
    b().SetInsertPoint(kw_args);

    // keywords
    {
      BasicBlock* before_kw = info_.new_block("before_keywords");
      BasicBlock* after_kw = info_.new_block("after_keywords");

      Value* null = Constant::getNullValue(obj_type);
      Value* kw_val = b().CreateLoad(keyword_object, "kw_val");

      b().CreateCondBr(
          b().CreateICmpNE(kw_val, null),
          before_kw, after_kw);

      b().SetInsertPoint(before_kw);

      Value* idx[] = {
        cint(0),
        cint(offset::StackVariables::locals),
        cint(T - 1)
      };

      b().CreateStore(
          b().CreateLoad(keyword_object),
          b().CreateGEP(vars, idx));

      b().CreateBr(after_kw);

      b().SetInsertPoint(after_kw);
    }
  }
}
}

#endif
