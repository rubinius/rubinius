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
    Value* cm_gep = get_field(call_frame, offset::cf_cm);

    method = b().CreateLoad(get_field(block_env, offset::blockenv_code),
                            "env.code");

    // previous
    b().CreateStore(prev, get_field(call_frame, offset::cf_previous));

    // static_scope
    Value* ss = b().CreateLoad(get_field(block_inv, offset::blockinv_static_scope),
                               "invocation.static_scope");

    b().CreateStore(ss, get_field(call_frame, offset::cf_static_scope));

    // arguments
    b().CreateStore(args, get_field(call_frame, offset::cf_arguments));

    // msg
    b().CreateStore(Constant::getNullValue(ls_->Int8PtrTy),
        get_field(call_frame, offset::cf_msg));

    // cm
    b().CreateStore(method, cm_gep);

    // flags
    Value* inv_flags = b().CreateLoad(get_field(block_inv, offset::blockinv_flags),
        "invocation.flags");

    int block_flags = CallFrame::cCustomStaticScope |
      CallFrame::cMultipleScopes |
      CallFrame::cBlock |
      CallFrame::cJITed;

    if(!use_full_scope_) block_flags |= CallFrame::cClosedScope;

    Value* flags = b().CreateOr(inv_flags,
        ConstantInt::get(ls_->Int32Ty, block_flags), "flags");

    b().CreateStore(flags, get_field(call_frame, offset::cf_flags));

    // ip
    b().CreateStore(ConstantInt::get(ls_->Int32Ty, 0),
        get_field(call_frame, offset::cf_ip));

    // scope
    b().CreateStore(vars, get_field(call_frame, offset::cf_scope));

    // top_scope
    top_scope = b().CreateLoad(
        get_field(block_env, offset::blockenv_top_scope),
        "env.top_scope");

    b().CreateStore(top_scope, get_field(call_frame, offset::cf_top_scope));

    // jit_data
    b().CreateStore(
        constant(info_.context().runtime_data_holder(), ls_->Int8PtrTy),
        get_field(call_frame, offset::cf_jit_data));

  }
}
}

#endif
