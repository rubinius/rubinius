#ifdef ENABLE_LLVM

#include "llvm/jit_workhorse.hpp"
#include "instruments/profiler.hpp"
#include "call_frame.hpp"
#include "vmmethod.hpp"

#include "llvm/jit_visit.hpp"

static Value* get_field(BasicBlock* block, Value* val, int which) {
  Value* idx[] = {
    ConstantInt::get(Type::Int32Ty, 0),
    ConstantInt::get(Type::Int32Ty, which)
  };
  Value* gep = GetElementPtrInst::Create(val, idx, idx+2, "gep", block);
  return gep;
}

template <typename T>
static Value* constant(T obj, const Type* obj_type, BasicBlock* block) {
  return CastInst::Create(
      Instruction::IntToPtr,
      ConstantInt::get(Type::Int32Ty, (intptr_t)obj),
      obj_type, "cast_to_obj", block);
}

namespace rubinius {
  void LLVMWorkHorse::return_value(Value* ret, BasicBlock* cont) {
    if(ls_->include_profiling()) {
      Value* test = new LoadInst(ls_->profiling(), "profiling", block);
      BasicBlock* end_profiling = BasicBlock::Create("end_profiling", func);
      if(!cont) {
        cont = BasicBlock::Create("continue", func);
      }

      BranchInst::Create(end_profiling, cont, test, block);

      block = end_profiling;

      Signature sig(ls_, Type::VoidTy);
      sig << PointerType::getUnqual(Type::Int8Ty);

      Value* call_args[] = {
        method_entry_
      };

      sig.call("rbx_end_profiling", call_args, 1, "", block);

      BranchInst::Create(cont, block);

      block = cont;
    }

    ReturnInst::Create(ret, block);
  }

  void LLVMWorkHorse::initialize_call_frame(int stack_size) {
    Value* exec = new LoadInst(get_field(block, msg, 2), "msg.exec", block);
    Value* cm_gep = get_field(block, call_frame, offset::cf_cm);
    method = CastInst::Create(
        Instruction::BitCast,
        exec,
        cast<PointerType>(cm_gep->getType())->getElementType(),
        "cm", block);

    // previous
    new StoreInst(prev, get_field(block, call_frame, offset::cf_previous),
        false, block);

    // msg
    new StoreInst(msg, get_field(block, call_frame, offset::cf_msg),
        false, block);

    // cm
    new StoreInst(method, cm_gep, false, block);

    // flags
    new StoreInst(ConstantInt::get(Type::Int32Ty, 0),
        get_field(block, call_frame, offset::cf_flags), false, block);

    // ip
    new StoreInst(ConstantInt::get(Type::Int32Ty, 0),
        get_field(block, call_frame, offset::cf_ip), false, block);

    // scope
    new StoreInst(vars, get_field(block, call_frame, offset::cf_scope),
        false, block);

    if(ls_->include_profiling()) {
      method_entry_ = new AllocaInst(Type::Int8Ty,
          ConstantInt::get(Type::Int32Ty, sizeof(profiler::MethodEntry)),
          "method_entry", block);

      Value* test = new LoadInst(ls_->profiling(), "profiling", block);

      BasicBlock* setup_profiling = BasicBlock::Create("setup_profiling", func);
      BasicBlock* cont = BasicBlock::Create("continue", func);

      BranchInst::Create(setup_profiling, cont, test, block);

      block = setup_profiling;

      Signature sig(ls_, Type::VoidTy);
      sig << "VM";
      sig << PointerType::getUnqual(Type::Int8Ty);
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

      sig.call("rbx_begin_profiling", call_args, 5, "", block);

      BranchInst::Create(cont, block);

      block = cont;
    }
  }

