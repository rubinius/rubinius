#ifdef ENABLE_LLVM

#include "llvm/jit_workhorse.hpp"
#include "instruments/profiler.hpp"
#include "call_frame.hpp"
#include "vmmethod.hpp"

#include "llvm/jit_visit.hpp"
#include "llvm/control_flow.hpp"

#include <llvm/Analysis/CaptureTracking.h>

namespace rubinius {
  LLVMWorkHorse::LLVMWorkHorse(LLVMState* ls, JITMethodInfo& i)
    : ls_(ls)
    , vmm_(i.vmm)
    , builder_(ls->ctx())
    , use_full_scope_(false)
    , import_args_(0)
    , method_body_(0)
    , info_(i)
  {
    llvm::Module* mod = ls->module();
    cf_type = mod->getTypeByName("struct.rubinius::CallFrame");
    vars_type = mod->getTypeByName("struct.rubinius::VariableScope");
    stack_vars_type = mod->getTypeByName("struct.rubinius::StackVariables");
    obj_type = ls->ptr_type("Object");
    obj_ary_type = PointerType::getUnqual(obj_type);
  }

  void LLVMWorkHorse::return_value(Value* ret, BasicBlock* cont) {
    if(ls_->include_profiling()) {
      Value* test = b().CreateLoad(ls_->profiling(), "profiling");
      BasicBlock* end_profiling = BasicBlock::Create(ls_->ctx(), "end_profiling", func);
      if(!cont) {
        cont = BasicBlock::Create(ls_->ctx(), "continue", func);
      }

      b().CreateCondBr(test, end_profiling, cont);

      b().SetInsertPoint(end_profiling);

      Signature sig(ls_, ls_->VoidTy);
      sig << PointerType::getUnqual(ls_->Int8Ty);

      Value* call_args[] = {
        method_entry_
      };

      sig.call("rbx_end_profiling", call_args, 1, "", b());

      b().CreateBr(cont);

      b().SetInsertPoint(cont);
    }

    b().CreateRet(ret);
  }

  Value* LLVMWorkHorse::get_field(Value* val, int which) {
    return b().CreateConstGEP2_32(val, 0, which);
  }

  void LLVMWorkHorse::initialize_call_frame(int stack_size) {
    Value* exec = b().CreateLoad(get_field(msg, 2), "msg.exec");
    Value* cm_gep = get_field(call_frame, offset::cf_cm);
    method = b().CreateBitCast(
        exec, cast<PointerType>(cm_gep->getType())->getElementType(), "cm");

    // previous
    b().CreateStore(prev, get_field(call_frame, offset::cf_previous));

    // msg
    b().CreateStore(msg, get_field(call_frame, offset::cf_msg));

    // cm
    b().CreateStore(method, cm_gep);

    // flags
    int flags = 0;
    if(!use_full_scope_) flags |= CallFrame::cClosedScope;

    b().CreateStore(
        ConstantInt::get(ls_->Int32Ty, flags),
        get_field(call_frame, offset::cf_flags));

    // ip
    b().CreateStore(
        ConstantInt::get(ls_->Int32Ty, 0),
        get_field(call_frame, offset::cf_ip));

    // scope
    b().CreateStore(vars, get_field(call_frame, offset::cf_scope));

    if(ls_->include_profiling()) {
      Value* test = b().CreateLoad(ls_->profiling(), "profiling");

      BasicBlock* setup_profiling = BasicBlock::Create(ls_->ctx(), "setup_profiling", func);
      BasicBlock* cont = BasicBlock::Create(ls_->ctx(), "continue", func);

      b().CreateCondBr(test, setup_profiling, cont);

      b().SetInsertPoint(setup_profiling);

      Signature sig(ls_, ls_->VoidTy);
      sig << "VM";
      sig << PointerType::getUnqual(ls_->Int8Ty);
      sig << "Dispatch";
      sig << "Arguments";
      sig << "CompiledMethod";

      Value* call_args[] = {
        vm,
        method_entry_,
        msg,
        args,
        method
      };

      sig.call("rbx_begin_profiling", call_args, 5, "", b());

      b().CreateBr(cont);

      b().SetInsertPoint(cont);
    }
  }

  void LLVMWorkHorse::initialize_block_frame(int stack_size) {
    Value* cm_gep = get_field(call_frame, offset::cf_cm);

    method = b().CreateLoad(get_field(block_env, offset::blockenv_method),
                            "env.method");

    // previous
    b().CreateStore(prev, get_field(call_frame, offset::cf_previous));

    // static_scope
    Value* ss = b().CreateLoad(get_field(block_inv, offset::blockinv_static_scope),
                               "invocation.static_scope");

    b().CreateStore(ss, get_field(call_frame, offset::cf_static_scope));

    // msg
    b().CreateStore(Constant::getNullValue(ls_->ptr_type("Dispatch")),
        get_field(call_frame, offset::cf_msg));

    // cm
    b().CreateStore(method, cm_gep);

    // flags
    Value* inv_flags = b().CreateLoad(get_field(block_inv, offset::blockinv_flags),
        "invocation.flags");

    int block_flags = CallFrame::cCustomStaticScope |
      CallFrame::cMultipleScopes;

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

    if(ls_->include_profiling()) {
      Value* test = b().CreateLoad(ls_->profiling(), "profiling");

      BasicBlock* setup_profiling = BasicBlock::Create(ls_->ctx(), "setup_profiling", func);
      BasicBlock* cont = BasicBlock::Create(ls_->ctx(), "continue", func);

      b().CreateCondBr(test, setup_profiling, cont);

      b().SetInsertPoint(setup_profiling);

      Signature sig(ls_, ls_->VoidTy);
      sig << "VM";
      sig << PointerType::getUnqual(ls_->Int8Ty);
      sig << "Dispatch";
      sig << "Arguments";
      sig << "CompiledMethod";

      Value* call_args[] = {
        vm,
        method_entry_,
        msg,
        args,
        method
      };

      sig.call("rbx_begin_profiling", call_args, 5, "", b());

      b().CreateBr(cont);

      b().SetInsertPoint(cont);
    }
  }

