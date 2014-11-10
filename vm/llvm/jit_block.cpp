#ifdef ENABLE_LLVM

#include "llvm/jit_block.hpp"
#include "llvm/jit_context.hpp"
#include "llvm/method_info.hpp"

#include "call_frame.hpp"
#include "stack_variables.hpp"
#include "builtin/constant_scope.hpp"
#include "builtin/module.hpp"

#include "instruments/tooling.hpp"

using namespace llvm;

namespace rubinius {
namespace jit {

  void BlockBuilder::setup() {
    std::vector<Type*> ftypes;
    ftypes.push_back(ctx_->ptr_type("State"));
    ftypes.push_back(ctx_->ptr_type("CallFrame"));
    ftypes.push_back(ctx_->ptr_type("BlockEnvironment"));
    ftypes.push_back(ctx_->ptr_type("Arguments"));
    ftypes.push_back(ctx_->ptr_type("BlockInvocation"));

    FunctionType* ft = FunctionType::get(ctx_->ptr_type("Object"), ftypes, false);

    std::ostringstream ss;
    ss << std::string("_X_")
       << ctx_->llvm_state()->enclosure_name(info_.method())
       << "#"
       << ctx_->llvm_state()->symbol_debug_str(info_.method()->name())
       << "$block@"
       << ++ctx_->llvm_state()->vm()->metrics()->m.jit_metrics.methods_compiled;

    llvm::Function* func = Function::Create(ft, GlobalValue::ExternalLinkage,
                            ss.str(), ctx_->module());

    Function::arg_iterator ai = func->arg_begin();
    llvm::Value* state = ai++; state->setName("state");
    llvm::Value* prev = ai++; prev->setName("previous");
    block_env = ai++; block_env->setName("env");
    llvm::Value* args = ai++; args->setName("args");
    block_inv = ai++; block_inv->setName("invocation");

    BasicBlock* block = BasicBlock::Create(ctx_->llvm_context(), "entry", func);
    b().SetInsertPoint(block);

    ctx_->set_function(func);

    info_.set_state(state);
    info_.set_args(args);
    info_.set_previous(prev);
    info_.set_entry(block);

    alloc_frame("block_body");

    check_arity();

    initialize_frame(machine_code_->stack_size);

    nil_stack(machine_code_->stack_size, constant(cNil, obj_type));

    setup_block_scope();

    import_args();

    import_args_ = b().GetInsertBlock();

    if(ctx_->llvm_state()->include_profiling()) {
      Value* test = b().CreateLoad(ctx_->profiling(), "profiling");

      BasicBlock* setup_profiling = BasicBlock::Create(ctx_->llvm_context(), "setup_profiling", func);
      BasicBlock* cont = BasicBlock::Create(ctx_->llvm_context(), "continue", func);

      b().CreateCondBr(test, setup_profiling, cont);

      b().SetInsertPoint(setup_profiling);

      Signature sig(ctx_, ctx_->VoidTy);
      sig << "State";
      sig << llvm::PointerType::getUnqual(ctx_->Int8Ty);
      sig << "BlockEnvironment";
      sig << "Module";
      sig << "CompiledCode";

      Value* call_args[] = {
        state,
        method_entry_,
        block_env,
        module_,
        method
      };

      sig.call("rbx_begin_profiling_block", call_args, 5, "", b());

      b().CreateBr(cont);

      b().SetInsertPoint(cont);
    }

    b().CreateBr(body_);
    b().SetInsertPoint(body_);
  }

  void BlockBuilder::setup_block_scope() {
    b().CreateStore(ConstantExpr::getNullValue(llvm::PointerType::getUnqual(vars_type)),
        get_field(vars, offset::StackVariables::on_heap));
    Value* self = b().CreateLoad(
        get_field(block_inv, offset::BlockInvocation::self),
        "invocation.self");

    b().CreateStore(self, get_field(vars, offset::StackVariables::self));

    Value* inv_mod = b().CreateLoad(
        get_field(block_inv, offset::BlockInvocation::module),
        "invocation.module");

    Value* creation_mod = b().CreateLoad(
        get_field(block_env, offset::BlockEnvironment::module),
        "env.module");

    Value* mod = b().CreateSelect(
        b().CreateICmpNE(inv_mod, ConstantExpr::getNullValue(inv_mod->getType())),
        inv_mod,
        creation_mod);

    module_ = mod;

    b().CreateStore(mod, get_field(vars, offset::StackVariables::module));

    Value* blk = b().CreateLoad(get_field(top_scope, offset::VariableScope::block),
        "args.block");
    b().CreateStore(blk, get_field(vars, offset::StackVariables::block));


    // We don't use top_scope here because of nested blocks. Parent MUST be
    // the scope the block was created in, not the top scope for depth
    // variables to work.
    Value* be_scope = b().CreateLoad(
        get_field(block_env, offset::BlockEnvironment::scope),
        "env.scope");

    b().CreateStore(be_scope, get_field(vars, offset::StackVariables::parent));
    b().CreateStore(constant(cNil, obj_type), get_field(vars, offset::StackVariables::last_match));

    nil_locals();
  }