  void LLVMWorkHorse::initialize_block_frame(int stack_size) {
    Value* cm_gep = get_field(block, call_frame, offset::cf_cm);

    method = new LoadInst(get_field(block, block_env, offset::blockenv_method),
        "env.method", block);

    // previous
    new StoreInst(prev, get_field(block, call_frame, offset::cf_previous),
        false, block);

    // static_scope
    Value* ss = new LoadInst(get_field(block, block_inv, offset::blockinv_static_scope),
        "invocation.static_scope", block);

    new StoreInst(ss, get_field(block, call_frame, offset::cf_static_scope),
        false, block);

    // msg
    new StoreInst(Constant::getNullValue(ls_->ptr_type("Dispatch")),
        get_field(block, call_frame, offset::cf_msg), false, block);

    // cm
    new StoreInst(method, cm_gep, false, block);

    // flags
    Value* inv_flags = new LoadInst(get_field(block, block_inv, offset::blockinv_flags),
        "invocation.flags", block);

    int block_flags = CallFrame::cCustomStaticScope |
      CallFrame::cMultipleScopes;

    Value* flags = BinaryOperator::CreateOr(inv_flags,
        ConstantInt::get(Type::Int32Ty, block_flags), "flags", block);

    new StoreInst(flags,
        get_field(block, call_frame, offset::cf_flags), false, block);

    // ip
    new StoreInst(ConstantInt::get(Type::Int32Ty, 0),
        get_field(block, call_frame, offset::cf_ip), false, block);

    // scope
    new StoreInst(vars, get_field(block, call_frame, offset::cf_scope),
        false, block);

    // top_scope
    top_scope = new LoadInst(
        get_field(block, block_env, offset::blockenv_top_scope),
        "env.top_scope", block);

    new StoreInst(top_scope, get_field(block, call_frame, offset::cf_top_scope),
        false, block);

    if(ls_->include_profiling()) {
      method_entry_ = new AllocaInst(Type::Int8Ty,
          ConstantInt::get(Type::Int32Ty, sizeof(profiler::MethodEntry)),
          "method_entry", block);

      Value* test = new LoadInst(ls_->profiling(), "profiling", block);

      BasicBlock* setup_profiling = BasicBlock::Create("setup_profiling", func);
      BasicBlock* cont = BasicBlock::Create("continue", func);

      BranchInst::Create(setup_profiling, cont, test, block);

      block = setup_profiling;

      Signature sig(ls_, Type::VoidTy);
      sig << "VM";
      sig << PointerType::getUnqual(Type::Int8Ty);
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

      sig.call("rbx_begin_profiling", call_args, 5, "", block);

      BranchInst::Create(cont, block);

      block = cont;
    }
  }

  void LLVMWorkHorse::nil_stack(int size, Value* nil) {
    if(size == 0) return;
    // Stack size 5 or less, do 5 stores in a row rather than
    // the loop.
    if(size <= 5) {
      for(int i = 0; i < size; i++) {
        Value* idx[] = {
          ConstantInt::get(Type::Int32Ty, i)
        };

        Value* gep = GetElementPtrInst::Create(stk, idx, idx+1, "stack_pos", block);
        new StoreInst(nil, gep, block);
      }
      return;
    }

    Value* max = ConstantInt::get(Type::Int32Ty, size);
    Value* one = ConstantInt::get(Type::Int32Ty, 1);

    BasicBlock* top = BasicBlock::Create("stack_nil", func);
    BasicBlock* cont = BasicBlock::Create("bottom", func);

    Value* counter = new AllocaInst(Type::Int32Ty, 0, "counter_alloca", block);
    new StoreInst(ConstantInt::get(Type::Int32Ty, 0), counter, block);

    BranchInst::Create(top, block);

    Value* cur = new LoadInst(counter, "counter", top);
    Value* idx[] = { cur };

    Value* gep = GetElementPtrInst::Create(stk, idx, idx+1, "stack_pos", top);
    new StoreInst(nil, gep, top);

    Value* added = BinaryOperator::CreateAdd(cur, one, "added", top);
    new StoreInst(added, counter, top);

    Value* cmp = new ICmpInst(ICmpInst::ICMP_EQ, added, max, "loop_check", top);
    BranchInst::Create(cont, top, cmp, top);

    block = cont;
  }