  void LLVMWorkHorse::nil_stack(int size, Value* nil) {
    if(size == 0) return;
    // Stack size 5 or less, do 5 stores in a row rather than
    // the loop.
    if(size <= 5) {
      for(int i = 0; i < size; i++) {
        b().CreateStore(nil, b().CreateConstGEP1_32(stk, i, "stack_pos"));
      }
      return;
    }

    Value* max = ConstantInt::get(ls_->Int32Ty, size);
    Value* one = ConstantInt::get(ls_->Int32Ty, 1);

    BasicBlock* top = BasicBlock::Create(ls_->ctx(), "stack_nil", func);
    BasicBlock* cont = BasicBlock::Create(ls_->ctx(), "bottom", func);

    b().CreateStore(ConstantInt::get(ls_->Int32Ty, 0), info_.counter());

    b().CreateBr(top);

    b().SetInsertPoint(top);

    Value* cur = b().CreateLoad(info_.counter(), "counter");
    b().CreateStore(nil, b().CreateGEP(stk, cur, "stack_pos"));

    Value* added = b().CreateAdd(cur, one, "added");
    b().CreateStore(added, info_.counter());

    Value* cmp = b().CreateICmpEQ(added, max, "loop_check");
    b().CreateCondBr(cmp, cont, top);

    b().SetInsertPoint(cont);
  }

  void LLVMWorkHorse::nil_locals() {
    Value* nil = constant(Qnil, obj_type);
    int size = vmm_->number_of_locals;

    if(size == 0) return;
    // Stack size 5 or less, do 5 stores in a row rather than
    // the loop.
    if(size <= 5) {
      for(int i = 0; i < size; i++) {
        Value* idx[] = {
          ConstantInt::get(ls_->Int32Ty, 0),
          ConstantInt::get(ls_->Int32Ty, offset::vars_tuple),
          ConstantInt::get(ls_->Int32Ty, i)
        };

        Value* gep = b().CreateGEP(vars, idx, idx+3, "local_pos");
        b().CreateStore(nil, gep);
      }
      return;
    }

    Value* max = ConstantInt::get(ls_->Int32Ty, size);
    Value* one = ConstantInt::get(ls_->Int32Ty, 1);

    BasicBlock* top = BasicBlock::Create(ls_->ctx(), "locals_nil", func);
    BasicBlock* cont = BasicBlock::Create(ls_->ctx(), "bottom", func);

    b().CreateStore(ConstantInt::get(ls_->Int32Ty, 0), info_.counter());

    b().CreateBr(top);

    b().SetInsertPoint(top);

    Value* cur = b().CreateLoad(info_.counter(), "counter");
    Value* idx[] = {
      ConstantInt::get(ls_->Int32Ty, 0),
      ConstantInt::get(ls_->Int32Ty, offset::vars_tuple),
      cur
    };

    Value* gep = b().CreateGEP(vars, idx, idx+3, "local_pos");
    b().CreateStore(nil, gep);

    Value* added = b().CreateAdd(cur, one, "added");
    b().CreateStore(added, info_.counter());

    Value* cmp = b().CreateICmpEQ(added, max, "loop_check");
    b().CreateCondBr(cmp, cont, top);

    b().SetInsertPoint(cont);
  }

  void LLVMWorkHorse::setup_scope() {
    Value* heap_null = ConstantExpr::getNullValue(PointerType::getUnqual(vars_type));
    Value* heap_pos = get_field(vars, offset::vars_on_heap);

    b().CreateStore(heap_null, heap_pos);

    Value* self = b().CreateLoad(get_field(args, offset::args_recv),
        "args.recv");
    b().CreateStore(self, get_field(vars, offset::vars_self));
    Value* mod = b().CreateLoad(get_field(msg, offset::msg_module),
        "msg.module");
    b().CreateStore(mod, get_field(vars, offset::vars_module));

    Value* blk = b().CreateLoad(get_field(args, offset::args_block),
        "args.block");
    b().CreateStore(blk, get_field(vars, offset::vars_block));

    b().CreateStore(Constant::getNullValue(ls_->ptr_type("VariableScope")),
        get_field(vars, offset::vars_parent));

    nil_locals();
  }

