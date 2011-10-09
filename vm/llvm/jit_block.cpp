#ifdef ENABLE_LLVM

#include "llvm/jit_block.hpp"
#include "llvm/jit_context.hpp"
#include "llvm/method_info.hpp"

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

    std::ostringstream ss;
    ss << std::string("_X_")
       << ls_->enclosure_name(info_.method())
       << "#"
       << ls_->symbol_debug_str(info_.method()->name())
       << "$block@" << ls_->add_jitted_method();

    llvm::Function* func = Function::Create(ft, GlobalValue::ExternalLinkage,
                            ss.str().c_str(), ls_->module());

    Function::arg_iterator ai = func->arg_begin();
    llvm::Value* vm =   ai++; vm->setName("state");
    llvm::Value* prev = ai++; prev->setName("previous");
    block_env = ai++; block_env->setName("env");
    llvm::Value* args = ai++; args->setName("args");
    block_inv = ai++; block_inv->setName("invocation");

    BasicBlock* block = BasicBlock::Create(ls_->ctx(), "entry", func);
    b().SetInsertPoint(block);

    info_.context().set_function(func);

    info_.set_vm(vm);
    info_.set_args(args);
    info_.set_previous(prev);
    info_.set_entry(block);

    alloc_frame("block_body");

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

    b().CreateBr(body_);
    b().SetInsertPoint(body_);
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
    b().CreateStore(info_.previous(), get_field(call_frame, offset::CallFrame::previous));

    // static_scope
    Value* ss = b().CreateLoad(get_field(block_inv, offset::blockinv_static_scope),
                               "invocation.static_scope");

    b().CreateStore(ss, get_field(call_frame, offset::CallFrame::static_scope));

    // arguments
    b().CreateStore(info_.args(), get_field(call_frame, offset::CallFrame::arguments));

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
        cint(block_flags), "flags");

    b().CreateStore(flags, get_field(call_frame, offset::CallFrame::flags));

    // ip
    b().CreateStore(cint(0),
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
    if(vmm_->required_args > 1) {
      Value* lambda_check =
        b().CreateICmpEQ(
          b().CreateAnd(
            inv_flags_,
            ConstantInt::get(inv_flags_->getType(), CallFrame::cIsLambda)),
          ConstantInt::get(inv_flags_->getType(), 0));

      BasicBlock* destruct = BasicBlock::Create(ls_->ctx(), "destructure", info_.function());
      BasicBlock* cont = BasicBlock::Create(ls_->ctx(), "arg_loop_body", info_.function());

      b().CreateCondBr(lambda_check, destruct, cont);

      b().SetInsertPoint(destruct);

      Signature sig(ls_, "Object");
      sig << "VM";
      sig << "CallFrame";
      sig << "Arguments";

      Value* call_args[] = { info_.vm(), info_.call_frame(), info_.args() };

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
                       b().CreateConstGEP2_32(info_.args(), 0, offset::args_ary),
                       "arg_ary");

    // The variables used in the 4 phases.
    int P = vmm_->post_args;
    Value* Pv = cint(P);

    int R = vmm_->required_args;

    int M = R - P;
    Value* Mv = cint(M);

    Value* T = b().CreateLoad(
                 b().CreateConstGEP2_32(info_.args(), 0, offset::args_total),
                 "args.total");

    int DT = vmm_->total_args;
    Value* DTv = cint(DT);

    int O = DT - R;
    Value* Ov = cint(O);

    int HS = vmm_->splat_position > 0 ? 1 : 0;

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
        cint(offset::vars_tuple),
        loop_val
      };

      // locals[loop_val] = args[loop_val]
      b().CreateStore(
          b().CreateLoad(
            b().CreateGEP(arg_ary, loop_val)),
          b().CreateGEP(vars, idx2, idx2+3));

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
        cint(offset::vars_tuple),
        local_val
      };

      // locals[local_idx] = args[loop_val]
      b().CreateStore(
          b().CreateLoad(
            b().CreateGEP(arg_ary, loop_val)),
          b().CreateGEP(vars, idx2, idx2+3));

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
        cint(offset::vars_tuple),
        loop_val
      };

      // locals[loop_val] = args[loop_val]
      b().CreateStore(
          b().CreateLoad(
            b().CreateGEP(arg_ary, loop_val)),
          b().CreateGEP(vars, idx2, idx2+3));

      // *loop_i = loop_val + 1
      b().CreateStore(
          b().CreateAdd(loop_val, cint(1)),
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
        info_.vm(),
        info_.args(),
        cint(M + O),
        cint(DT)
      };

      Function* func = sig.function("rbx_construct_splat");
      func->setOnlyReadsMemory(true);
      func->setDoesNotThrow(true);

      CallInst* splat_val = sig.call("rbx_construct_splat", call_args, 4, "splat_val", b());

      splat_val->setOnlyReadsMemory(true);
      splat_val->setDoesNotThrow(true);

      Value* idx3[] = {
        cint(0),
        cint(offset::vars_tuple),
        cint(vmm_->splat_position)
      };

      Value* pos = b().CreateGEP(vars, idx3, idx3+3, "splat_pos");
      b().CreateStore(splat_val, pos);
    }
  }

}
}

#endif
