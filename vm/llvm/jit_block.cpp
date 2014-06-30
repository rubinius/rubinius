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
       << "$block@" << ctx_->llvm_state()->add_jitted_method();

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

    // import_args_19_style();

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
    Value* total_offset = b().CreateConstGEP2_32(info_.args(), 0,
        offset::Arguments::total, "total_pos");
    Value* N = b().CreateLoad(total_offset, "arg.total");

    // For others to use.
    arg_total = N;

    BasicBlock* check_arity = info_.new_block("check_arity");
    BasicBlock* arg_error = info_.new_block("arg_error");
    BasicBlock* cont = info_.new_block("import_args");

    const native_int T = machine_code_->total_args;
    const native_int M = machine_code_->required_args;
    const native_int RI = machine_code_->splat_position;
    const bool RP = (RI >= 0);

    if(T == 0) {
      if(!RP) {
        Value* cmp = b().CreateICmpNE(N, cint(0), "arg_cmp");
        b().CreateCondBr(cmp, arg_error, cont);
      } else {
        b().CreateBr(cont);
      }
    } else {
      b().CreateBr(check_arity);
    }

    b().SetInsertPoint(check_arity);

    BasicBlock* n_lt_m = info_.new_block("n_lt_m");

    Value* lambda_flag = cint(CallFrame::cIsLambda);
    Value* flags = b().CreateLoad(
        get_field(block_inv, offset::BlockInvocation::flags),
        "invocation.flags");

    if(M > 1 || (M == 1 && (RP || RI < -2))) {
      Value* not_lambda = b().CreateICmpNE(
          b().CreateAnd(flags, lambda_flag), lambda_flag);

      BasicBlock* destruct = info_.new_block("destructure");
      BasicBlock* destruct_cont = info_.new_block("destructure_contuation");

      b().CreateCondBr(not_lambda, destruct, n_lt_m);

      b().SetInsertPoint(destruct);

      Signature sig(ctx_, "Object");
      sig << "State";
      sig << "CallFrame";
      sig << "Arguments";

      Value* call_args[] = {
        info_.state(),
        info_.call_frame(),
        info_.args()
      };

      Value* val = sig.call("rbx_destructure_args", call_args, 3, "", b());

      Value* null = Constant::getNullValue(val->getType());
      Value* is_null = b().CreateICmpEQ(val, null);

      info_.add_return_value(null, b().GetInsertBlock());
      b().CreateCondBr(is_null, info_.return_pad(), destruct_cont);

      b().SetInsertPoint(destruct_cont);

      N = b().CreateLoad(total_offset, "arg.total");
      arg_total = N;
    }

    b().CreateBr(n_lt_m);
    b().SetInsertPoint(n_lt_m);

    Value* lambda = b().CreateICmpEQ(
        b().CreateAnd(flags, lambda_flag), lambda_flag);

    BasicBlock* n_gt_t = info_.new_block("not_rp_and_n_gt_t");

    // N < M
    Value* cmp = b().CreateAnd(lambda, b().CreateICmpSLT(N, cint(M), "arg_cmp"));
    b().CreateCondBr(cmp, arg_error, n_gt_t);

    b().SetInsertPoint(n_gt_t);

    // !RP && N > T
    if(!RP) {
      BasicBlock* update_n = info_.new_block("update_n");

      Value* cmp = b().CreateAnd(lambda, b().CreateICmpSGT(N, cint(T), "arg_cmp"));
      b().CreateCondBr(cmp, arg_error, update_n);

      b().SetInsertPoint(update_n);

      N = cint(T);
      arg_total = N;
    }

    if(machine_code_->keywords) {
      BasicBlock* kw_check = info_.new_block("kw_check_object");

      b().CreateCondBr(
          b().CreateICmpSGT(N, cint(M)),
          kw_check, cont);

      b().SetInsertPoint(kw_check);

      Signature sig(ctx_, "Object");
      sig << "State";
      sig << "CallFrame";
      sig << "Arguments";

      Value* call_args[] = {
        info_.state(),
        info_.previous(),
        info_.args()
      };

      Value* keyword_val = sig.call("rbx_check_keyword",
          call_args, 3, "keyword_val", b());

      b().CreateStore(keyword_val, keyword_object_);
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

    Value* val = sig.call("rbx_arg_error", call_args, 4, "ret", b());
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
    Value* N = arg_total;

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

    if(!RP && M == T) {
      if(H > 0) assign_fixed_args(arg_ary, 0, H);
      if(P > 0) assign_fixed_args(arg_ary, H, H + P);

      return;
    }

    // K = (KP && N > M) ? 1 : 0;
    Value* null = Constant::getNullValue(obj_type);

    Value* K = b().CreateSelect(
        b().CreateAnd(
          b().CreateICmpNE(b().CreateLoad(keyword_object_), null),
          b().CreateICmpSGT(N, cint(M))),
        cint(1), cint(0));

    // const native_int O = T - M - (mcode->keywords ? 1 : 0);
    Value* O = cint(T - M - (machine_code_->keywords ? 1 : 0));

    // const native_int E = N - M - K;
    Value* E = b().CreateSub(N, b().CreateAdd(cint(M), K));

    // const native_int ON = (X = MIN(O, E)) > 0 ? X : 0;
    Value* X = b().CreateSelect(b().CreateICmpSLE(O, E), O, E);
    Value* ON = b().CreateSelect(b().CreateICmpSGT(X, cint(0)), X, cint(0));

    // head arguments
    assign_fixed_args(arg_ary, 0, H);

    // optional arguments
    // for(l = a = H; l < H + O && a < H + ON; l++, a++)

    // local limit = H + O
    Value* H_O = b().CreateAdd(cint(H), O);

    // arg limit = H + ON
    Value* H_ON = b().CreateAdd(cint(H), ON);

    // local_index_ = H
    b().CreateStore(cint(H), local_index_);

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

      // l < H + O && l < H + ON
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
          pre, after);

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
    }

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

      Function* func = sig.function("rbx_construct_splat_x");
      func->setOnlyReadsMemory();
      func->setDoesNotThrow();

      CallInst* splat_val = sig.call("rbx_construct_splat_x",
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

    // post arguments
    // PI = H + O + (RP ? 1 : 0)
    Value* PI = b().CreateAdd(cint(H), RP ? b().CreateAdd(O, cint(1)) : O);

    // l = PI; l < PI + P && a < N - K
    Value* PI_P = b().CreateAdd(PI, cint(P));
    Value* N_K = b().CreateSub(N, K);

    // local_index_ = PI
    b().CreateStore(PI, local_index_);

    // arg_index_ = N - P - K
    b().CreateStore(
        b().CreateSub(N,
          b().CreateSub(cint(P), K)),
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
          b().CreateICmpSLT(local_index_, PI_P),
          b().CreateICmpSLT(arg_index_, N_K));

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

    // keywords
    {
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
  }

  void BlockBuilder::import_args_19_style() {
    if(machine_code_->required_args > 1) {
      Value* lambda_check =
        b().CreateICmpEQ(
          b().CreateAnd(
            inv_flags_,
            ConstantInt::get(inv_flags_->getType(), CallFrame::cIsLambda)),
          ConstantInt::get(inv_flags_->getType(), 0));

      BasicBlock* destruct = BasicBlock::Create(ctx_->llvm_context(), "destructure", info_.function());
      BasicBlock* cont = BasicBlock::Create(ctx_->llvm_context(), "arg_loop_body", info_.function());

      b().CreateCondBr(lambda_check, destruct, cont);

      b().SetInsertPoint(destruct);

      Signature sig(ctx_, "Object");
      sig << "State";
      sig << "CallFrame";
      sig << "Arguments";

      Value* call_args[] = { info_.state(), info_.call_frame(), info_.args() };

      Value* val = sig.call("rbx_destructure_args", call_args, 3, "", b());

      Value* null = Constant::getNullValue(val->getType());
      Value* is_null = b().CreateICmpEQ(val, null);

      info_.add_return_value(null, b().GetInsertBlock());
      b().CreateCondBr(is_null, info_.return_pad(), cont);

      b().SetInsertPoint(cont);
    }

    Value* local_i = counter2_;

    Value* loop_i = info_.counter();
    Value* arg_ary = b().CreateLoad(
                       b().CreateConstGEP2_32(info_.args(), 0, offset::Arguments::arguments),
                       "arg_ary");

    // The variables used in the 4 phases.
    int P = machine_code_->post_args;
    Value* Pv = cint(P);

    int R = machine_code_->required_args;

    int M = R - P;
    Value* Mv = cint(M);

    Value* T = b().CreateLoad(
                 b().CreateConstGEP2_32(info_.args(), 0, offset::Arguments::total),
                 "args.total");

    int DT = machine_code_->total_args;
    Value* DTv = cint(DT);

    int O = DT - R;
    Value* Ov = cint(O);

    int HS = machine_code_->splat_position > 0 ? 1 : 0;

    Value* CT = HS ? T
                   : b().CreateSelect(b().CreateICmpSLT(T, DTv), T, DTv);

    Value* Z = b().CreateSub(CT, cint(M));
    Value* U = b().CreateSub(Z, cint(P));



    // Phase 1, the mandatories
    // 0 ... min(M,T)

    {
      BasicBlock* top = info_.new_block("mand_loop_top");
      BasicBlock* body = info_.new_block("mand_loop_body");
      BasicBlock* after = info_.new_block("mand_loop_cont");

      // limit = min(M,T)
      Value* limit = b().CreateSelect(b().CreateICmpSLT(Mv, T), Mv, T);

      // *loop_i = T-P
      b().CreateStore(cint(0), loop_i);

      b().CreateBr(top);

      b().SetInsertPoint(top);

      // loop_val = *loop_i
      Value* loop_val = b().CreateLoad(loop_i, "loop_val");

      // if(loop_val < T) { goto body } else { goto after }
      b().CreateCondBr(
          b().CreateICmpSLT(loop_val, limit, "loop_test"),
          body, after);

      // Now, the body
      b().SetInsertPoint(body);

      Value* idx2[] = {
        cint(0),
        cint(offset::StackVariables::locals),
        loop_val
      };

      // locals[loop_val] = args[loop_val]
      b().CreateStore(
          b().CreateLoad(
            b().CreateGEP(arg_ary, loop_val)),
          b().CreateGEP(vars, idx2));

      // *loop_i = loop_val + 1
      b().CreateStore(
          b().CreateAdd(loop_val, cint(1)),
          loop_i);

      b().CreateBr(top);

      b().SetInsertPoint(after);
    }

    // Phase 2, the post args
    // CT - min(Z,P) ... CT
    {
      BasicBlock* top = info_.new_block("post_loop_top");
      BasicBlock* body = info_.new_block("post_loop_body");
      BasicBlock* after = info_.new_block("post_loop_cont");

      // *loop_i = CT - min(Z,P)
      b().CreateStore(
        b().CreateSub(
          CT,
          b().CreateSelect(b().CreateICmpSLT(Z, Pv), Z, Pv)),
        loop_i);

      // *local_i = M+O+HS
      b().CreateStore(
        cint(M+O+HS),
        local_i);

      b().CreateBr(top);

      b().SetInsertPoint(top);

      // loop_val = *loop_i
      Value* loop_val = b().CreateLoad(loop_i, "loop_val");

      // if(loop_val < T) { goto body } else { goto after }
      b().CreateCondBr(
          b().CreateICmpSLT(loop_val, CT, "loop_test"),
          body, after);

      // Now, the body
      b().SetInsertPoint(body);

      // local_val = *local_i
      Value* local_val = b().CreateLoad(local_i, "local_val");

      Value* idx2[] = {
        cint(0),
        cint(offset::StackVariables::locals),
        local_val
      };

      // locals[local_idx] = args[loop_val]
      b().CreateStore(
          b().CreateLoad(
            b().CreateGEP(arg_ary, loop_val)),
          b().CreateGEP(vars, idx2));

      // *loop_i = loop_val + 1
      b().CreateStore(
          b().CreateAdd(loop_val, cint(1)),
          loop_i);

      // *local_i = local_val + 1;
      b().CreateStore(
          b().CreateAdd(local_val, cint(1)),
          local_i);

      b().CreateBr(top);

      b().SetInsertPoint(after);
    }

    // Phase 3 - optionals
    // M ... M + min(O, U)

    {
      BasicBlock* top = info_.new_block("opt_arg_loop_top");
      BasicBlock* body = info_.new_block("opt_arg_loop_body");
      BasicBlock* after = info_.new_block("opt_arg_loop_cont");

      Value* limit =
        b().CreateAdd(
            Mv,
            b().CreateSelect(b().CreateICmpSLT(Ov, U), Ov, U));

      // *loop_i = M
      b().CreateStore(Mv, loop_i);
      b().CreateBr(top);

      b().SetInsertPoint(top);

      // loop_val = *loop_i;
      Value* loop_val = b().CreateLoad(loop_i, "loop_val");

      // if(loop_val < limit) { goto body; } else { goto after; }
      b().CreateCondBr(
          b().CreateICmpSLT(loop_val, limit, "loop_test"),
          body, after);

      // Now, the body
      b().SetInsertPoint(body);

      Value* idx2[] = {
        cint(0),
        cint(offset::StackVariables::locals),
        loop_val
      };

      // locals[loop_val] = args[loop_val]
      b().CreateStore(
          b().CreateLoad(
            b().CreateGEP(arg_ary, loop_val)),
          b().CreateGEP(vars, idx2));

      // *loop_i = loop_val + 1
      b().CreateStore(
          b().CreateAdd(loop_val, cint(1)),
          loop_i);

      b().CreateBr(top);

      b().SetInsertPoint(after);
    }

    // Phase 4 - splat
    if(machine_code_->splat_position >= 0) {
      Signature sig(ctx_, "Object");
      sig << "State";
      sig << "Arguments";
      sig << ctx_->Int32Ty;
      sig << ctx_->Int32Ty;

      Value* call_args[] = {
        info_.state(),
        info_.args(),
        cint(M + O),
        cint(DT)
      };

      Function* func = sig.function("rbx_construct_splat");
      func->setOnlyReadsMemory();
      func->setDoesNotThrow();

      CallInst* splat_val = sig.call("rbx_construct_splat", call_args, 4, "splat_val", b());

      splat_val->setOnlyReadsMemory();
      splat_val->setDoesNotThrow();

      Value* idx3[] = {
        cint(0),
        cint(offset::StackVariables::locals),
        cint(machine_code_->splat_position)
      };

      Value* pos = b().CreateGEP(vars, idx3, "splat_pos");
      b().CreateStore(splat_val, pos);
    }
  }

}
}

#endif