  void LLVMWorkHorse::setup_inline_scope(Value* self, Value* blk, Value* mod) {
    Value* heap_null = ConstantExpr::getNullValue(PointerType::getUnqual(vars_type));
    Value* heap_pos = get_field(vars, offset::vars_on_heap);
    b().CreateStore(heap_null, heap_pos);

    b().CreateStore(self, get_field(vars, offset::vars_self));
    b().CreateStore(mod, get_field(vars, offset::vars_module));

    b().CreateStore(blk, get_field(vars, offset::vars_block));

    b().CreateStore(Constant::getNullValue(ls_->ptr_type("VariableScope")),
        get_field(vars, offset::vars_parent));

    nil_locals();
  }

  void LLVMWorkHorse::setup_block_scope() {
    b().CreateStore(ConstantExpr::getNullValue(PointerType::getUnqual(vars_type)),
        get_field(vars, offset::vars_on_heap));
    Value* self = b().CreateLoad(
        get_field(block_inv, offset::blockinv_self),
        "invocation.self");

    b().CreateStore(self, get_field(vars, offset::vars_self));


    Value* inv_mod = b().CreateLoad(
        get_field(block_inv, offset::blockinv_module),
        "invocation.module");

    Value* ts_mod = b().CreateLoad(get_field(top_scope, offset::varscope_module),
        "top_scope.module");

    Value* mod = b().CreateSelect(
        b().CreateICmpNE(inv_mod, ConstantExpr::getNullValue(inv_mod->getType())),
        inv_mod,
        ts_mod);

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

    nil_locals();
  }

  void LLVMWorkHorse::check_arity() {
    Value* vm_obj = vm;
    Value* dis_obj = msg;
    Value* arg_obj = args;

    Value* total_offset = b().CreateConstGEP2_32(arg_obj, 0,
        offset::args_total, "total_pos");
    Value* total = b().CreateLoad(total_offset, "arg.total");

    // For others to use.
    arg_total = total;

    BasicBlock* arg_error = BasicBlock::Create(ls_->ctx(), "arg_error", func);
    BasicBlock* cont = BasicBlock::Create(ls_->ctx(), "import_args", func);

    // Check arguments
    //
    // if there is a splat..
    if(vmm_->splat_position >= 0) {
      if(vmm_->required_args > 0) {
        // Make sure we got at least the required args
        Value* cmp = b().CreateICmpSLT(total,
            ConstantInt::get(ls_->Int32Ty, vmm_->required_args), "arg_cmp");
        b().CreateCondBr(cmp, arg_error, cont);
      } else {
        // Only splat or optionals, no handling!
        b().CreateBr(cont);
      }

      // No splat, a precise number of args
    } else if(vmm_->required_args == vmm_->total_args) {
      // Make sure we got the exact number of arguments
      Value* cmp = b().CreateICmpNE(total,
          ConstantInt::get(ls_->Int32Ty, vmm_->required_args), "arg_cmp");
      b().CreateCondBr(cmp, arg_error, cont);

      // No splat, with optionals
    } else {
      Value* c1 = b().CreateICmpSLT(total,
          ConstantInt::get(ls_->Int32Ty, vmm_->required_args), "arg_cmp");
      Value* c2 = b().CreateICmpSGT(total,
          ConstantInt::get(ls_->Int32Ty, vmm_->total_args), "arg_cmp");

      Value* cmp = b().CreateOr(c1, c2, "arg_combine");
      b().CreateCondBr(cmp, arg_error, cont);
    }

    b().SetInsertPoint(arg_error);

    // Call our arg_error helper
    Signature sig(ls_, "Object");

    sig << "VM";
    sig << "CallFrame";
    sig << "Dispatch";
    sig << "Arguments";
    sig << ls_->Int32Ty;

    Value* call_args[] = {
      vm_obj,
      prev,
      dis_obj,
      arg_obj,
      ConstantInt::get(ls_->Int32Ty, vmm_->required_args)
    };

    Value* val = sig.call("rbx_arg_error", call_args, 5, "ret", b());
    return_value(val);

    // Switch to using continuation
    b().SetInsertPoint(cont);
  }

