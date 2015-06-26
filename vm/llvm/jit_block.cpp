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
       << ++ctx_->llvm_state()->vm()->metrics().jit.methods_compiled;

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

    initialize_frame(machine_code_->stack_size);

    nil_stack(machine_code_->stack_size, constant(cNil, obj_type));

    setup_block_scope();

    if(ctx_->llvm_state()->include_profiling()) {
      BasicBlock* setup_profiling = BasicBlock::Create(ctx_->llvm_context(),
          "setup_profiling", func);
      BasicBlock* cont = BasicBlock::Create(ctx_->llvm_context(), "continue", func);

      ctx_->profiling(b(), setup_profiling, cont);

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

}
}

#endif
