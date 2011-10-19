#ifdef ENABLE_LLVM

#include "llvm/jit_method.hpp"
#include "llvm/jit_context.hpp"

#include "llvm/method_info.hpp"

#include "call_frame.hpp"
#include "vmmethod.hpp"
#include "instruments/tooling.hpp"

#include "builtin/staticscope.hpp"
#include "builtin/module.hpp"

using namespace llvm;

namespace rubinius {

namespace jit {

  void MethodBuilder::setup() {
    std::vector<const Type*> ftypes;
    ftypes.push_back(ls_->ptr_type("VM"));
    ftypes.push_back(ls_->ptr_type("CallFrame"));
    ftypes.push_back(ls_->ptr_type("Executable"));
    ftypes.push_back(ls_->ptr_type("Module"));
    ftypes.push_back(ls_->ptr_type("Arguments"));

    FunctionType* ft = FunctionType::get(ls_->ptr_type("Object"), ftypes, false);

    std::ostringstream ss;
    ss << std::string("_X_")
       << ls_->enclosure_name(info_.method())
       << "#"
       << ls_->symbol_debug_str(info_.method()->name())
       << "@" << ls_->add_jitted_method();

    llvm::Function* func = Function::Create(ft, GlobalValue::ExternalLinkage,
                            ss.str().c_str(), ls_->module());

    Function::arg_iterator ai = func->arg_begin();
    llvm::Value* vm =   ai++; vm->setName("state");
    llvm::Value* prev = ai++; prev->setName("previous");
    exec = ai++; exec->setName("exec");
    module = ai++; module->setName("mod");
    llvm::Value* args = ai++; args->setName("args");

    BasicBlock* block = BasicBlock::Create(ls_->ctx(), "entry", func);
    builder_.SetInsertPoint(block);

    info_.context().set_function(func);

    info_.set_vm(vm);
    info_.set_args(args);
    info_.set_previous(prev);
    info_.set_entry(block);

    alloc_frame("method_body");

    check_arity();

    // check_self_type();

    initialize_frame(vmm_->stack_size);

    nil_stack(vmm_->stack_size, constant(Qnil, obj_type));

    import_args();

    import_args_ = b().GetInsertBlock();

    b().CreateBr(body_);
    b().SetInsertPoint(body_);
  }