  void LLVMWorkHorse::import_args() {
    Value* vm_obj = vm;
    Value* arg_obj = args;

    setup_scope();

    // Import the arguments
    Value* offset = b().CreateConstGEP2_32(args, 0, offset::args_ary, "arg_ary_pos");

    Value* arg_ary = b().CreateLoad(offset, "arg_ary");

    // If there are a precise number of args, easy.
    if(vmm_->required_args == vmm_->total_args) {
      for(int i = 0; i < vmm_->required_args; i++) {
        Value* int_pos = ConstantInt::get(ls_->Int32Ty, i);

        Value* arg_val_offset = b().CreateConstGEP1_32(arg_ary, i, "arg_val_offset");

        Value* arg_val = b().CreateLoad(arg_val_offset, "arg_val");

        Value* idx2[] = {
          ConstantInt::get(ls_->Int32Ty, 0),
          ConstantInt::get(ls_->Int32Ty, offset::vars_tuple),
          int_pos
        };

        Value* pos = b().CreateGEP(vars, idx2, idx2+3, "var_pos");

        b().CreateStore(arg_val, pos);
      }

      // Otherwise, we must loop in the generate code because we don't know
      // how many they've actually passed in.
    } else {
      Value* loop_i = info_.counter();

      BasicBlock* top = BasicBlock::Create(ls_->ctx(), "arg_loop_top", func);
      BasicBlock* body = BasicBlock::Create(ls_->ctx(), "arg_loop_body", func);
      BasicBlock* after = BasicBlock::Create(ls_->ctx(), "arg_loop_cont", func);

      b().CreateStore(ConstantInt::get(ls_->Int32Ty, 0), loop_i);
      b().CreateBr(top);

      b().SetInsertPoint(top);

      // now at the top of block, check if we should continue...
      Value* loop_val = b().CreateLoad(loop_i, "loop_val");
      Value* cmp = b().CreateICmpSLT(loop_val, arg_total, "loop_test");

      b().CreateCondBr(cmp, body, after);

      // Now, the body
      b().SetInsertPoint(body);

      Value* arg_val_offset =
        b().CreateGEP(arg_ary, loop_val, "arg_val_offset");

      Value* arg_val = b().CreateLoad(arg_val_offset, "arg_val");

      Value* idx2[] = {
        ConstantInt::get(ls_->Int32Ty, 0),
        ConstantInt::get(ls_->Int32Ty, offset::vars_tuple),
        loop_val
      };

      Value* pos = b().CreateGEP(vars, idx2, idx2+3, "var_pos");

      b().CreateStore(arg_val, pos);

      Value* plus_one = b().CreateAdd(loop_val,
          ConstantInt::get(ls_->Int32Ty, 1), "add");
      b().CreateStore(plus_one, loop_i);

      b().CreateBr(top);

      b().SetInsertPoint(after);
    }

    // Setup the splat.
    if(vmm_->splat_position >= 0) {
      Signature sig(ls_, "Object");
      sig << "VM";
      sig << "Arguments";
      sig << ls_->Int32Ty;

      Value* call_args[] = {
        vm_obj,
        arg_obj,
        ConstantInt::get(ls_->Int32Ty, vmm_->total_args)
      };

      Function* func = sig.function("rbx_construct_splat");
      func->setOnlyReadsMemory(true);
      func->setDoesNotThrow(true);

      CallInst* splat_val = sig.call("rbx_construct_splat", call_args, 3, "splat_val", b());

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

  void LLVMWorkHorse::setup_block() {
    Signature sig(ls_, "Object");
    sig << "VM";
    sig << "CallFrame";
    sig << "BlockEnvironment";
    sig << "Arguments";
    sig << "BlockInvocation";

    func = sig.function("");

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

    valid_flag = b().CreateAlloca(ls_->Int1Ty, 0, "valid_flag");

    Value* cfstk = b().CreateAlloca(obj_type,
        ConstantInt::get(ls_->Int32Ty,
          (sizeof(CallFrame) / sizeof(Object*)) + vmm_->stack_size),
        "cfstk");

    call_frame = b().CreateBitCast(
        cfstk,
        PointerType::getUnqual(cf_type), "call_frame");

    info_.set_out_args(b().CreateAlloca(ls_->type("Arguments"), 0, "out_args"));

    if(ls_->include_profiling()) {
      method_entry_ = b().CreateAlloca(ls_->Int8Ty,
          ConstantInt::get(ls_->Int32Ty, sizeof(profiler::MethodEntry)),
          "method_entry");
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
        PointerType::getUnqual(stack_vars_type), "vars");

    info_.set_variables(vars);

    initialize_block_frame(vmm_->stack_size);

    nil_stack(vmm_->stack_size, constant(Qnil, obj_type));

    setup_block_scope();
    b().CreateBr(body);

    b().SetInsertPoint(body);
  }

  void LLVMWorkHorse::check_self_type() {
    int klass_id = 0;
    {
      AccessManagedMemory memguard(ls_);
      if(Class* cls = try_as<Class>(vmm_->original.get()->scope()->module())) {
        klass_id = cls->class_id();
      } else {
        return;
      }
    }

    // Now, validate class_id

    Value* self = b().CreateLoad(
        b().CreateConstGEP2_32(args, 0, offset::args_recv), "self");

    BasicBlock* restart_interp = BasicBlock::Create(ls_->ctx(), "restart_interp", func);
    BasicBlock* check_class = BasicBlock::Create(ls_->ctx(), "check_class", func);
    BasicBlock* cont = BasicBlock::Create(ls_->ctx(), "prologue_continue", func);

    Value* mask = ConstantInt::get(ls_->Int32Ty, TAG_REF_MASK);
    Value* zero = ConstantInt::get(ls_->Int32Ty, TAG_REF);

    Value* lint = b().CreateAnd(
        b().CreatePtrToInt(self, ls_->Int32Ty),
        mask, "masked");

    Value* is_ref = b().CreateICmpEQ(lint, zero, "is_reference");

    b().CreateCondBr(is_ref, check_class, restart_interp);

    b().SetInsertPoint(check_class);

    Value* class_idx[] = {
      ConstantInt::get(ls_->Int32Ty, 0),
      ConstantInt::get(ls_->Int32Ty, 0),
      ConstantInt::get(ls_->Int32Ty, 1)
    };

    Value* self_class = b().CreateLoad(
        b().CreateGEP(self, class_idx, class_idx+3),
        "class");

    Value* runtime_id = b().CreateLoad(
        b().CreateConstGEP2_32(self_class, 0, 3),
        "class_id");

    Value* equal = b().CreateICmpEQ(runtime_id,
        ConstantInt::get(ls_->Int32Ty, klass_id));

    b().CreateCondBr(equal, cont, restart_interp);

    b().SetInsertPoint(restart_interp);

    Value* call_args[] = { vm, prev, msg, args };

    Signature sig(ls_, "Object");
    sig << "VM";
    sig << "CallFrame";
    sig << "Dispatch";
    sig << "Arguments";

    b().CreateRet(sig.call("rbx_restart_interp", call_args, 4, "ir", b()));

    b().SetInsertPoint(cont);
  }

  void LLVMWorkHorse::setup() {
    std::vector<const Type*> ftypes;
    ftypes.push_back(ls_->ptr_type("VM"));
    ftypes.push_back(ls_->ptr_type("CallFrame"));
    ftypes.push_back(ls_->ptr_type("Dispatch"));
    ftypes.push_back(ls_->ptr_type("Arguments"));

    FunctionType* ft = FunctionType::get(ls_->ptr_type("Object"), ftypes, false);

    std::string name = std::string("_X_") +
                         ls_->symbol_cstr(vmm_->original->scope()->module()->name()) +
                         "#" +
                         ls_->symbol_cstr(vmm_->original->name());

    func = Function::Create(ft, GlobalValue::ExternalLinkage,
                            name.c_str(), ls_->module());

    Function::arg_iterator ai = func->arg_begin();
    vm =   ai++; vm->setName("state");
    prev = ai++; prev->setName("previous");
    msg =  ai++; msg->setName("msg");
    args = ai++; args->setName("args");

    BasicBlock* block = BasicBlock::Create(ls_->ctx(), "entry", func);
    builder_.SetInsertPoint(block);

    info_.set_function(func);
    info_.set_vm(vm);
    info_.set_args(args);
    info_.set_previous(prev);
    info_.set_entry(block);

    BasicBlock* body = BasicBlock::Create(ls_->ctx(), "method_body", func);
    method_body_ = body;

    pass_one(body);

    valid_flag = b().CreateAlloca(ls_->Int1Ty, 0, "valid_flag");
    info_.set_counter(b().CreateAlloca(ls_->Int32Ty, 0, "counter_alloca"));

    Value* cfstk = b().CreateAlloca(obj_type,
        ConstantInt::get(ls_->Int32Ty,
          (sizeof(CallFrame) / sizeof(Object*)) + vmm_->stack_size),
        "cfstk");

    Value* var_mem = b().CreateAlloca(obj_type,
        ConstantInt::get(ls_->Int32Ty,
          (sizeof(StackVariables) / sizeof(Object*)) + vmm_->number_of_locals),
        "var_mem");

    info_.set_out_args(b().CreateAlloca(ls_->type("Arguments"), 0, "out_args"));

    if(ls_->include_profiling()) {
      method_entry_ = b().CreateAlloca(ls_->Int8Ty,
          ConstantInt::get(ls_->Int32Ty, sizeof(profiler::MethodEntry)),
          "method_entry");
    }

    check_arity();

    // check_self_type();

    call_frame = b().CreateBitCast(
        cfstk,
        PointerType::getUnqual(cf_type), "call_frame");

    info_.set_call_frame(call_frame);

    stk = b().CreateConstGEP1_32(cfstk, sizeof(CallFrame) / sizeof(Object*), "stack");

    info_.set_stack(stk);

    vars = b().CreateBitCast(
        var_mem,
        PointerType::getUnqual(stack_vars_type), "vars");

    info_.set_variables(vars);

    initialize_call_frame(vmm_->stack_size);

    nil_stack(vmm_->stack_size, constant(Qnil, obj_type));

    import_args();

    import_args_ = b().GetInsertBlock();

    b().CreateBr(body);
    b().SetInsertPoint(body);
  }

  BasicBlock* LLVMWorkHorse::setup_inline(Value* self, Value* blk,
      Value* mod, std::vector<Value*>& stack_args)
  {
    func = info_.function();
    vm = info_.vm();
    prev = info_.parent_call_frame();
    args = ConstantExpr::getNullValue(ls_->ptr_type("Arguments"));

    BasicBlock* entry = BasicBlock::Create(ls_->ctx(), "inline_entry", func);
    b().SetInsertPoint(entry);

    info_.set_args(args);
    info_.set_previous(prev);
    info_.set_entry(entry);

    BasicBlock* body = BasicBlock::Create(ls_->ctx(), "method_body", func);
    pass_one(body);

    BasicBlock* alloca_block = &func->getEntryBlock();

    Value* cfstk = new AllocaInst(obj_type,
        ConstantInt::get(ls_->Int32Ty,
          (sizeof(CallFrame) / sizeof(Object*)) + vmm_->stack_size),
        "cfstk", alloca_block->getTerminator());

    call_frame = b().CreateBitCast(
        cfstk,
        PointerType::getUnqual(cf_type), "call_frame");

    stk = b().CreateConstGEP1_32(cfstk, sizeof(CallFrame) / sizeof(Object*), "stack");

    info_.set_call_frame(call_frame);
    info_.set_stack(stk);

    Value* var_mem = new AllocaInst(obj_type,
        ConstantInt::get(ls_->Int32Ty,
          (sizeof(StackVariables) / sizeof(Object*)) + vmm_->number_of_locals),
        "var_mem", alloca_block->getTerminator());

    vars = b().CreateBitCast(
        var_mem,
        PointerType::getUnqual(stack_vars_type), "vars");

    info_.set_variables(vars);

    //  Setup the CallFrame
    //
    // previous
    b().CreateStore(prev, get_field(call_frame, offset::cf_previous));

    // msg
    b().CreateStore(ConstantExpr::getNullValue(ls_->ptr_type("Dispatch")),
        get_field(call_frame, offset::cf_msg));

    // cm
    Value* obj_addr = constant(vmm_->original.object_address(),
        PointerType::getUnqual(ls_->ptr_type("CompiledMethod")));

    method = b().CreateLoad(obj_addr, "cm");
    Value* cm_gep = get_field(call_frame, offset::cf_cm);
    b().CreateStore(method, cm_gep);

    // flags
    int flags = CallFrame::cInlineFrame;
    if(!use_full_scope_) flags |= CallFrame::cClosedScope;

    b().CreateStore(ConstantInt::get(ls_->Int32Ty, flags),
        get_field(call_frame, offset::cf_flags));

    // ip
    b().CreateStore(ConstantInt::get(ls_->Int32Ty, 0),
        get_field(call_frame, offset::cf_ip));

    // scope
    b().CreateStore(vars, get_field(call_frame, offset::cf_scope));

    nil_stack(vmm_->stack_size, constant(Qnil, obj_type));

    setup_inline_scope(self, blk, mod);

    // We know the right arguments are present, so we just need to put them
    // in the right place.
    //
    // We don't support splat in an inlined method!
    assert(vmm_->splat_position < 0);

    assert(stack_args.size() <= (size_t)vmm_->total_args);

    for(size_t i = 0; i < stack_args.size(); i++) {
      Value* int_pos = ConstantInt::get(ls_->Int32Ty, i);

      Value* idx2[] = {
        ConstantInt::get(ls_->Int32Ty, 0),
        ConstantInt::get(ls_->Int32Ty, offset::vars_tuple),
        int_pos
      };

      Value* pos = b().CreateGEP(vars, idx2, idx2+3, "local_pos");

      b().CreateStore(stack_args[i], pos);
    }

    b().CreateBr(body);
    b().SetInsertPoint(body);

    return entry;
  }

  BasicBlock* LLVMWorkHorse::setup_inline_block(Value* self, Value* mod) {
    func = info_.function();
    vm = info_.vm();
    prev = info_.parent_call_frame();
    args = ConstantExpr::getNullValue(ls_->ptr_type("Arguments"));

    BasicBlock* entry = BasicBlock::Create(ls_->ctx(), "inline_entry", func);
    b().SetInsertPoint(entry);

    info_.set_args(args);
    info_.set_previous(prev);
    info_.set_entry(entry);

    BasicBlock* body = BasicBlock::Create(ls_->ctx(), "block_body", func);
    pass_one(body);

    BasicBlock* alloca_block = &func->getEntryBlock();

    Value* cfstk = new AllocaInst(obj_type,
        ConstantInt::get(ls_->Int32Ty,
          (sizeof(CallFrame) / sizeof(Object*)) + vmm_->stack_size),
        "cfstk", alloca_block->getTerminator());

    call_frame = b().CreateBitCast(
        cfstk,
        PointerType::getUnqual(cf_type), "call_frame");

    info_.set_call_frame(call_frame);

    stk = b().CreateConstGEP1_32(cfstk, sizeof(CallFrame) / sizeof(Object*), "stack");

    info_.set_stack(stk);

    Value* var_mem = new AllocaInst(obj_type,
        ConstantInt::get(ls_->Int32Ty,
          (sizeof(StackVariables) / sizeof(Object*)) + vmm_->number_of_locals),
        "var_mem", alloca_block->getTerminator());

    vars = b().CreateBitCast(
        var_mem,
        PointerType::getUnqual(stack_vars_type), "vars");

    info_.set_variables(vars);

    //  Setup the CallFrame
    //
    // previous
    b().CreateStore(prev, get_field(call_frame, offset::cf_previous));

    // msg
    b().CreateStore(ConstantExpr::getNullValue(ls_->ptr_type("Dispatch")),
        get_field(call_frame, offset::cf_msg));

    // cm
    Value* obj_addr = constant(vmm_->original.object_address(),
        PointerType::getUnqual(ls_->ptr_type("CompiledMethod")));

    method = b().CreateLoad(obj_addr, "cm");
    Value* cm_gep = get_field(call_frame, offset::cf_cm);
    b().CreateStore(method, cm_gep);

    // flags
    int flags = CallFrame::cInlineFrame;
    if(!use_full_scope_) flags |= CallFrame::cClosedScope;

    b().CreateStore(ConstantInt::get(ls_->Int32Ty, flags),
        get_field(call_frame, offset::cf_flags));

    // ip
    b().CreateStore(ConstantInt::get(ls_->Int32Ty, 0),
        get_field(call_frame, offset::cf_ip));

    // scope
    b().CreateStore(vars, get_field(call_frame, offset::cf_scope));

    nil_stack(vmm_->stack_size, constant(Qnil, obj_type));

    setup_inline_scope(self, constant(Qnil, obj_type), mod);

    // No argument handling, there are bytecodes in the body that
    // do that. We just have to make stack_args available.

    b().CreateBr(body);
    b().SetInsertPoint(body);

    return entry;
  }

  class PassOne : public VisitInstructions<PassOne> {
    LLVMState* ls_;
    BlockMap& map_;
    Function* function_;
    opcode current_ip_;
    int force_break_;
    bool creates_blocks_;
    int number_of_sends_;
    bool loops_;
    int sp_;
    JITBasicBlock* current_block_;
    bool calls_evalish_;

    Symbol* s_eval_;
    Symbol* s_binding_;
    Symbol* s_class_eval_;
    Symbol* s_module_eval_;

    std::list<JITBasicBlock*> exception_handlers_;

  public:

    PassOne(LLVMState* ls, BlockMap& map, Function* func, BasicBlock* start)
      : ls_(ls)
      , map_(map)
      , function_(func)
      , current_ip_(0)
      , force_break_(false)
      , creates_blocks_(false)
      , number_of_sends_(0)
      , loops_(false)
      , sp_(-1)
      , calls_evalish_(false)
    {
      JITBasicBlock& jbb = map_[0];
      jbb.reachable = true;
      jbb.block = start;

      current_block_ = &jbb;

      s_eval_ = ls->symbol("eval");
      s_binding_ = ls->symbol("binding");
      s_class_eval_ = ls->symbol("class_eval");
      s_module_eval_ = ls->symbol("module_eval");

      // By default, there is no handler.
      exception_handlers_.push_back(0);
    }

    bool calls_evalish() {
      return calls_evalish_;
    }

    bool creates_blocks() {
      return creates_blocks_;
    }

    int number_of_sends() {
      return number_of_sends_;
    }

    bool loops_p() {
      return loops_;
    }

    void at_ip(int ip) {
      current_ip_ = ip;

      // If this is a new block, reset sp here
      /*
      BlockMap::iterator i = map_.find(ip);
      if(i != map_.end()) {
        sp_ = i->second.sp;
      }
      */
    }

    const static int cUnknown = -10;
    const static bool cDebugStack = false;

#include "gen/instruction_effects.hpp"

    bool before(opcode op, opcode arg1=0, opcode arg2=0) {
      // Handle pop_unwind specially, so we always see it.
      if(op == InstructionSequence::insn_pop_unwind) {
        exception_handlers_.pop_back();
      }

      BlockMap::iterator i = map_.find(current_ip_);
      if(i != map_.end()) {
        // Setup the exception handler for this block
        i->second.exception_handler = exception_handlers_.back();

        if(i->second.sp == cUnknown) {
          if(cDebugStack) {
            std::cout << current_ip_ << ": " << sp_ << " (inherit)\n";
          }
          i->second.sp = sp_;
        } else {
          sp_ = i->second.sp;
          if(cDebugStack) {
            std::cout << current_ip_ << ": " << sp_ << " (reset)\n";
          }
        }

        current_block_ = &i->second;
      } else {
        if(force_break_) {
          if(cDebugStack) {
            std::cout << current_ip_ << ": dead\n";
          }
          return false;
        }

        if(cDebugStack) {
          std::cout << current_ip_ << ": " << sp_ << "\n";
        }
      }

      // Update current_block everytime. When current_block changes,
      // previous current blocks will thereby contain their real end_ip
      current_block_->end_ip = current_ip_;

      force_break_ = false;
      if(sp_ != cUnknown) {
        sp_ += stack_difference(op, arg1, arg2);
        assert(sp_ >= -1);
      }

      return true;
    }

    JITBasicBlock* break_at(opcode ip) {
      BlockMap::iterator i = map_.find(ip);
      if(i == map_.end()) {
        std::ostringstream ss;
        ss << "ip" << ip;
        JITBasicBlock& jbb = map_[ip];
        jbb.block = BasicBlock::Create(ls_->ctx(), ss.str().c_str(), function_);
        jbb.start_ip = ip;
        jbb.sp = sp_;

        if(ip < current_ip_) {
          jbb.end_ip = current_ip_;
        }

        if(cDebugStack) {
          std::cout << "patch " << ip << ": " << jbb.sp << "\n";
        }
        return &jbb;
      } else {
        assert(i->second.sp == sp_);
        return &(i->second);
      }
    }

    void next_ip_too() {
      force_break_ = true;
    }

    void visit_goto(opcode which) {
      if(current_ip_ < which) loops_ = true;

      break_at(which);
      next_ip_too();
    }

    void visit_goto_if_true(opcode which) {
      if(current_ip_ < which) loops_ = true;

      break_at(which);
      break_at(current_ip_ + 2);
    }

    void visit_goto_if_false(opcode which) {
      if(current_ip_ < which) loops_ = true;

      break_at(which);
      break_at(current_ip_ + 2);
    }

    void visit_goto_if_defined(opcode which) {
      if(current_ip_ < which) loops_ = true;

      break_at(which);
      break_at(current_ip_ + 2);
    }

    void visit_setup_unwind(opcode which, opcode type) {
      // setup_unwind must always refer to an instruction further
      // on in the stream
      assert(current_ip_ < which);

      JITBasicBlock* jbb = break_at(which);

      // Install the handler...
      exception_handlers_.push_back(jbb);

      // Break at the next IP. When we advance to it, the logic
      // above will install the handler we just installed on it.
      break_at(current_ip_ + 3);
    }

    void visit_ret() {
      next_ip_too();
    }

    void visit_raise_return() {
      next_ip_too();
    }

    void visit_raise_break() {
      next_ip_too();
    }

    void visit_ensure_return() {
      next_ip_too();
    }

    void visit_reraise() {
      next_ip_too();
    }

    void visit_raise_exc() {
      next_ip_too();
    }

    void visit_create_block(opcode which) {
      creates_blocks_ = true;
    }

    void check_for_eval(opcode which) {
      InlineCache* ic = reinterpret_cast<InlineCache*>(which);
      if(ic->name == s_eval_ ||
          ic->name == s_binding_ ||
          ic->name == s_class_eval_ ||
          ic->name == s_module_eval_) {
        calls_evalish_ = true;
      }
    }

    void visit_send_stack(opcode which, opcode args) {
      check_for_eval(which);
      number_of_sends_++;
    }

    void visit_send_method(opcode which) {
      number_of_sends_++;
    }

    void visit_send_stack_with_block(opcode which, opcode args) {
      number_of_sends_++;
    }

    void visit_send_stack_with_splat(opcode which, opcode args) {
      check_for_eval(which);
      number_of_sends_++;
    }

    void visit_send_super_stack_with_block(opcode which, opcode args) {
      number_of_sends_++;
    }

    void visit_send_super_stack_with_splat(opcode which, opcode args) {
      number_of_sends_++;
    }
  };

  void LLVMWorkHorse::pass_one(BasicBlock* body) {
    // Pass 1, detect BasicBlock boundaries
    PassOne finder(ls_, block_map_, func, body);
    finder.drive(vmm_);

    if(finder.creates_blocks() || finder.calls_evalish()) {
      info_.set_use_full_scope();
      use_full_scope_ = true;
    }

    number_of_sends_ = finder.number_of_sends();
    loops_ = finder.loops_p();
  }

  class Walker {
    JITVisit& v_;
    BlockMap& map_;

  public:
    Walker(JITVisit& v, BlockMap& map)
      : v_(v)
      , map_(map)
    {}

    void call(OpcodeIterator& iter) {
      v_.dispatch(iter.stream(), iter.ip());

      if(v_.b().GetInsertBlock()->getTerminator() == NULL) {
        BlockMap::iterator i = map_.find(iter.next_ip());
        if(i != map_.end()) {
          v_.b().CreateBr(i->second.block);
        }
      }
    }
  };

  bool LLVMWorkHorse::generate_body() {
    JITVisit visitor(ls_, info_, block_map_, b().GetInsertBlock());

    if(info_.inline_policy) {
      visitor.set_policy(info_.inline_policy);
    } else {
      visitor.init_policy();
    }

    visitor.set_called_args(info_.called_args);

    visitor.set_valid_flag(valid_flag);

    if(use_full_scope_) visitor.use_full_scope();

    visitor.initialize();

    // Pass 2, compile!
    // Drive by following the control flow.
    jit::ControlFlowWalker walker(info_.vmm);
    Walker cb(visitor, block_map_);

    try {
      walker.run<Walker>(cb);
    } catch(JITVisit::Unsupported &e) {
      return false;
    }

    // See if we should check interrupts now
    if(method_body_ && (visitor.sends_done() > 2 || loops_)) {
      BasicBlock* cur = b().GetInsertBlock();

      // Remove the branch to method_body
      import_args_->back().eraseFromParent();

      b().SetInsertPoint(import_args_);
      Signature sig(ls_, obj_type);
      sig << "VM";
      sig << "CallFrame";

      Function* func_ci = sig.function("rbx_check_interrupts");
      func_ci->setDoesNotCapture(1, true);
      func_ci->setDoesNotCapture(2, true);

      Value* call_args[] = { vm, call_frame };

      BasicBlock* ret_null = BasicBlock::Create(ls_->ctx(), "ret_null", func);

      Value* ret = sig.call("rbx_prologue_check", call_args, 2, "ci", b());
      b().CreateCondBr(
          b().CreateICmpEQ(ret, Constant::getNullValue(obj_type)),
          ret_null, method_body_);

      b().SetInsertPoint(ret_null);
      b().CreateRet(Constant::getNullValue(obj_type));

      b().SetInsertPoint(cur);
    }

    // debugging/optimization test code
    /*
    if(llvm::PointerMayBeCaptured(stk, true)) {
      std::cout << "Stack is captured!\n";
    } else {
      std::cout << "Stack is NOT captured!\n";
    }
    */

    info_.return_value = visitor.return_value();
    info_.fin_block = visitor.current_block();
    return true;
  }
}

#endif
