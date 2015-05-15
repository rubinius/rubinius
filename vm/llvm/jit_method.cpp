#ifdef ENABLE_LLVM

#include "llvm/jit_method.hpp"
#include "llvm/jit_context.hpp"

#include "llvm/method_info.hpp"

#include "call_frame.hpp"
#include "machine_code.hpp"
#include "instruments/tooling.hpp"

#include "builtin/constant_scope.hpp"
#include "builtin/module.hpp"
#include "builtin/class.hpp"

using namespace llvm;

namespace rubinius {

namespace jit {

  void MethodBuilder::setup() {
    std::vector<Type*> ftypes;
    ftypes.push_back(ctx_->ptr_type("State"));
    ftypes.push_back(ctx_->ptr_type("CallFrame"));
    ftypes.push_back(ctx_->ptr_type("Executable"));
    ftypes.push_back(ctx_->ptr_type("Module"));
    ftypes.push_back(ctx_->ptr_type("Arguments"));

    FunctionType* ft = FunctionType::get(ctx_->ptr_type("Object"), ftypes, false);

    std::ostringstream ss;
    ss << std::string("_X_")
       << ctx_->llvm_state()->enclosure_name(info_.method())
       << "#"
       << ctx_->llvm_state()->symbol_debug_str(info_.method()->name())
       << "k@" << ++ctx_->llvm_state()->vm()->metrics().m.jit_metrics.methods_compiled;

    llvm::Function* func = Function::Create(ft, GlobalValue::ExternalLinkage,
                            ss.str(), ctx_->module());

    Function::arg_iterator ai = func->arg_begin();
    llvm::Value* state = ai++; state->setName("state");
    llvm::Value* prev = ai++; prev->setName("previous");
    exec = ai++; exec->setName("exec");
    module = ai++; module->setName("mod");
    llvm::Value* args = ai++; args->setName("args");

    BasicBlock* block = BasicBlock::Create(ctx_->llvm_context(), "entry", func);
    b().SetInsertPoint(block);

    ctx_->set_function(func);

    info_.set_state(state);
    info_.set_args(args);
    info_.set_previous(prev);
    info_.set_entry(block);

    alloc_frame("method_body");

    check_arity();

    initialize_frame(machine_code_->stack_size);

    nil_stack(machine_code_->stack_size, constant(cNil, obj_type));

    setup_scope();

    import_args();

    import_args_ = b().GetInsertBlock();

    b().CreateBr(body_);
    b().SetInsertPoint(body_);
  }