  void BlockBuilder::initialize_frame(int stack_size) {
    Value* code_gep = get_field(call_frame, offset::CallFrame::compiled_code);

    method = b().CreateLoad(get_field(block_env, offset::BlockEnvironment::code),
                            "env.code");

    // previous
    b().CreateStore(info_.previous(), get_field(call_frame, offset::CallFrame::previous));

    // constant_scope
    Value* cs = b().CreateLoad(get_field(block_inv, offset::BlockInvocation::constant_scope),
                               "invocation.constant_scope");

    b().CreateStore(cs, get_field(call_frame, offset::CallFrame::constant_scope));

    // arguments
    b().CreateStore(info_.args(), get_field(call_frame, offset::CallFrame::arguments));

    // msg
    b().CreateStore(Constant::getNullValue(ctx_->Int8PtrTy),
        get_field(call_frame, offset::CallFrame::dispatch_data));

    // compiled_code
    b().CreateStore(method, code_gep);

    // flags
    inv_flags_ = b().CreateLoad(get_field(block_inv, offset::BlockInvocation::flags),
        "invocation.flags");

    int block_flags = CallFrame::cMultipleScopes |
      CallFrame::cBlock |
      CallFrame::cJITed;

    if(!use_full_scope_) block_flags |= CallFrame::cClosedScope;

    Value* flags = b().CreateOr(inv_flags_,
        cint(block_flags), "flags");

    b().CreateStore(flags, get_field(call_frame, offset::CallFrame::flags));

    // ip
    b().CreateStore(cint(0),
        get_field(call_frame, offset::CallFrame::ip));

    // scope
    b().CreateStore(vars, get_field(call_frame, offset::CallFrame::scope));

    // top_scope
    top_scope = b().CreateLoad(
        get_field(block_env, offset::BlockEnvironment::top_scope),
        "env.top_scope");

    b().CreateStore(top_scope, get_field(call_frame, offset::CallFrame::top_scope));

    // jit_data
    b().CreateStore(
        constant(ctx_->runtime_data_holder(), ctx_->Int8PtrTy),
        get_field(call_frame, offset::CallFrame::jit_data));

  }

