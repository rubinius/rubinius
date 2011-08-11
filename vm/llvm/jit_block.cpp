#ifdef ENABLE_LLVM

#include "llvm/jit_block.hpp"
#include "llvm/jit_context.hpp"

#include "call_frame.hpp"
#include "stack_variables.hpp"
#include "builtin/staticscope.hpp"
#include "builtin/module.hpp"

#include "instruments/tooling.hpp"

using namespace llvm;

namespace rubinius {
namespace jit {

  void BlockBuilder::setup() {
    std::vector<const Type*> ftypes;
    ftypes.push_back(ls_->ptr_type("VM"));
    ftypes.push_back(ls_->ptr_type("CallFrame"));
    ftypes.push_back(ls_->ptr_type("BlockEnvironment"));
    ftypes.push_back(ls_->ptr_type("Arguments"));
    ftypes.push_back(ls_->ptr_type("BlockInvocation"));

    FunctionType* ft = FunctionType::get(ls_->ptr_type("Object"), ftypes, false);

    std::stringstream ss;
    ss << std::string("_X_")
       << ls_->enclosure_name(info_.method())
       << "#"
       << ls_->symbol_cstr(info_.method()->name())
       << "$block@" << ls_->add_jitted_method();

    func = Function::Create(ft, GlobalValue::ExternalLinkage,
                            ss.str().c_str(), ls_->module());

    Function::arg_iterator ai = func->arg_begin();
    vm =   ai++; vm->setName("state");
    prev = ai++; prev->setName("previous");
    block_env = ai++; block_env->setName("env");
    args = ai++; args->setName("args");
    block_inv = ai++; block_inv->setName("invocation");

    BasicBlock* block = BasicBlock::Create(ls_->ctx(), "entry", func);
    b().SetInsertPoint(block);

    info_.set_function(func);
    info_.set_vm(vm);
    info_.set_args(args);
    info_.set_previous(prev);
    info_.set_entry(block);

    BasicBlock* body = BasicBlock::Create(ls_->ctx(), "block_body", func);

    pass_one(body);

    info_.set_counter(b().CreateAlloca(ls_->Int32Ty, 0, "counter_alloca"));
    counter2_ = b().CreateAlloca(ls_->Int32Ty, 0, "counter2");

    // The 3 here is because we store {ip, sp, type} per unwind.
    info_.set_unwind_info(b().CreateAlloca(ls_->Int32Ty,
          ConstantInt::get(ls_->Int32Ty, rubinius::kMaxUnwindInfos * 3),
          "unwind_info"));

    valid_flag = b().CreateAlloca(ls_->Int1Ty, 0, "valid_flag");

    Value* cfstk = b().CreateAlloca(obj_type,
        ConstantInt::get(ls_->Int32Ty,
          (sizeof(CallFrame) / sizeof(Object*)) + vmm_->stack_size),
        "cfstk");

    call_frame = b().CreateBitCast(
        cfstk,
        llvm::PointerType::getUnqual(cf_type), "call_frame");

    info_.set_out_args(b().CreateAlloca(ls_->type("Arguments"), 0, "out_args"));

    if(ls_->include_profiling()) {
      method_entry_ = b().CreateAlloca(ls_->Int8Ty,
          ConstantInt::get(ls_->Int32Ty, sizeof(tooling::MethodEntry)),
          "method_entry");

      info_.set_profiling_entry(method_entry_);
    }

    info_.set_call_frame(call_frame);

    stk = b().CreateConstGEP1_32(cfstk, sizeof(CallFrame) / sizeof(Object*), "stack");

    info_.set_stack(stk);

    Value* var_mem = b().CreateAlloca(obj_type,
        ConstantInt::get(ls_->Int32Ty,
          (sizeof(StackVariables) / sizeof(Object*)) + vmm_->number_of_locals),
        "var_mem");

    vars = b().CreateBitCast(
        var_mem,
        llvm::PointerType::getUnqual(stack_vars_type), "vars");

    info_.set_variables(vars);

    initialize_frame(vmm_->stack_size);

    nil_stack(vmm_->stack_size, constant(Qnil, obj_type));

    setup_block_scope();

    if(ls_->config().version >= 19) {
      import_args_19_style();
    }

    if(ls_->include_profiling()) {
      Value* test = b().CreateLoad(ls_->profiling(), "profiling");

      BasicBlock* setup_profiling = BasicBlock::Create(ls_->ctx(), "setup_profiling", func);
      BasicBlock* cont = BasicBlock::Create(ls_->ctx(), "continue", func);

      b().CreateCondBr(test, setup_profiling, cont);

      b().SetInsertPoint(setup_profiling);

      Signature sig(ls_, ls_->VoidTy);
      sig << "VM";
      sig << llvm::PointerType::getUnqual(ls_->Int8Ty);
      sig << "BlockEnvironment";
      sig << "Module";
      sig << "CompiledMethod";

      Value* call_args[] = {
        vm,
        method_entry_,
        block_env,
        module_,
        method
      };

      sig.call("rbx_begin_profiling_block", call_args, 5, "", b());

      b().CreateBr(cont);

      b().SetInsertPoint(cont);
    }
    b().CreateBr(body);

    b().SetInsertPoint(body);
  }