  void LLVMWorkHorse::nil_locals(VMMethod* vmm) {
    Value* nil = constant(Qnil, obj_type, block);
    int size = vmm->number_of_locals;

    if(size == 0) return;
    // Stack size 5 or less, do 5 stores in a row rather than
    // the loop.
    if(size <= 5) {
      for(int i = 0; i < size; i++) {
        Value* idx[] = {
          ConstantInt::get(Type::Int32Ty, 0),
          ConstantInt::get(Type::Int32Ty, offset::vars_tuple),
          ConstantInt::get(Type::Int32Ty, i)
        };

        Value* gep = GetElementPtrInst::Create(vars, idx, idx+3, "local_pos", block);
        new StoreInst(nil, gep, block);
      }
      return;
    }

    Value* max = ConstantInt::get(Type::Int32Ty, size);
    Value* one = ConstantInt::get(Type::Int32Ty, 1);

    BasicBlock* top = BasicBlock::Create("locals_nil", func);
    BasicBlock* cont = BasicBlock::Create("bottom", func);

    Value* counter = new AllocaInst(Type::Int32Ty, 0, "counter_alloca", block);
    new StoreInst(ConstantInt::get(Type::Int32Ty, 0), counter, block);

    BranchInst::Create(top, block);

    Value* cur = new LoadInst(counter, "counter", top);
    Value* idx[] = {
      ConstantInt::get(Type::Int32Ty, 0),
      ConstantInt::get(Type::Int32Ty, offset::vars_tuple),
      cur
    };

    Value* gep = GetElementPtrInst::Create(vars, idx, idx+3, "local_pos", top);
    new StoreInst(nil, gep, top);

    Value* added = BinaryOperator::CreateAdd(cur, one, "added", top);
    new StoreInst(added, counter, top);

    Value* cmp = new ICmpInst(ICmpInst::ICMP_EQ, added, max, "loop_check", top);
    BranchInst::Create(cont, top, cmp, top);

    block = cont;
  }

  void LLVMWorkHorse::setup_scope(VMMethod* vmm) {
    Value* heap_null = ConstantExpr::getNullValue(PointerType::getUnqual(vars_type));
    Value* heap_pos = get_field(block, vars, offset::vars_on_heap);
    new StoreInst(heap_null, heap_pos, false, block);

    Value* self = new LoadInst(get_field(block, args, offset::args_recv),
        "args.recv", block);
    new StoreInst(self, get_field(block, vars, offset::vars_self), false, block);
    Value* mod = new LoadInst(get_field(block, msg, offset::msg_module),
        "msg.module", block);
    new StoreInst(mod, get_field(block, vars, offset::vars_module), false, block);

    Value* blk = new LoadInst(get_field(block, args, offset::args_block),
        "args.block", block);
    new StoreInst(blk, get_field(block, vars, offset::vars_block), false, block);

    new StoreInst(Constant::getNullValue(ls_->ptr_type("VariableScope")),
        get_field(block, vars, offset::vars_parent), false, block);

    nil_locals(vmm);
  }

  void LLVMWorkHorse::setup_inline_scope(Value* self, Value* mod, VMMethod* vmm) {
    Value* heap_null = ConstantExpr::getNullValue(PointerType::getUnqual(vars_type));
    Value* heap_pos = get_field(block, vars, offset::vars_on_heap);
    new StoreInst(heap_null, heap_pos, false, block);

    new StoreInst(self, get_field(block, vars, offset::vars_self), false, block);
    new StoreInst(mod, get_field(block, vars, offset::vars_module), false, block);

    Value* blk = constant(Qnil, obj_type, block);
    new StoreInst(blk, get_field(block, vars, offset::vars_block), false, block);

    new StoreInst(Constant::getNullValue(ls_->ptr_type("VariableScope")),
        get_field(block, vars, offset::vars_parent), false, block);

    nil_locals(vmm);
  }