  void MethodBuilder::check_arity() {
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

    BasicBlock* n_gt_t = info_.new_block("not_rp_and_n_gt_t");

    // N < M
    Value* cmp = b().CreateICmpSLT(N, cint(M), "arg_cmp");
    b().CreateCondBr(cmp, arg_error, n_gt_t);

    b().SetInsertPoint(n_gt_t);

    BasicBlock* kw_check = info_.new_block("kw_check");

    // !RP && N > T
    if(!RP) {
      Value* cmp = b().CreateICmpSGT(N, cint(T), "arg_cmp");
      b().CreateCondBr(cmp, arg_error, kw_check);
    } else {
      b().CreateBr(kw_check);
    }

    b().SetInsertPoint(kw_check);

    if(machine_code_->keywords) {
      BasicBlock* kw_check = info_.new_block("kw_check_object");

      b().CreateCondBr(
          b().CreateICmpSGT(N, cint(M)),
          kw_check, cont);

      b().SetInsertPoint(kw_check);

      Value* arg_ary = b().CreateLoad(
          b().CreateConstGEP2_32(info_.args(), 0,
            offset::Arguments::arguments, "arg_ary_pos"),
          "arg_ary");

      Value* kw_arg = b().CreateLoad(
            b().CreateGEP(arg_ary,
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
    return_value(val);

    // Switch to using continuation
    b().SetInsertPoint(cont);
  }

  void MethodBuilder::assign_fixed_args(Value* arg_ary, int start, int end) {
    for(int i = start; i < end; i++) {
      Value* int_pos = cint(i);

      Value* arg_val_offset = b().CreateConstGEP1_32(arg_ary, i, "arg_val_offset");

      Value* arg_val = b().CreateLoad(arg_val_offset, "arg_val");

      Value* idx[] = {
        cint(0),
        cint(offset::StackVariables::locals),
        int_pos
      };

      Value* pos = b().CreateGEP(vars, idx, "var_pos");

      b().CreateStore(arg_val, pos);
    }
  }

  void MethodBuilder::import_args() {
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

    BasicBlock* after_args = info_.new_block("after_args");
    BasicBlock* setup_bounds = info_.new_block("setup_bounds");

    if(!RP && M == T) {
      if(H > 0) assign_fixed_args(arg_ary, 0, H);
      if(P > 0) assign_fixed_args(arg_ary, H, H + P);

      b().CreateBr(after_args);
    } else {
      b().CreateBr(setup_bounds);
    }

    b().SetInsertPoint(setup_bounds);

    Value* null = Constant::getNullValue(obj_type);

    // K = (keyword_object && N > M) ? 1 : 0;
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
      assign_fixed_args(arg_ary, 0, H);
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

      // local_index_ < H + O && arg_index_ < H + ON
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
      Value* N_K = b().CreateSub(N, K);

      // local_index_ = PI
      b().CreateStore(PI, local_index_);

      // arg_index_ = N - P - K
      b().CreateStore(
          b().CreateSub(
            b().CreateSub(N, cint(P)), K),
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
    }

    b().CreateBr(kw_args);
    b().SetInsertPoint(kw_args);

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

    b().CreateBr(after_args);
    b().SetInsertPoint(after_args);
  }

  void MethodBuilder::return_value(Value* ret, BasicBlock* cont) {
    if(ctx_->llvm_state()->include_profiling()) {
      BasicBlock* end_profiling = info_.new_block("end_profiling");
      if(!cont) {
        cont = info_.new_block("continue");
      }

      ctx_->profiling(b(), end_profiling, cont);

      Signature sig(ctx_, ctx_->VoidTy);
      sig << llvm::PointerType::getUnqual(ctx_->Int8Ty);

      Value* call_args[] = {
        method_entry_
      };

      sig.call("rbx_end_profiling", call_args, 1, "", b());

      b().CreateBr(cont);

      b().SetInsertPoint(cont);
    }

    info_.add_return_value(ret, b().GetInsertBlock());
    b().CreateBr(info_.return_pad());
  }


  void MethodBuilder::setup_scope() {
    llvm::Value* args = info_.args();

    Value* heap_null = ConstantExpr::getNullValue(llvm::PointerType::getUnqual(vars_type));
    Value* heap_pos = get_field(vars, offset::StackVariables::on_heap);

    b().CreateStore(heap_null, heap_pos);

    Value* self = b().CreateLoad(get_field(args, offset::Arguments::recv),
        "args.recv");

    if(Class* klass = info_.self_class()) {
      if(!klass->nil_p()) {
        type::KnownType kt = type::KnownType::unknown();
        if(kind_of<SingletonClass>(klass)) {
          kt = type::KnownType::singleton_instance(klass->class_id());
        } else {
          kt = type::KnownType::instance(klass->class_id());
        }
        kt.associate(ctx_, self);
      }
    }

    b().CreateStore(self, get_field(vars, offset::StackVariables::self));
    Value* mod = module;
    b().CreateStore(mod, get_field(vars, offset::StackVariables::module));

    Value* blk = b().CreateLoad(get_field(args, offset::Arguments::block),
        "args.block");
    b().CreateStore(blk, get_field(vars, offset::StackVariables::block));

    b().CreateStore(Constant::getNullValue(ctx_->ptr_type("VariableScope")),
        get_field(vars, offset::StackVariables::parent));

    b().CreateStore(constant(cNil, obj_type), get_field(vars, offset::StackVariables::last_match));

    nil_locals();
  }

  void MethodBuilder::initialize_frame(int stack_size) {
    Value* code_gep = get_field(call_frame, offset::CallFrame::compiled_code);
    method = b().CreateBitCast(
        exec, cast<llvm::PointerType>(code_gep->getType())->getElementType(), "compiled_code");

    // previous
    b().CreateStore(info_.previous(), get_field(call_frame, offset::CallFrame::previous));

    // arguments
    b().CreateStore(info_.args(), get_field(call_frame, offset::CallFrame::arguments));

    // msg
    b().CreateStore(
        ConstantInt::getNullValue(ctx_->Int8PtrTy),
        get_field(call_frame, offset::CallFrame::dispatch_data));

    // compiled_code
    b().CreateStore(method, code_gep);

    // constant_scope
    Value* constant_scope = b().CreateLoad(
        b().CreateConstGEP2_32(method, 0, offset::CompiledCode::scope, "constant_scope_pos"),
        "constant_scope");

    Value* constant_scope_gep = get_field(call_frame, offset::CallFrame::constant_scope);
    b().CreateStore(constant_scope, constant_scope_gep);

    // flags
    int flags = CallFrame::cJITed;
    if(!use_full_scope_) flags |= CallFrame::cClosedScope;

    b().CreateStore(
        cint(flags),
        get_field(call_frame, offset::CallFrame::flags));

    // ip
    b().CreateStore(
        cint(0),
        get_field(call_frame, offset::CallFrame::ip));

    // scope
    b().CreateStore(vars, get_field(call_frame, offset::CallFrame::scope));

    // jit_data
    b().CreateStore(
        constant(ctx_->runtime_data_holder(), ctx_->Int8PtrTy),
        get_field(call_frame, offset::CallFrame::jit_data));

    if(ctx_->llvm_state()->include_profiling()) {
      BasicBlock* setup_profiling = info_.new_block("setup_profiling");
      BasicBlock* cont = info_.new_block("continue");

      ctx_->profiling(b(), setup_profiling, cont);

      Signature sig(ctx_, ctx_->VoidTy);
      sig << "State";
      sig << llvm::PointerType::getUnqual(ctx_->Int8Ty);
      sig << "Executable";
      sig << "Module";
      sig << "Arguments";
      sig << "CompiledCode";

      Value* call_args[] = {
        info_.state(),
        method_entry_,
        exec,
        module,
        info_.args(),
        method
      };

      sig.call("rbx_begin_profiling", call_args, 6, "", b());

      b().CreateBr(cont);

      b().SetInsertPoint(cont);
    }
  }
}
}

#endif