  void BlockBuilder::setup_block_scope() {
    b().CreateStore(ConstantExpr::getNullValue(llvm::PointerType::getUnqual(vars_type)),
        get_field(vars, offset::vars_on_heap));
    Value* self = b().CreateLoad(
        get_field(block_inv, offset::blockinv_self),
        "invocation.self");

    b().CreateStore(self, get_field(vars, offset::vars_self));

    Value* inv_mod = b().CreateLoad(
        get_field(block_inv, offset::blockinv_module),
        "invocation.module");

    Value* creation_mod = b().CreateLoad(
        get_field(block_env, offset::blockenv_module),
        "env.module");

    Value* mod = b().CreateSelect(
        b().CreateICmpNE(inv_mod, ConstantExpr::getNullValue(inv_mod->getType())),
        inv_mod,
        creation_mod);

    module_ = mod;

    b().CreateStore(mod, get_field(vars, offset::vars_module));

    Value* blk = b().CreateLoad(get_field(top_scope, offset::varscope_block),
        "args.block");
    b().CreateStore(blk, get_field(vars, offset::vars_block));


    // We don't use top_scope here because of nested blocks. Parent MUST be
    // the scope the block was created in, not the top scope for depth
    // variables to work.
    Value* be_scope = b().CreateLoad(
        get_field(block_env, offset::blockenv_scope),
        "env.scope");

    b().CreateStore(be_scope, get_field(vars, offset::vars_parent));
    b().CreateStore(constant(Qnil, obj_type), get_field(vars, offset::vars_last_match));

    nil_locals();
  }

  void BlockBuilder::initialize_frame(int stack_size) {
    Value* cm_gep = get_field(call_frame, offset::CallFrame::cm);

    method = b().CreateLoad(get_field(block_env, offset::blockenv_code),
                            "env.code");

    // previous
    b().CreateStore(prev, get_field(call_frame, offset::CallFrame::previous));

    // static_scope
    Value* ss = b().CreateLoad(get_field(block_inv, offset::blockinv_static_scope),
                               "invocation.static_scope");

    b().CreateStore(ss, get_field(call_frame, offset::CallFrame::static_scope));

    // arguments
    b().CreateStore(args, get_field(call_frame, offset::CallFrame::arguments));

    // msg
    b().CreateStore(Constant::getNullValue(ls_->Int8PtrTy),
        get_field(call_frame, offset::CallFrame::dispatch_data));

    // cm
    b().CreateStore(method, cm_gep);

    // flags
    inv_flags_ = b().CreateLoad(get_field(block_inv, offset::blockinv_flags),
        "invocation.flags");

    int block_flags = CallFrame::cCustomStaticScope |
      CallFrame::cMultipleScopes |
      CallFrame::cBlock |
      CallFrame::cJITed;

    if(!use_full_scope_) block_flags |= CallFrame::cClosedScope;

    Value* flags = b().CreateOr(inv_flags_,
        ConstantInt::get(ls_->Int32Ty, block_flags), "flags");

    b().CreateStore(flags, get_field(call_frame, offset::CallFrame::flags));

    // ip
    b().CreateStore(ConstantInt::get(ls_->Int32Ty, 0),
        get_field(call_frame, offset::CallFrame::ip));

    // scope
    b().CreateStore(vars, get_field(call_frame, offset::CallFrame::scope));

    // top_scope
    top_scope = b().CreateLoad(
        get_field(block_env, offset::blockenv_top_scope),
        "env.top_scope");

    b().CreateStore(top_scope, get_field(call_frame, offset::CallFrame::top_scope));

    // jit_data
    b().CreateStore(
        constant(info_.context().runtime_data_holder(), ls_->Int8PtrTy),
        get_field(call_frame, offset::CallFrame::jit_data));

  }