  void LLVMWorkHorse::setup_block_scope(VMMethod* vmm) {
    new StoreInst(ConstantExpr::getNullValue(PointerType::getUnqual(vars_type)),
        get_field(block, vars, offset::vars_on_heap), false, block);
    Value* self = new LoadInst(
        get_field(block, block_inv, offset::blockinv_self),
        "invocation.self", block);

    new StoreInst(self, get_field(block, vars, offset::vars_self), false, block);

    Value* mod = new LoadInst(get_field(block, top_scope, offset::varscope_module),
        "top_scope.module", block);
    new StoreInst(mod, get_field(block, vars, offset::vars_module), false, block);

    Value* blk = new LoadInst(get_field(block, top_scope, offset::varscope_block),
        "args.block", block);
    new StoreInst(blk, get_field(block, vars, offset::vars_block), false, block);


    // We don't use top_scope here because of nested blocks. Parent MUST be
    // the scope the block was created in, not the top scope for depth
    // variables to work.
    Value* be_scope = new LoadInst(
        get_field(block, block_env, offset::blockenv_scope),
        "env.scope", block);

    new StoreInst(be_scope,
        get_field(block, vars, offset::vars_parent), false, block);

    nil_locals(vmm);
  }

  void LLVMWorkHorse::import_args(VMMethod* vmm) {
    Value* vm_obj = vm;
    Value* dis_obj = msg;
    Value* arg_obj = args;

    // Check the argument count
    Value* total_idx[] = {
      ConstantInt::get(Type::Int32Ty, 0),
      ConstantInt::get(Type::Int32Ty, offset::args_total),
    };

    Value* total_offset = GetElementPtrInst::Create(arg_obj, total_idx,
        total_idx + 2, "total_pos", block);
    Value* total = new LoadInst(total_offset, "arg.total", block);

    BasicBlock* arg_error = BasicBlock::Create("arg_error", func);
    BasicBlock* cont = BasicBlock::Create("import_args", func);

    // Check arguments
    //
    // if there is a splat..
    if(vmm->splat_position >= 0) {
      if(vmm->required_args > 0) {
        // Make sure we got at least the required args
        Value* cmp = new ICmpInst(ICmpInst::ICMP_SLT, total,
            ConstantInt::get(Type::Int32Ty, vmm->required_args), "arg_cmp", block);
        BranchInst::Create(arg_error, cont, cmp, block);
      } else {
        // Only splat or optionals, no handling!
        BranchInst::Create(cont, block);
      }

      // No splat, a precise number of args
    } else if(vmm->required_args == vmm->total_args) {
      // Make sure we got the exact number of arguments
      Value* cmp = new ICmpInst(ICmpInst::ICMP_NE, total,
          ConstantInt::get(Type::Int32Ty, vmm->required_args), "arg_cmp", block);
      BranchInst::Create(arg_error, cont, cmp, block);

      // No splat, with optionals
    } else {
      Value* c1 = new ICmpInst(ICmpInst::ICMP_SLT, total,
          ConstantInt::get(Type::Int32Ty, vmm->required_args), "arg_cmp", block);
      Value* c2 = new ICmpInst(ICmpInst::ICMP_SGT, total,
          ConstantInt::get(Type::Int32Ty, vmm->total_args), "arg_cmp", block);

      Value* cmp = BinaryOperator::CreateOr(c1, c2, "arg_combine", block);
      BranchInst::Create(arg_error, cont, cmp, block);
    }

    block = arg_error;

    // Call our arg_error helper
    Signature sig(ls_, "Object");

    sig << "VM";
    sig << "CallFrame";
    sig << "Dispatch";
    sig << "Arguments";
    sig << Type::Int32Ty;

    Value* call_args[] = {
      vm_obj,
      prev,
      dis_obj,
      arg_obj,
      ConstantInt::get(Type::Int32Ty, vmm->required_args)
    };

    Value* val = sig.call("rbx_arg_error", call_args, 5, "ret", block);
    return_value(val);

    // Switch to using continuation
    block = cont;

    setup_scope(vmm);

    // Import the arguments
    Value* idx1[] = {
      ConstantInt::get(Type::Int32Ty, 0),
      ConstantInt::get(Type::Int32Ty, offset::args_ary),
    };

    Value* offset = GetElementPtrInst::Create(arg_obj, idx1, idx1+2, "arg_ary_pos", block);

    Value* arg_ary = new LoadInst(offset, "arg_ary", block);

    // If there are a precise number of args, easy.
    if(vmm->required_args == vmm->total_args) {
      for(int i = 0; i < vmm->required_args; i++) {
        Value* int_pos = ConstantInt::get(Type::Int32Ty, i);

        Value* arg_val_offset =
          GetElementPtrInst::Create(arg_ary, int_pos, "arg_val_offset", block);

        Value* arg_val = new LoadInst(arg_val_offset, "arg_val", block);

        Value* idx2[] = {
          ConstantInt::get(Type::Int32Ty, 0),
          ConstantInt::get(Type::Int32Ty, offset::vars_tuple),
          int_pos
        };

        Value* pos = GetElementPtrInst::Create(vars, idx2, idx2+3, "var_pos", block);

        new StoreInst(arg_val, pos, false, block);
      }

      // Otherwise, we must loop in the generate code because we don't know
      // how many they've actually passed in.
    } else {
      Value* loop_i = new AllocaInst(Type::Int32Ty, 0, "loop_i", block);

      BasicBlock* top = BasicBlock::Create("arg_loop_top", func);
      BasicBlock* body = BasicBlock::Create("arg_loop_body", func);
      BasicBlock* after = BasicBlock::Create("arg_loop_cont", func);

      new StoreInst(ConstantInt::get(Type::Int32Ty, 0), loop_i, false, block);
      BranchInst::Create(top, block);

      // now at the top of block, check if we should continue...
      Value* loop_val = new LoadInst(loop_i, "loop_val", top);
      Value* cmp = new ICmpInst(ICmpInst::ICMP_SLT, loop_val, total,
          "loop_test", top);

      BranchInst::Create(body, after, cmp, top);

      // Now, the body

      Value* arg_val_offset =
        GetElementPtrInst::Create(arg_ary, loop_val, "arg_val_offset", body);

      Value* arg_val = new LoadInst(arg_val_offset, "arg_val", body);

      Value* idx2[] = {
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, offset::vars_tuple),
        loop_val
      };

      Value* pos = GetElementPtrInst::Create(vars, idx2, idx2+3, "var_pos", body);

      new StoreInst(arg_val, pos, false, body);

      Value* plus_one = BinaryOperator::CreateAdd(loop_val,
          ConstantInt::get(Type::Int32Ty, 1), "add", body);
      new StoreInst(plus_one, loop_i, false, body);

      BranchInst::Create(top, body);

      block = after;
    }