  void BlockBuilder::check_arity() {
    BasicBlock* destruct_check = info_.new_block("destruct_check");
    BasicBlock* arg_error = info_.new_block("arg_error");
    BasicBlock* cont = info_.new_block("import_args");

    const native_int T = machine_code_->total_args;
    const native_int M = machine_code_->required_args;
    const native_int RI = machine_code_->splat_position;
    const bool RP = (RI >= 0);

    Value* lambda_flag = cint(CallFrame::cIsLambda);
    Value* flags = b().CreateLoad(
        get_field(block_inv, offset::BlockInvocation::flags),
        "invocation.flags");
    Value* not_lambda = b().CreateICmpNE(
        b().CreateAnd(flags, lambda_flag), lambda_flag);

    if(T == 0) {
      if(RP) {
        b().CreateBr(cont);
        b().SetInsertPoint(cont);
        return;
      }

      BasicBlock* arity_check = info_.new_block("arity_check");

      b().CreateCondBr(not_lambda, cont, arity_check);

      b().SetInsertPoint(arity_check);

      Value* N = b().CreateLoad(
          b().CreateConstGEP2_32(
            info_.args(), 0, offset::Arguments::total));
      Value* cmp = b().CreateICmpNE(N, cint(0), "arg_cmp");

      b().CreateCondBr(cmp, arg_error, cont);
    } else {
      b().CreateBr(destruct_check);
    }

    b().SetInsertPoint(destruct_check);

    BasicBlock* n_lt_m = info_.new_block("n_lt_m");

    if(T > 1 || (RP && T > 0) || RI < -2) {
      BasicBlock* destruct = info_.new_block("destructure");
      BasicBlock* destruct_cont = info_.new_block("destructure_contuation");

      Value* N = b().CreateLoad(
          b().CreateConstGEP2_32(
            info_.args(), 0, offset::Arguments::total));

      b().CreateCondBr(b().CreateAnd(
            b().CreateICmpEQ(N, cint(1)), not_lambda),
          destruct, n_lt_m);

      b().SetInsertPoint(destruct);

      Signature sig(ctx_, ctx_->Int32Ty);
      sig << "State";
      sig << "CallFrame";
      sig << "Arguments";

      Value* call_args[] = {
        info_.state(),
        info_.call_frame(),
        info_.args()
      };

      Value* val = sig.call("rbx_destructure_args", call_args, 3, "", b());

      Value* is_error = b().CreateICmpEQ(val, cint(-1));

      info_.add_return_value(Constant::getNullValue(obj_type), b().GetInsertBlock());
      b().CreateCondBr(is_error, info_.return_pad(), destruct_cont);

      b().SetInsertPoint(destruct_cont);
      Value* idx[] = {
        cint(0),
        cint(offset::Arguments::total),
      };

      b().CreateStore(val, b().CreateGEP(info_.args(), idx));
    }

    b().CreateBr(n_lt_m);
    b().SetInsertPoint(n_lt_m);

    Value* lambda = b().CreateICmpEQ(
        b().CreateAnd(flags, lambda_flag), lambda_flag);

    BasicBlock* n_gt_t = info_.new_block("not_rp_and_n_gt_t");

    // N < M
    Value* cmp = b().CreateAnd(lambda,
        b().CreateICmpSLT(
          b().CreateLoad(
            b().CreateConstGEP2_32(
              info_.args(), 0, offset::Arguments::total)),
          cint(M), "arg_cmp"));
    b().CreateCondBr(cmp, arg_error, n_gt_t);

    b().SetInsertPoint(n_gt_t);

    // !RP && N > T
    if(!RP) {
      BasicBlock* update_n = info_.new_block("update_n");

      Value* N = b().CreateLoad(
          b().CreateConstGEP2_32(
            info_.args(), 0, offset::Arguments::total));

      Value* cmp = b().CreateAnd(
          lambda,
          b().CreateICmpSGT(N, cint(T), "arg_cmp"));

      b().CreateCondBr(cmp, arg_error, update_n);

      b().SetInsertPoint(update_n);
    }

    if(machine_code_->keywords) {
      BasicBlock* kw_check = info_.new_block("kw_check_object");

      Value* N = b().CreateLoad(
          b().CreateConstGEP2_32(
            info_.args(), 0, offset::Arguments::total));

      Value* null = Constant::getNullValue(obj_type);

      b().CreateStore(null, keyword_object_);

      b().CreateCondBr(
          b().CreateICmpSGT(N, cint(M)),
          kw_check, cont);

      b().SetInsertPoint(kw_check);

      Value* arg_ary = b().CreateLoad(
          b().CreateConstGEP2_32(info_.args(), 0,
            offset::Arguments::arguments, "arg_ary_pos"),
          "arg_ary");

      // RP || (!RP && N < T) ? N - 1 : T
      Value* kw_index = b().CreateSelect(
          b().CreateOr(
            b().CreateICmpEQ(cint(RP), cint(true)),
            b().CreateAnd(
              b().CreateICmpEQ(cint(RP), cint(false)),
              b().CreateICmpSLT(N, cint(T)))),
          b().CreateSub(N, cint(1)),
          cint(T));

      Value* kw_arg = b().CreateLoad(
          b().CreateGEP(arg_ary, kw_index));

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
          call_args, 3, "rbx_check_keyword", b());

      b().CreateStore(keyword_val, keyword_object_);

      if(!RP) {
        BasicBlock* after_kw_arity = info_.new_block("after_kw_arity");

        b().CreateCondBr(
            b().CreateAnd(
              lambda,
              b().CreateAnd(
                b().CreateICmpEQ(keyword_val, null),
                b().CreateICmpSGE(N, cint(T)))),
            arg_error, after_kw_arity);

        b().SetInsertPoint(after_kw_arity);
      }
    }

    b().CreateBr(cont);

    b().SetInsertPoint(arg_error);

    // Call our arg_error helper
    Signature sig(ctx_, "Object");

    sig << "State";
    sig << "CallFrame";
    sig << "Arguments";
    sig << ctx_->Int32Ty;

    Value* call_args[] = {
      info_.state(),
      info_.previous(),
      info_.args(),
      cint(machine_code_->total_args)
    };