  void BlockBuilder::import_args_19_style() {
    Value* vm_obj = vm;
    Value* arg_obj = args;

    if(vmm_->required_args > 1) {
      Value* lambda_check =
        b().CreateICmpEQ(
          b().CreateAnd(
            inv_flags_,
            ConstantInt::get(inv_flags_->getType(), CallFrame::cIsLambda)),
          ConstantInt::get(inv_flags_->getType(), 0));

      BasicBlock* destruct = BasicBlock::Create(ls_->ctx(), "destructure", func);
      BasicBlock* cont = BasicBlock::Create(ls_->ctx(), "arg_loop_body", func);

      b().CreateCondBr(lambda_check, destruct, cont);

      b().SetInsertPoint(destruct);

      Signature sig(ls_, "Object");
      sig << "VM";
      sig << "CallFrame";
      sig << "Arguments";

      Value* call_args[] = { vm, call_frame, args };

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
                       b().CreateConstGEP2_32(args, 0, offset::args_ary),
                       "arg_ary");

    // The variables used in the 4 phases.
    int P = vmm_->post_args;
    Value* Pv = ConstantInt::get(ls_->Int32Ty, P);

    int R = vmm_->required_args;

    int M = R - P;
    Value* Mv = ConstantInt::get(ls_->Int32Ty, M);

    Value* T = b().CreateLoad(
                 b().CreateConstGEP2_32(arg_obj, 0, offset::args_total),
                 "args.total");

    int DT = vmm_->total_args;
    Value* DTv = ConstantInt::get(ls_->Int32Ty, DT);

    int O = DT - R;
    Value* Ov = ConstantInt::get(ls_->Int32Ty, O);

    int HS = vmm_->splat_position > 0 ? 1 : 0;

    Value* CT = HS ? T
                   : b().CreateSelect(b().CreateICmpSLT(T, DTv), T, DTv);

    Value* Z = b().CreateSub(CT, ConstantInt::get(ls_->Int32Ty, M));
    Value* U = b().CreateSub(Z, ConstantInt::get(ls_->Int32Ty, P));



    // Phase 1, the manditories
    // 0 ... min(M,T)

    {
      BasicBlock* top = BasicBlock::Create(ls_->ctx(), "mand_loop_top", func);
      BasicBlock* body = BasicBlock::Create(ls_->ctx(), "mand_loop_body", func);
      BasicBlock* after = BasicBlock::Create(ls_->ctx(), "mand_loop_cont", func);

      // limit = min(M,T)
      Value* limit = b().CreateSelect(b().CreateICmpSLT(Mv, T), Mv, T);

      // *loop_i = T-P
      b().CreateStore(ConstantInt::get(ls_->Int32Ty, 0), loop_i);

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
        ConstantInt::get(ls_->Int32Ty, 0),
        ConstantInt::get(ls_->Int32Ty, offset::vars_tuple),
        loop_val
      };

      // locals[loop_val] = args[loop_val]
      b().CreateStore(
          b().CreateLoad(
            b().CreateGEP(arg_ary, loop_val)),
          b().CreateGEP(vars, idx2, idx2+3));

      // *loop_i = loop_val + 1
      b().CreateStore(
          b().CreateAdd(loop_val, ConstantInt::get(ls_->Int32Ty, 1)),
          loop_i);

      b().CreateBr(top);

      b().SetInsertPoint(after);
    }