  void MethodBuilder::import_args_19_style() {
    Value* local_i = counter2_;

    Value* loop_i = info_.counter();
    Value* arg_ary = b().CreateLoad(
                       b().CreateConstGEP2_32(info_.args(), 0, offset::args_ary,
                                              "arg_ary_pos"),
                       "arg_ary");

    // The variables used in the 4 phases.
    int P = vmm_->post_args;
    int M = vmm_->required_args - P;
    Value* T = arg_total;
    int DT = vmm_->total_args;
    int R = vmm_->required_args;
    int O = vmm_->total_args - R;
    int HS = vmm_->splat_position > 0 ? 1 : 0;


    // Phase 1, the manditories
    // 0 ... M

    for(int i = 0; i < M; i++) {
      Value* int_pos = cint(i);

      Value* arg_val_offset = b().CreateConstGEP1_32(arg_ary, i, "arg_val_offset");

      Value* arg_val = b().CreateLoad(arg_val_offset, "arg_val");

      Value* idx2[] = {
        cint(0),
        cint(offset::vars_tuple),
        int_pos
      };

      Value* pos = b().CreateGEP(vars, idx2, idx2+3, "var_pos");

      b().CreateStore(arg_val, pos);
    }

    // Phase 2, the post args
    // T-P ... T
    {
      BasicBlock* top = info_.new_block("arg_loop_top");
      BasicBlock* body = info_.new_block("arg_loop_body");
      BasicBlock* after = info_.new_block("arg_loop_cont");

      // *loop_i = T-P
      b().CreateStore(
        b().CreateSub(T, ConstantInt::get(T->getType(), P)),
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
          b().CreateICmpSLT(loop_val, T, "loop_test"),
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
    // M ... M + min(O, T-R)

    Value* Oval = cint(O);
    Value* left = b().CreateSub(T, ConstantInt::get(ls_->Int32Ty, R));

    Value* limit =
      b().CreateAdd(
          cint(M),
          b().CreateSelect(
            b().CreateICmpSLT(Oval, left),
            Oval, left));

    {
      BasicBlock* top = info_.new_block("opt_arg_loop_top");
      BasicBlock* body = info_.new_block("opt_arg_loop_body");
      BasicBlock* after = info_.new_block("opt_arg_loop_cont");

      // *loop_i = M
      b().CreateStore(cint(M), loop_i);
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

  void MethodBuilder::import_args() {
    setup_scope();

    if(vmm_->post_args > 0) {
      import_args_19_style();
      return;
    }

    // Import the arguments
    Value* offset = b().CreateConstGEP2_32(info_.args(), 0, offset::args_ary, "arg_ary_pos");

    Value* arg_ary = b().CreateLoad(offset, "arg_ary");

    // If there are a precise number of args, easy.
    if(vmm_->required_args == vmm_->total_args) {
      for(int i = 0; i < vmm_->required_args; i++) {
        Value* int_pos = cint(i);

        Value* arg_val_offset = b().CreateConstGEP1_32(arg_ary, i, "arg_val_offset");

        Value* arg_val = b().CreateLoad(arg_val_offset, "arg_val");

        Value* idx2[] = {
          cint(0),
          cint(offset::vars_tuple),
          int_pos
        };

        Value* pos = b().CreateGEP(vars, idx2, idx2+3, "var_pos");

        b().CreateStore(arg_val, pos);
      }

      // Otherwise, we must loop in the generate code because we don't know
      // how many they've actually passed in.
    } else {
      Value* loop_i = info_.counter();

      BasicBlock* top = info_.new_block("arg_loop_top");
      BasicBlock* body = info_.new_block("arg_loop_body");
      BasicBlock* after = info_.new_block("arg_loop_cont");

      Value* limit;

      // Because of a splat, there can be more args given than
      // vmm->total_args, so we need to use vmm->total_args as a max.
      if(vmm_->splat_position >= 0) {
        Value* static_total = cint(vmm_->total_args);

        limit = b().CreateSelect(
            b().CreateICmpSLT(static_total, arg_total),
            static_total,
            arg_total);
      } else {
        // Because of arity checks, arg_total is less than or equal
        // to vmm->total_args.
        limit = arg_total;
      }

      b().CreateStore(cint(0), loop_i);
      b().CreateBr(top);

      b().SetInsertPoint(top);

      // now at the top of block, check if we should continue...
      Value* loop_val = b().CreateLoad(loop_i, "loop_val");
      Value* cmp = b().CreateICmpSLT(loop_val, limit, "loop_test");

      b().CreateCondBr(cmp, body, after);

      // Now, the body
      b().SetInsertPoint(body);

      Value* arg_val_offset =
        b().CreateGEP(arg_ary, loop_val, "arg_val_offset");

      Value* arg_val = b().CreateLoad(arg_val_offset, "arg_val");

      Value* idx2[] = {
        cint(0),
        cint(offset::vars_tuple),
        loop_val
      };

      Value* pos = b().CreateGEP(vars, idx2, idx2+3, "var_pos");

      b().CreateStore(arg_val, pos);

      Value* plus_one = b().CreateAdd(loop_val,
          cint(1), "add");
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
      sig << ls_->Int32Ty;

      Value* call_args[] = {
        info_.vm(),
        info_.args(),
        cint(vmm_->total_args),
        cint(vmm_->total_args)
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

  void MethodBuilder::check_arity() {
    Value* total_offset = b().CreateConstGEP2_32(info_.args(), 0,
        offset::args_total, "total_pos");
    Value* total = b().CreateLoad(total_offset, "arg.total");

    // For others to use.
    arg_total = total;

    BasicBlock* arg_error = info_.new_block("arg_error");
    BasicBlock* cont = info_.new_block("import_args");

    // Check arguments
    //
    // if there is a splat..
    if(vmm_->splat_position >= 0) {
      if(vmm_->required_args > 0) {
        // Make sure we got at least the required args
        Value* cmp = b().CreateICmpSLT(total,
            cint(vmm_->required_args), "arg_cmp");
        b().CreateCondBr(cmp, arg_error, cont);
      } else {
        // Only splat or optionals, no handling!
        b().CreateBr(cont);
      }

      // No splat, a precise number of args
    } else if(vmm_->required_args == vmm_->total_args) {
      // Make sure we got the exact number of arguments
      Value* cmp = b().CreateICmpNE(total,
          cint(vmm_->required_args), "arg_cmp");
      b().CreateCondBr(cmp, arg_error, cont);

      // No splat, with optionals
    } else {
      Value* c1 = b().CreateICmpSLT(total,
          cint(vmm_->required_args), "arg_cmp");
      Value* c2 = b().CreateICmpSGT(total,
          cint(vmm_->total_args), "arg_cmp");

      Value* cmp = b().CreateOr(c1, c2, "arg_combine");
      b().CreateCondBr(cmp, arg_error, cont);
    }

    b().SetInsertPoint(arg_error);

    // Call our arg_error helper
    Signature sig(ls_, "Object");

    sig << "VM";
    sig << "CallFrame";
    sig << "Arguments";
    sig << ls_->Int32Ty;

    Value* call_args[] = {
      info_.vm(),
      info_.previous(),
      info_.args(),
      cint(vmm_->total_args)
    };

    Value* val = sig.call("rbx_arg_error", call_args, 4, "ret", b());
    return_value(val);

    // Switch to using continuation
    b().SetInsertPoint(cont);
  }

  void MethodBuilder::return_value(Value* ret, BasicBlock* cont) {
    if(ls_->include_profiling()) {
      Value* test = b().CreateLoad(ls_->profiling(), "profiling");
      BasicBlock* end_profiling = info_.new_block("end_profiling");
      if(!cont) {
        cont = info_.new_block("continue");
      }

      b().CreateCondBr(test, end_profiling, cont);

      b().SetInsertPoint(end_profiling);

      Signature sig(ls_, ls_->VoidTy);
      sig << llvm::PointerType::getUnqual(ls_->Int8Ty);

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
    Value* heap_pos = get_field(vars, offset::vars_on_heap);

    b().CreateStore(heap_null, heap_pos);

    Value* self = b().CreateLoad(get_field(args, offset::args_recv),
        "args.recv");
    b().CreateStore(self, get_field(vars, offset::vars_self));
    Value* mod = module;
    b().CreateStore(mod, get_field(vars, offset::vars_module));

    Value* blk = b().CreateLoad(get_field(args, offset::args_block),
        "args.block");
    b().CreateStore(blk, get_field(vars, offset::vars_block));

    b().CreateStore(Constant::getNullValue(ls_->ptr_type("VariableScope")),
        get_field(vars, offset::vars_parent));

    b().CreateStore(constant(Qnil, obj_type), get_field(vars, offset::vars_last_match));

    nil_locals();
  }



  void MethodBuilder::initialize_frame(int stack_size) {
    Value* cm_gep = get_field(call_frame, offset::CallFrame::cm);
    method = b().CreateBitCast(
        exec, cast<llvm::PointerType>(cm_gep->getType())->getElementType(), "cm");

    // previous
    b().CreateStore(info_.previous(), get_field(call_frame, offset::CallFrame::previous));

    // arguments
    b().CreateStore(info_.args(), get_field(call_frame, offset::CallFrame::arguments));

    // msg
    b().CreateStore(
        ConstantInt::getNullValue(ls_->Int8PtrTy),
        get_field(call_frame, offset::CallFrame::dispatch_data));

    // cm
    b().CreateStore(method, cm_gep);

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
        constant(info_.context().runtime_data_holder(), ls_->Int8PtrTy),
        get_field(call_frame, offset::CallFrame::jit_data));

    if(ls_->include_profiling()) {
      Value* test = b().CreateLoad(ls_->profiling(), "profiling");

      BasicBlock* setup_profiling = info_.new_block("setup_profiling");
      BasicBlock* cont = info_.new_block("continue");

      b().CreateCondBr(test, setup_profiling, cont);

      b().SetInsertPoint(setup_profiling);

      Signature sig(ls_, ls_->VoidTy);
      sig << "VM";
      sig << llvm::PointerType::getUnqual(ls_->Int8Ty);
      sig << "Executable";
      sig << "Module";
      sig << "Arguments";
      sig << "CompiledMethod";

      Value* call_args[] = {
        info_.vm(),
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