    Value* val = sig.call("rbx_arg_error", call_args, 4, "rbx_arg_error", b());
    info_.add_return_value(val, b().GetInsertBlock());
    b().CreateBr(info_.return_pad());

    b().SetInsertPoint(cont);
  }

  void BlockBuilder::assign_fixed_args(Value* arg_ary, int start, int end) {
    for(int i = start; i < end; i++) {
      Value* int_pos = cint(i);

      Value* arg_val_offset = b().CreateConstGEP1_32(arg_ary, i, "arg_val_offset");

      Value* arg_val = b().CreateLoad(arg_val_offset, "arg_val");

      Value* idx2[] = {
        cint(0),
        cint(offset::StackVariables::locals),
        int_pos
      };

      Value* pos = b().CreateGEP(vars, idx2, "var_pos");

      b().CreateStore(arg_val, pos);
    }
  }

  void BlockBuilder::import_args() {
    const native_int T = machine_code_->total_args;
    const native_int M = machine_code_->required_args;
    const native_int P = machine_code_->post_args;
    const native_int H = M - P;
    const native_int RI = machine_code_->splat_position;
    const bool RP = (RI >= 0);

    Value* arg_ary = b().CreateLoad(
        b().CreateConstGEP2_32(info_.args(), 0,
          offset::Arguments::arguments, "arg_ary_pos"),
        "arg_ary");

    Value* N = b().CreateLoad(
        b().CreateConstGEP2_32(
          info_.args(), 0, offset::Arguments::total));

    BasicBlock* after_args = info_.new_block("after_args");
    BasicBlock* setup_bounds = info_.new_block("setup_bounds");

    Signature jit_spot(ctx_, ctx_->VoidTy);
    jit_spot << ctx_->Int32Ty;

    if(!RP && M == T) {
      BasicBlock* assign_args = info_.new_block("assign_args");

      b().CreateCondBr(
          b().CreateICmpSLE(cint(T), N),
          assign_args, setup_bounds);

      b().SetInsertPoint(assign_args);

      if(H > 0) assign_fixed_args(arg_ary, 0, H);
      if(P > 0) assign_fixed_args(arg_ary, H, H + P);

      b().CreateBr(after_args);
    } else {
      b().CreateBr(setup_bounds);
    }

    b().SetInsertPoint(setup_bounds);

    Value* null = Constant::getNullValue(obj_type);

    // K = (KP && N > M) ? 1 : 0;
    Value* K = b().CreateSelect(
        b().CreateAnd(
          b().CreateICmpNE(b().CreateLoad(keyword_object_), null),
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
    if(H > 0) {
      {
        BasicBlock* head_top = info_.new_block("head_arg_loop_top");
        BasicBlock* head_body = info_.new_block("head_arg_loop_body");
        BasicBlock* head_after = info_.new_block("head_arg_loop_cont");

        // local_index_ = arg_index_ = 0
        b().CreateStore(cint(0), local_index_);
        b().CreateStore(cint(0), arg_index_);

        b().CreateBr(head_top);
        b().SetInsertPoint(head_top);

        Value* local_i = b().CreateLoad(local_index_, "local_index");
        Value* arg_i = b().CreateLoad(arg_index_, "arg_index");

        // l < H && a < N
        Value* loop_test = b().CreateAnd(
            b().CreateICmpSLT(local_i, cint(H)),
            b().CreateICmpSLT(arg_i, N));

        b().CreateCondBr(loop_test, head_body, head_after);

        b().SetInsertPoint(head_body);

        Value* idx[] = {
          cint(0),
          cint(offset::StackVariables::locals),
          local_i
        };

        // locals[local_index_] = args[local_index_]
        b().CreateStore(
            b().CreateLoad(
              b().CreateGEP(arg_ary, arg_i)),
            b().CreateGEP(vars, idx));

        // local_index_++
        b().CreateStore(b().CreateAdd(local_i, cint(1)), local_index_);

        // arg_index++
        b().CreateStore(b().CreateAdd(arg_i, cint(1)), arg_index_);

        b().CreateBr(head_top);

        b().SetInsertPoint(head_after);
      }

      {
        // if(N < H)
        BasicBlock* head_missing_top = info_.new_block("head_missing_arg_loop_top");
        BasicBlock* head_missing_body = info_.new_block("head_missing_arg_loop_body");
        BasicBlock* head_missing_after = info_.new_block("head_missing_arg_loop_cont");

        b().CreateCondBr(
            b().CreateICmpSLT(N, cint(H)),
            head_missing_top, head_missing_after);

        b().SetInsertPoint(head_missing_top);

        Value* local_i = b().CreateLoad(local_index_, "local_index");

        // l < H
        b().CreateCondBr(
            b().CreateICmpSLT(local_i, cint(H)),
            head_missing_body, head_missing_after);

        b().SetInsertPoint(head_missing_body);

        Value* idx[] = {
          cint(0),
          cint(offset::StackVariables::locals),
          local_i
        };

        // locals[local_index_] = cNil
        b().CreateStore(
            constant(cNil, obj_type),
            b().CreateGEP(vars, idx));

        // local_index_++
        b().CreateStore(b().CreateAdd(local_i, cint(1)), local_index_);

        b().CreateBr(head_missing_top);

        b().SetInsertPoint(head_missing_after);
      }
    }

    // arg limit = H + ON
    Value* H_ON = b().CreateAdd(cint(H), ON);

    BasicBlock* pre_opts = info_.new_block("pre_optional_args");
    BasicBlock* post_opts = info_.new_block("post_optional_args");

    b().CreateCondBr(
        b().CreateICmpSGT(O, cint(0)),
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

      // local_index_ = arg_index_ = H
      b().CreateStore(cint(H), local_index_);
      b().CreateStore(cint(H), arg_index_);

      b().CreateBr(top);
      b().SetInsertPoint(top);

      Value* local_i = b().CreateLoad(local_index_, "local_index");
      Value* arg_i = b().CreateLoad(arg_index_, "arg_index");

      // l < H + O && a < MIN(N, H + ON)
      Value* loop_test = b().CreateAnd(
          b().CreateICmpSLT(local_i, H_O),
          b().CreateICmpSLT(arg_i,
            b().CreateSelect(
              b().CreateICmpSLT(N, H_ON), N, H_ON)));

      b().CreateCondBr(loop_test, body, after);

      b().SetInsertPoint(body);

      Value* idx[] = {
        cint(0),
        cint(offset::StackVariables::locals),
        local_i
      };

      // locals[local_index_] = args[local_index_]
      b().CreateStore(
          b().CreateLoad(
            b().CreateGEP(arg_ary, arg_i)),
          b().CreateGEP(vars, idx));

      // local_index_++
      b().CreateStore(b().CreateAdd(local_i, cint(1)), local_index_);

      // arg_index++
      b().CreateStore(b().CreateAdd(arg_i, cint(1)), arg_index_);

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

      b().CreateStore(H_ON, local_index_);

      Type* undef_type = llvm::PointerType::getUnqual(obj_type);
      Object** addr = ctx_->llvm_state()->shared().globals.undefined.object_address();
      Value* undef = b().CreateLoad(constant(addr, undef_type));

      b().CreateBr(top);
      b().SetInsertPoint(top);

      Value* local_i = b().CreateLoad(local_index_, "local_index");

      b().CreateCondBr(
          b().CreateICmpSLT(local_i, H_O),
          body, after);

      b().SetInsertPoint(body);

      Value* idx[] = {
        cint(0),
        cint(offset::StackVariables::locals),
        local_i
      };

      // locals[local_index_] = undefined
      b().CreateStore(
          undef,
          b().CreateGEP(vars, idx));

      // local_index_++
      b().CreateStore(b().CreateAdd(local_i, cint(1)), local_index_);

      b().CreateBr(top);

      b().SetInsertPoint(after);
      b().CreateBr(post_opts);
    }

    b().SetInsertPoint(post_opts);

    BasicBlock* post_args = info_.new_block("post_args");

    // rest arguments
    if(RP) {
      // RN = (RP && (X = E - ON) > 0) ? X : 0;
        X = b().CreateSub(E, ON);
        Value* RN = b().CreateSelect(
            b().CreateICmpSGT(X, cint(0)), X, cint(0));

      Signature sig(ctx_, "Object");
      sig << "State";
      sig << "Arguments";
      sig << ctx_->Int32Ty;
      sig << ctx_->Int32Ty;

      Value* call_args[] = {
        info_.state(),
        info_.args(),
        H_ON,
        RN
      };

      Function* func = sig.function("rbx_construct_splat");
      func->setOnlyReadsMemory();
      func->setDoesNotThrow();

      CallInst* splat_val = sig.call("rbx_construct_splat",
          call_args, 4, "splat_val", b());

      splat_val->setOnlyReadsMemory();
      splat_val->setDoesNotThrow();

      Value* idx[] = {
        cint(0),
        cint(offset::StackVariables::locals),
        cint(RI)
      };

      Value* pos = b().CreateGEP(vars, idx, "splat_pos");
      b().CreateStore(splat_val, pos);
    }

    b().CreateBr(post_args);
    b().SetInsertPoint(post_args);

    BasicBlock* kw_args = info_.new_block("kw_args");

    if(P > 0) {
      // post arguments
      // PI = H + O + (RP ? 1 : 0)
      Value* PI = b().CreateAdd(cint(H), RP ? b().CreateAdd(O, cint(1)) : O);

      // l = PI; l < PI + P && a < N - K
      Value* PI_P = b().CreateAdd(PI, cint(P));
      X = b().CreateSub(N, K);
      Value* N_K = b().CreateSelect(
          b().CreateAnd(
            b().CreateICmpSGT(X, cint(0)),
            b().CreateICmpSLT(X, N)),
          X, N);

      // local_index_ = PI
      b().CreateStore(PI, local_index_);

      // arg_index_ = N - P - K
      X = b().CreateSub(
            b().CreateSub(N, cint(P)), K);
      b().CreateStore(
          b().CreateSelect(
            b().CreateAnd(
              b().CreateICmpSGT(X, cint(0)),
              b().CreateICmpSLT(X, N)),
            X, N),
          arg_index_);

      {
        BasicBlock* top = info_.new_block("post_arg_loop_top");
        BasicBlock* body = info_.new_block("post_arg_loop_body");
        BasicBlock* after = info_.new_block("post_arg_loop_cont");

        b().CreateBr(top);
        b().SetInsertPoint(top);

        Value* local_i = b().CreateLoad(local_index_, "local_index");
        Value* arg_i = b().CreateLoad(arg_index_, "arg_index");

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

        // locals[local_index_] = args[local_index_]
        b().CreateStore(
            b().CreateLoad(
              b().CreateGEP(arg_ary, arg_i)),
            b().CreateGEP(vars, idx));

        // local_index_++
        b().CreateStore(b().CreateAdd(local_i, cint(1)), local_index_);

        // arg_index++
        b().CreateStore(b().CreateAdd(arg_i, cint(1)), arg_index_);

        b().CreateBr(top);

        b().SetInsertPoint(after);
      }

      {
        // if(l < PI + P)
        BasicBlock* post_missing_top = info_.new_block("post_missing_arg_loop_top");
        BasicBlock* post_missing_body = info_.new_block("post_missing_arg_loop_body");
        BasicBlock* post_missing_after = info_.new_block("post_missing_arg_loop_cont");

        b().CreateCondBr(
            b().CreateICmpSLT(
              b().CreateLoad(local_index_),
              PI_P),
            post_missing_top, post_missing_after);

        b().SetInsertPoint(post_missing_top);

        Value* local_i = b().CreateLoad(local_index_, "local_index");

        // l < PI + P
        b().CreateCondBr(
            b().CreateICmpSLT(local_i, PI_P),
            post_missing_body, post_missing_after);

        b().SetInsertPoint(post_missing_body);

        Value* idx[] = {
          cint(0),
          cint(offset::StackVariables::locals),
          local_i
        };

        // locals[local_index_] = cNil
        b().CreateStore(
            constant(cNil, obj_type),
            b().CreateGEP(vars, idx));

        // local_index_++
        b().CreateStore(b().CreateAdd(local_i, cint(1)), local_index_);

        b().CreateBr(post_missing_top);

        b().SetInsertPoint(post_missing_after);
      }
    }

    b().CreateBr(kw_args);
    b().SetInsertPoint(kw_args);

    // keywords
    if(machine_code_->keywords) {
      BasicBlock* before_kw = info_.new_block("before_keywords");
      BasicBlock* after_kw = info_.new_block("after_keywords");

      Value* null = Constant::getNullValue(obj_type);
      Value* kw_val = b().CreateLoad(keyword_object_, "kw_val");

      b().CreateCondBr(
          b().CreateICmpNE(kw_val, null),
          before_kw, after_kw);

      b().SetInsertPoint(before_kw);

      Value* idx[] = {
        cint(0),
        cint(offset::StackVariables::locals),
        RP ? cint(T) : cint(T - 1)
      };

      b().CreateStore(
          b().CreateLoad(keyword_object_),
          b().CreateGEP(vars, idx));

      b().CreateBr(after_kw);
      b().SetInsertPoint(after_kw);
    }

    b().CreateBr(after_args);
    b().SetInsertPoint(after_args);
  }
}
}

#endif