    // Phase 2, the post args
    // CT - min(Z,P) ... CT
    {
      BasicBlock* top = BasicBlock::Create(ls_->ctx(), "post_loop_top", func);
      BasicBlock* body = BasicBlock::Create(ls_->ctx(), "post_loop_body", func);
      BasicBlock* after = BasicBlock::Create(ls_->ctx(), "post_loop_cont", func);

      // *loop_i = CT - min(Z,P)
      b().CreateStore(
        b().CreateSub(
          CT,
          b().CreateSelect(b().CreateICmpSLT(Z, Pv), Z, Pv)),
        loop_i);

      // *local_i = M+O+HS
      b().CreateStore(
        ConstantInt::get(ls_->Int32Ty, M+O+HS),
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
        ConstantInt::get(ls_->Int32Ty, 0),
        ConstantInt::get(ls_->Int32Ty, offset::vars_tuple),
        local_val
      };

      // locals[local_idx] = args[loop_val]
      b().CreateStore(
          b().CreateLoad(
            b().CreateGEP(arg_ary, loop_val)),
          b().CreateGEP(vars, idx2, idx2+3));

      // *loop_i = loop_val + 1
      b().CreateStore(
          b().CreateAdd(loop_val, ConstantInt::get(ls_->Int32Ty, 1)),
          loop_i);

      // *local_i = local_val + 1;
      b().CreateStore(
          b().CreateAdd(local_val, ConstantInt::get(ls_->Int32Ty, 1)),
          local_i);

      b().CreateBr(top);

      b().SetInsertPoint(after);
    }

    // Phase 3 - optionals
    // M ... M + min(O, U)

    {
      BasicBlock* top = BasicBlock::Create(ls_->ctx(), "opt_arg_loop_top", func);
      BasicBlock* body = BasicBlock::Create(ls_->ctx(), "opt_arg_loop_body", func);
      BasicBlock* after = BasicBlock::Create(ls_->ctx(), "opt_arg_loop_cont", func);

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
        ConstantInt::get(ls_->Int32Ty, 0),
        ConstantInt::get(ls_->Int32Ty, offset::vars_tuple),
        loop_val
      };

      // locals[loop_val] = args[loop_val]
      b().CreateStore(
          b().CreateLoad(
            b().CreateGEP(arg_ary, loop_val)),
          b().CreateGEP(vars, idx2, idx2+3));

      // *loop_i = loop_val + 1
      b().CreateStore(
          b().CreateAdd(loop_val, ConstantInt::get(ls_->Int32Ty, 1)),
          loop_i);

      b().CreateBr(top);

      b().SetInsertPoint(after);
    }

    // Phase 4 - splat
    if(vmm_->splat_position >= 0) {
      Signature sig(ls_, "Object");
      sig << "VM";
      sig << "Arguments";
      sig << ls_->Int32Ty;
      sig << ls_->Int32Ty;

      Value* call_args[] = {
        vm_obj,
        arg_obj,
        ConstantInt::get(ls_->Int32Ty, M + O),
        ConstantInt::get(ls_->Int32Ty, DT)
      };

      Function* func = sig.function("rbx_construct_splat");
      func->setOnlyReadsMemory(true);
      func->setDoesNotThrow(true);

      CallInst* splat_val = sig.call("rbx_construct_splat", call_args, 4, "splat_val", b());

      splat_val->setOnlyReadsMemory(true);
      splat_val->setDoesNotThrow(true);

      Value* idx3[] = {
        ConstantInt::get(ls_->Int32Ty, 0),
        ConstantInt::get(ls_->Int32Ty, offset::vars_tuple),
        ConstantInt::get(ls_->Int32Ty, vmm_->splat_position)
      };

      Value* pos = b().CreateGEP(vars, idx3, idx3+3, "splat_pos");
      b().CreateStore(splat_val, pos);
    }
  }

}
}

#endif