    // Setup the splat.
    if(vmm->splat_position >= 0) {
      Signature sig(ls_, "Object");
      sig << "VM";
      sig << "Arguments";
      sig << Type::Int32Ty;

      Value* call_args[] = {
        vm_obj,
        arg_obj,
        ConstantInt::get(Type::Int32Ty, vmm->total_args)
      };

      Function* func = sig.function("rbx_construct_splat");
      func->setOnlyReadsMemory(true);
      func->setDoesNotThrow(true);

      CallInst* splat_val = sig.call("rbx_construct_splat", call_args, 3, "splat_val", block);

      splat_val->setOnlyReadsMemory(true);
      splat_val->setDoesNotThrow(true);

      Value* idx3[] = {
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, offset::vars_tuple),
        ConstantInt::get(Type::Int32Ty, vmm->splat_position)
      };

      Value* pos = GetElementPtrInst::Create(vars, idx3, idx3+3, "splat_pos", block);
      new StoreInst(splat_val, pos, false, block);
    }
  }

  void LLVMWorkHorse::setup_block(VMMethod* vmm) {
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

    block = BasicBlock::Create("entry", func);

    Value* cfstk = new AllocaInst(obj_type,
        ConstantInt::get(Type::Int32Ty,
          (sizeof(CallFrame) / sizeof(Object*)) + vmm->stack_size),
        "cfstk", block);

    call_frame = CastInst::Create(
        Instruction::BitCast,
        cfstk,
        PointerType::getUnqual(cf_type), "call_frame", block);

    Value* cfstk_idx[] = {
      ConstantInt::get(Type::Int32Ty, sizeof(CallFrame) / sizeof(Object*))
    };

    stk = GetElementPtrInst::Create(cfstk, cfstk_idx, cfstk_idx+1, "stack", block);

    Value* var_mem = new AllocaInst(obj_type,
        ConstantInt::get(Type::Int32Ty,
          (sizeof(StackVariables) / sizeof(Object*)) + vmm->number_of_locals),
        "var_mem", block);

    vars = CastInst::Create(
        Instruction::BitCast,
        var_mem,
        PointerType::getUnqual(stack_vars_type), "vars", block);

    initialize_block_frame(vmm->stack_size);

    stack_top = new AllocaInst(obj_ary_type, NULL, "stack_top", block);

    Value* stk_idx[] = {
      ConstantInt::get(Type::Int32Ty, (uint64_t)-1),
    };

    Value* stk_back_one = GetElementPtrInst::Create(stk, stk_idx,
        stk_idx+1, "stk_back_one", block);
    new StoreInst(stk_back_one, stack_top, false, block);

    nil_stack(vmm->stack_size, constant(Qnil, obj_type, block));

    setup_block_scope(vmm);

    BasicBlock* body = BasicBlock::Create("block_body", func);
    BranchInst::Create(body, block);
    block = body;
  }

  void LLVMWorkHorse::setup(VMMethod* vmm) {
    Signature sig(ls_, "Object");
    sig << "VM";
    sig << "CallFrame";
    sig << "Dispatch";
    sig << "Arguments";

    func = sig.function("");

    Function::arg_iterator ai = func->arg_begin();
    vm =   ai++; vm->setName("state");
    prev = ai++; prev->setName("previous");
    msg =  ai++; msg->setName("msg");
    args = ai++; args->setName("args");

    block = BasicBlock::Create("entry", func);

    Value* cfstk = new AllocaInst(obj_type,
        ConstantInt::get(Type::Int32Ty,
          (sizeof(CallFrame) / sizeof(Object*)) + vmm->stack_size),
        "cfstk", block);

    call_frame = CastInst::Create(
        Instruction::BitCast,
        cfstk,
        PointerType::getUnqual(cf_type), "call_frame", block);

    Value* cfstk_idx[] = {
      ConstantInt::get(Type::Int32Ty, sizeof(CallFrame) / sizeof(Object*))
    };

    stk = GetElementPtrInst::Create(cfstk, cfstk_idx, cfstk_idx+1, "stack", block);

    Value* var_mem = new AllocaInst(obj_type,
        ConstantInt::get(Type::Int32Ty,
          (sizeof(StackVariables) / sizeof(Object*)) + vmm->number_of_locals),
        "var_mem", block);

    vars = CastInst::Create(
        Instruction::BitCast,
        var_mem,
        PointerType::getUnqual(stack_vars_type), "vars", block);

    initialize_call_frame(vmm->stack_size);

    stack_top = new AllocaInst(obj_ary_type, NULL, "stack_top", block);

    Value* stk_idx[] = {
      ConstantInt::get(Type::Int32Ty, (uint64_t)-1),
    };

    Value* stk_back_one = GetElementPtrInst::Create(stk, stk_idx,
        stk_idx+1, "stk_back_one", block);
    new StoreInst(stk_back_one, stack_top, false, block);

    nil_stack(vmm->stack_size, constant(Qnil, obj_type, block));

    import_args(vmm);

    BasicBlock* body = BasicBlock::Create("method_body", func);
    BranchInst::Create(body, block);
    block = body;
  }

  BasicBlock* LLVMWorkHorse::setup_inline(VMMethod* vmm, Function* current,
      Value* vm_i, Value* previous,
      Value* self, Value* mod, std::vector<Value*>& stack_args)
  {
    func = current;
    vm = vm_i;
    prev = previous;
    args = ConstantExpr::getNullValue(ls_->ptr_type("Arguments"));

    BasicBlock* entry = BasicBlock::Create("inline_entry", func);
    block = entry;

    BasicBlock* alloca_block = &current->getEntryBlock();

    Value* cfstk = new AllocaInst(obj_type,
        ConstantInt::get(Type::Int32Ty,
          (sizeof(CallFrame) / sizeof(Object*)) + vmm->stack_size),
        "cfstk", alloca_block->getTerminator());

    call_frame = CastInst::Create(
        Instruction::BitCast,
        cfstk,
        PointerType::getUnqual(cf_type), "call_frame", block);

    Value* cfstk_idx[] = {
      ConstantInt::get(Type::Int32Ty, sizeof(CallFrame) / sizeof(Object*))
    };

    stk = GetElementPtrInst::Create(cfstk, cfstk_idx, cfstk_idx+1, "stack", block);

    Value* var_mem = new AllocaInst(obj_type,
        ConstantInt::get(Type::Int32Ty,
          (sizeof(StackVariables) / sizeof(Object*)) + vmm->number_of_locals),
        "var_mem", alloca_block->getTerminator());

    vars = CastInst::Create(
        Instruction::BitCast,
        var_mem,
        PointerType::getUnqual(stack_vars_type), "vars", block);

    //  Setup the CallFrame
    //
    // previous
    new StoreInst(prev, get_field(block, call_frame, offset::cf_previous),
        false, block);

    // msg
    new StoreInst(ConstantExpr::getNullValue(ls_->ptr_type("Dispatch")),
        get_field(block, call_frame, offset::cf_msg),
        false, block);

    // cm
    Value* obj_addr = constant(vmm->original.object_address(),
        PointerType::getUnqual(ls_->ptr_type("CompiledMethod")), block);

    method = new LoadInst(obj_addr, "cm", block);
    Value* cm_gep = get_field(block, call_frame, offset::cf_cm);
    new StoreInst(method, cm_gep, false, block);

    // flags
    new StoreInst(ConstantInt::get(Type::Int32Ty, CallFrame::cInlineFrame),
        get_field(block, call_frame, offset::cf_flags), false, block);

    // ip
    new StoreInst(ConstantInt::get(Type::Int32Ty, 0),
        get_field(block, call_frame, offset::cf_ip), false, block);

    // scope
    new StoreInst(vars, get_field(block, call_frame, offset::cf_scope),
        false, block);


    stack_top = new AllocaInst(obj_ary_type, NULL, "stack_top", block);

    Value* stk_idx[] = {
      ConstantInt::get(Type::Int32Ty, (uint64_t)-1),
    };

    Value* stk_back_one = GetElementPtrInst::Create(stk, stk_idx,
        stk_idx+1, "stk_back_one", block);
    new StoreInst(stk_back_one, stack_top, false, block);

    nil_stack(vmm->stack_size, constant(Qnil, obj_type, block));

    setup_inline_scope(self, mod, vmm);

    // We know the right arguments are present, so we just need to put them
    // in the right place.
    //
    // We don't support splat in an inlined method!
    assert(vmm->splat_position < 0);

    assert(stack_args.size() <= (size_t)vmm->total_args);

    for(size_t i = 0; i < stack_args.size(); i++) {
      Value* int_pos = ConstantInt::get(Type::Int32Ty, i);

      Value* idx2[] = {
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, offset::vars_tuple),
        int_pos
      };

      Value* pos = GetElementPtrInst::Create(vars, idx2, idx2+3, "local_pos", block);

      new StoreInst(stack_args[i], pos, false, block);
    }

    BasicBlock* body = BasicBlock::Create("method_body", func);
    BranchInst::Create(body, block);
    block = body;

    return entry;
  }

  class BlockFinder : public VisitInstructions<BlockFinder> {
    BlockMap& map_;
    Function* function_;
    opcode current_ip_;
    int force_break_;
    bool creates_blocks_;
    int number_of_sends_;
    bool loops_;
    int sp_;

  public:

    BlockFinder(BlockMap& map, Function* func)
      : map_(map)
      , function_(func)
      , current_ip_(0)
      , force_break_(false)
      , creates_blocks_(false)
      , number_of_sends_(0)
      , loops_(false)
      , sp_(-1)
    {}

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

#include "gen/inst_stack.hpp"

    void before(opcode op, opcode arg1=0, opcode arg2=0) {
      BlockMap::iterator i = map_.find(current_ip_);
      if(i != map_.end()) {
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
      } else {
        if(force_break_) {
          if(cDebugStack) {
            std::cout << current_ip_ << ": unknown\n";
          }
          break_at(current_ip_, true);
          force_break_ = false;
          sp_ = cUnknown;
          return;
        }

        if(cDebugStack) {
          std::cout << current_ip_ << ": " << sp_ << "\n";
        }
      }

      force_break_ = false;
      if(sp_ != cUnknown) {
        sp_ += stack_difference(op, arg1, arg2);
        assert(sp_ >= -1);
      }
    }

    void break_at(int ip, bool unreachable=false) {
      BlockMap::iterator i = map_.find(ip);
      if(i == map_.end()) {
        std::ostringstream ss;
        ss << "ip" << ip;
        JITBasicBlock& jbb = map_[ip];
        jbb.block = BasicBlock::Create(ss.str().c_str(), function_);
        if(unreachable) {
          jbb.sp = cUnknown;
        } else {
          jbb.sp = sp_;
        }

        if(cDebugStack) {
          std::cout << "patch " << ip << ": " << jbb.sp << "\n";
        }
      } else {
        if(!unreachable) {
          if(i->second.sp == cUnknown) {
            i->second.sp = sp_;
          } else if(sp_ != cUnknown) {
            assert(i->second.sp == sp_);
          }
        }
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
    }

    void visit_goto_if_false(opcode which) {
      if(current_ip_ < which) loops_ = true;

      break_at(which);
    }

    void visit_goto_if_defined(opcode which) {
      if(current_ip_ < which) loops_ = true;

      break_at(which);
    }

    void visit_setup_unwind(opcode which, opcode type) {
      if(current_ip_ < which) loops_ = true;

      break_at(which);
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

    void visit_send_stack(opcode which, opcode args) {
      number_of_sends_++;
    }

    void visit_send_method(opcode which) {
      number_of_sends_++;
    }

    void visit_send_stack_with_block(opcode which, opcode args) {
      number_of_sends_++;
    }

    void visit_send_stack_with_splat(opcode which, opcode args) {
      number_of_sends_++;
    }

    void visit_send_super_stack_with_block(opcode which, opcode args) {
      number_of_sends_++;
    }

    void visit_send_super_stack_with_splat(opcode which, opcode args) {
      number_of_sends_++;
    }
  };

  bool LLVMWorkHorse::generate_body(JITMethodInfo& info) {
    JITVisit visitor(ls_, info, ls_->module(), func,
        block, stk, call_frame,
        method_entry_, args,
        vars, info.is_block, info.inline_return);

    if(info.inline_policy) {
      visitor.set_policy(info.inline_policy);
    } else {
      visitor.init_policy();
    }

    visitor.set_called_args(info.called_args);

    // Pass 1, detect BasicBlock boundaries
    BlockFinder finder(visitor.block_map(), func);
    finder.drive(info.vmm);

    // DISABLED: This still has problems.
    // visitor.set_creates_blocks(finder.creates_blocks());

    if(!info.inline_return &&
          (finder.number_of_sends() > 0 || finder.loops_p())) {
      // Check for interrupts at the top
      visitor.visit_check_interrupts();
    }

    // Pass 2, compile!
    try {
      visitor.drive(info.vmm);
    } catch(JITVisit::Unsupported &e) {
      return false;
    }

    info.return_value = visitor.return_value();
    info.fin_block = visitor.current_block();
    return true;
  }
}

#endif
