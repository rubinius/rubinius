#ifdef ENABLE_LLVM

#include "llvm/inline.hpp"
#include "llvm/jit_inline_method.hpp"
#include "llvm/jit_inline_block.hpp"
#include "llvm/jit_runtime.hpp"
#include "llvm/jit_context.hpp"

#include "llvm/stack_args.hpp"

#include "builtin/alias.hpp"
#include "builtin/methodtable.hpp"
#include "builtin/nativefunction.hpp"
#include "builtin/lookuptable.hpp"

#include "ffi_util.hpp"

namespace rubinius {

  void Inliner::check_class(llvm::Value* recv, Class* klass, llvm::BasicBlock* bb) {
    if(!bb) bb = failure();

    guarded_type_ = ops_.check_class(recv, klass, bb);
    guarded_type_.inherit_source(ops_.state(), recv);
  }

  void Inliner::check_recv(Class* klass, llvm::BasicBlock* bb) {
    check_class(recv(), klass, bb);
  }

  bool Inliner::consider_mono() {
    if(cache_->classes_seen() != 1) return false;
    Class* klass = cache_->get_class(0);
    return inline_for_class(klass);
  }

  bool Inliner::consider_poly() {
    int classes_seen = cache_->classes_seen();
    BasicBlock* fail = failure();
    BasicBlock* fallback = ops_.new_block("poly_fallback");
    BasicBlock* merge = ops_.new_block("merge");
    BasicBlock* current = ops_.current_block();

    ops_.set_block(merge);
    PHINode* phi = ops_.b().CreatePHI(ops_.ObjType, classes_seen, "poly_result");

    ops_.set_block(current);

    for(int i = 0; i < classes_seen; ++i) {
      Class* klass = cache_->get_class(i);
      // Fallback to the next for failure
      set_failure(fallback);

      if(!inline_for_class(klass)) {
        // If we fail to inline this, emit a send to the method
        Value* cache_const = ops_.b().CreateIntToPtr(
          ConstantInt::get(ops_.state()->IntPtrTy, (reinterpret_cast<uintptr_t>(cache_))),
          ops_.ptr_type("InlineCache"), "cast_to_ptr");

        Value* execute_pos_idx[] = {
          ops_.state()->cint(0),
          ops_.state()->cint(offset::ic_execute),
        };

        Value* execute_pos = ops_.b().CreateGEP(cache_const,
            execute_pos_idx, "execute_pos");

        Value* execute = ops_.b().CreateLoad(execute_pos, "execute");
        ops_.setup_out_args(count_);

        Value* call_args[] = {
          ops_.vm(),
          cache_const,
          ops_.call_frame(),
          ops_.out_args()
        };

        Value* res = ops_.b().CreateCall(execute, call_args, "ic_send");
        ops_.check_for_exception(res);
        set_result(res);
      }

      ops_.b().CreateBr(merge);

      BasicBlock* branch = ops_.current_block();

      ops_.set_block(merge);

      phi->addIncoming(result(), branch);

      ops_.set_block(fallback);
      fallback = ops_.new_block("poly_fallback");
    }

    ops_.b().CreateBr(fallback);
    ops_.set_block(fallback);
    ops_.b().CreateBr(fail);

    ops_.set_block(merge);
    set_failure(fail);

    set_result(phi);

    return true;
  }

  bool Inliner::inline_for_class(Class* klass) {
    if(!klass) return false;

    Module* defined_in = 0;
    Executable* meth = klass->find_method(cache_->name, &defined_in);

    if(!meth) {
      if(ops_.state()->config().jit_inline_debug) {
        context_.inline_log("NOT inlining")
          << ops_.state()->symbol_debug_str(cache_->name)
          << ". Inliner error, method missing.\n";
      }
      return false;
    }

    if(Alias* alias = try_as<Alias>(meth)) {
      meth = alias->original_exec();
    }

    if(AccessVariable* acc = try_as<AccessVariable>(meth)) {
      if(acc->write()->true_p()) {
        inline_ivar_write(klass, acc);
      } else {
        inline_ivar_access(klass, acc);
      }
    } else if(CompiledCode* code = try_as<CompiledCode>(meth)) {
      MachineCode* mcode = code->machine_code();

      if(!code->primitive()->nil_p()) {
        if(!inline_primitive(klass, code, meth->execute)) return false;
        goto remember;
      }

      // Not yet sure why we'd hit a CompiledCode that hasn't been
      // internalized, but protect against that case none the less.
      if(!mcode) return false;

      if(detect_trivial_method(mcode, code)) {
        inline_trivial_method(klass, code);
      } else if(int which = detect_jit_intrinsic(klass, code)) {
        inline_intrinsic(klass, code, which);
      } else if(ops_.state()->config().jit_inline_generic) {
        InlineDecision decision;
        InlineOptions opts;

        InlinePolicy* policy = ops_.inline_policy();
        assert(policy);

        if(mcode->no_inline_p()) {
          decision = cInlineDisabled;
        } else {
          decision = policy->inline_p(mcode, opts);
        }

        if(decision != cInline) {
          if(ops_.state()->config().jit_inline_debug) {

            context_.inline_log("NOT inlining")
              << ops_.state()->enclosure_name(code)
              << "#"
              << ops_.state()->symbol_debug_str(code->name())
              << " into "
              << ops_.state()->symbol_debug_str(ops_.method_name())
              << ". ";

            switch(decision) {
            case cInlineDisabled:
              ops_.state()->log() << "inlining disabled by request";
              break;
            case cTooBig:
              ops_.state()->log() << policy->current_size() << " + "
                << mcode->total << " > "
                << policy->max_size();
              break;
            case cTooComplex:
              ops_.state()->log() << "too complex";
              if(!opts.allow_blocks) {
                ops_.state()->log() << " (block not allowed)";
              }
              break;
            default:
              ops_.state()->log() << "no policy";
            }
            if(mcode->jitted()) {
              ops_.state()->log() << " (jitted)\n";
            } else {
              ops_.state()->log() << " (interp)\n";
            }
          }
          return false;
        }

        if(ops_.state()->config().jit_inline_debug) {
          context_.inline_log("inlining")
            << ops_.state()->enclosure_name(code)
            << "#"
            << ops_.state()->symbol_debug_str(code->name())
            << " into "
            << ops_.state()->symbol_debug_str(ops_.method_name());

          ConstantScope* cs = code->scope();
          if(kind_of<ConstantScope>(cs) && klass != cs->module() && !klass->module_name()->nil_p()) {
            ops_.state()->log() << " ("
              << ops_.state()->symbol_debug_str(klass->module_name()) << ")";
          }

          if(inline_block_) {
            ops_.state()->log() << " (w/ inline block)";
          }

          ops_.state()->log() << "\n";
        }

        policy->increase_size(mcode);
        meth->add_inliner(ops_.state()->shared().om, ops_.root_method_info()->method());

        inline_generic_method(klass, defined_in, code, mcode);
        return true;
      } else {
        if(ops_.state()->config().jit_inline_debug) {
          context_.inline_log("NOT inlining")
            << ops_.state()->enclosure_name(code)
            << "#"
            << ops_.state()->symbol_debug_str(code->name())
            << " into "
            << ops_.state()->symbol_debug_str(ops_.method_name())
            << ". generic inlining disabled\n";
        }

        return false;
      }
    } else if(NativeFunction* nf = try_as<NativeFunction>(meth)) {
      if(inline_ffi(klass, nf)) {
        if(ops_.state()->config().jit_inline_debug) {
          context_.inline_log("inlining")
            << "FFI call to "
            << ops_.state()->symbol_debug_str(nf->name())
            << "() into "
            << ops_.state()->symbol_debug_str(ops_.method_name())
            << " (" << ops_.state()->symbol_debug_str(klass->module_name()) << ")\n";
        }
      } else {
        return false;
      }
    } else {
      if(ops_.state()->config().jit_inline_debug) {
        context_.inline_log("NOT inlining")
          << ops_.state()->symbol_debug_str(klass->module_name())
          << "#"
          << ops_.state()->symbol_debug_str(cache_->name)
          << " into "
          << ops_.state()->symbol_debug_str(ops_.method_name())
          << ". unhandled executable type\n";
      }
      return false;
    }

remember:
    meth->add_inliner(ops_.state()->shared().om, ops_.root_method_info()->method());

    return true;
  }

  void Inliner::inline_block(JITInlineBlock* ib, Value* self) {
    if(ops_.state()->config().jit_inline_debug) {
      context_.inline_log("inlining block into")
        << ops_.state()->symbol_debug_str(ops_.method_name())
        << "\n";
    }

    emit_inline_block(ib, self);
  }

  bool Inliner::detect_trivial_method(MachineCode* mcode, CompiledCode* code) {
    opcode* stream = mcode->opcodes;
    size_t size_max = 2;
    switch(stream[0]) {
    case InstructionSequence::insn_push_int:
      size_max++;
      break;
    case InstructionSequence::insn_push_literal:
      if(code && kind_of<Symbol>(code->literals()->at(stream[1]))) {
        size_max++;
      } else {
        return false;
      }
      break;
    case InstructionSequence::insn_meta_push_0:
    case InstructionSequence::insn_meta_push_1:
    case InstructionSequence::insn_meta_push_2:
    case InstructionSequence::insn_meta_push_neg_1:
    case InstructionSequence::insn_push_self:
      break;
    default:
      return false;
    }

    if(mcode->total == size_max &&
        count_ == 0 &&
        mcode->required_args == mcode->total_args &&
        mcode->total_args == 0) return true;
    return false;
  }

  void Inliner::inline_trivial_method(Class* klass, CompiledCode* code) {
    if(ops_.state()->config().jit_inline_debug) {
      context_.inline_log("inlining")
        << ops_.state()->enclosure_name(code)
        << "#"
        << ops_.state()->symbol_debug_str(code->name())
        << " into "
        << ops_.state()->symbol_debug_str(ops_.method_name())
        << " (" << ops_.state()->symbol_debug_str(klass->module_name()) << ") trivial\n";
    }

    MachineCode* mcode = code->machine_code();

    if(klass) check_recv(klass);

    Value* val = 0;
    /////

    opcode* stream = mcode->opcodes;
    switch(stream[0]) {
    case InstructionSequence::insn_push_int:
      val = ops_.constant(Fixnum::from(stream[1]));
      break;
    case InstructionSequence::insn_push_literal: {
      Symbol* sym = try_as<Symbol>(code->literals()->at(stream[1]));
      assert(sym);

      val = ops_.constant(sym);
      } break;
    case InstructionSequence::insn_meta_push_0:
      val = ops_.constant(Fixnum::from(0));
      break;
    case InstructionSequence::insn_meta_push_1:
      val = ops_.constant(Fixnum::from(1));
      break;
    case InstructionSequence::insn_meta_push_2:
      val = ops_.constant(Fixnum::from(2));
      break;
    case InstructionSequence::insn_meta_push_neg_1:
      val = ops_.constant(Fixnum::from(-1));
      break;
    case InstructionSequence::insn_push_self:
      val = recv();
      break;
    default:
      assert(0 && "Trivial detection is broken!");
    }

    /////

    assert(val);

    exception_safe();
    set_result(val);
  }

  void Inliner::inline_ivar_write(Class* klass, AccessVariable* acc) {
    if(count_ != 1) return;

    if(ops_.state()->config().jit_inline_debug) {
      context_.inline_log("inlining")
        << "writer to '"
        << ops_.state()->symbol_debug_str(acc->name())
        << "' on "
        << ops_.state()->symbol_debug_str(klass->module_name())
        << " in "
        << "#"
        << ops_.state()->symbol_debug_str(ops_.method_name())
        << "\n";
    }

    context_.enter_inline();
    ops_.state()->add_accessor_inlined();

    check_recv(klass);

    Value* val  = arg(0);

    Value* self = recv();

    // Figure out if we should use the table ivar lookup or
    // the slot ivar lookup.

    TypeInfo* ti = klass->type_info();
    TypeInfo::Slots::iterator it = ti->slots.find(acc->name()->index());

    if(it != ti->slots.end()) {
      int offset = ti->slot_locations[it->second];
      ops_.set_object_slot(self, offset, val);
    } else {
      Signature sig2(ops_.state(), "Object");
      sig2 << "State";
      sig2 << "CallFrame";
      sig2 << "Object";
      sig2 << "Object";
      sig2 << "Object";

      Value* call_args2[] = {
        ops_.vm(),
        ops_.call_frame(),
        self,
        ops_.constant(acc->name()),
        val
      };

      sig2.call("rbx_set_ivar", call_args2, 5, "ivar",
          ops_.b());
    }

    exception_safe();
    set_result(val);

    context_.leave_inline();
  }

  void Inliner::inline_ivar_access(Class* klass, AccessVariable* acc) {
    if(count_ != 0) return;

    if(ops_.state()->config().jit_inline_debug) {
      context_.inline_log("inlining")
        << "read to '"
        << ops_.state()->symbol_debug_str(acc->name())
        << "' on "
        << ops_.state()->symbol_debug_str(klass->module_name())
        << " in "
        << "#"
        << ops_.state()->symbol_debug_str(ops_.method_name());
    }

    context_.enter_inline();
    ops_.state()->add_accessor_inlined();

    Value* self = recv();

    ops_.check_reference_class(self, klass->class_id(), failure());

    // Figure out if we should use the table ivar lookup or
    // the slot ivar lookup.

    TypeInfo* ti = klass->type_info();
    TypeInfo::Slots::iterator it = ti->slots.find(acc->name()->index());

    Value* ivar = 0;

    if(it != ti->slots.end()) {
      int offset = ti->slot_locations[it->second];
      ivar = ops_.get_object_slot(self, offset);
      if(ops_.state()->config().jit_inline_debug) {
        ops_.state()->log() << " (slot: " << it->second << ")";
      }
    } else {
      LookupTable* pii = klass->packed_ivar_info();
      if(!pii->nil_p()) {
        bool found = false;

        Fixnum* which = try_as<Fixnum>(pii->fetch(0, acc->name(), &found));
        if(found) {
          int index = which->to_native();
          int offset = sizeof(Object) + (sizeof(Object*) * index);
          Value* slot_val = ops_.get_object_slot(self, offset);

          Value* cmp = ops_.b().CreateICmpEQ(slot_val, ops_.constant(cUndef), "prune_undef");

          ivar = ops_.b().CreateSelect(cmp,
                                       ops_.constant(cNil), slot_val,
                                       "select ivar");

          if(ops_.state()->config().jit_inline_debug) {
            ops_.state()->log() << " (packed index: " << index << ", " << offset << ")";
          }
        }
      }

      if(!ivar) {
        Signature sig2(ops_.state(), "Object");
        sig2 << "State";
        sig2 << "Object";
        sig2 << "Object";

        Value* call_args2[] = {
          ops_.vm(),
          self,
          ops_.constant(acc->name())
        };

        ivar = sig2.call("rbx_get_ivar", call_args2, 3, "ivar",
            ops_.b());
      }
    }

    exception_safe();
    set_result(ivar);

    if(ops_.state()->config().jit_inline_debug) {
      ops_.state()->log() << "\n";
    }

    context_.leave_inline();

  }

  void Inliner::prime_info(JITMethodInfo& info) {
    info.set_parent_info(ops_.info());

    info.inline_policy = ops_.inline_policy();
    info.called_args = count_;
    info.root = ops_.root_method_info();
    info.set_inline_block(inline_block_);
  }

  void Inliner::inline_generic_method(Class* klass, Module* defined_in,
                                      CompiledCode* code, MachineCode* mcode) {
    context_.enter_inline();

    check_recv(klass);

    JITMethodInfo info(context_, code, mcode);

    prime_info(info);

    info.self_type = guarded_type_;

    info.set_self_class(klass);

    jit::RuntimeData* rd = new jit::RuntimeData(code, cache_->name, defined_in);
    context_.add_runtime_data(rd);

    jit::InlineMethodBuilder work(ops_.state(), info, rd);
    work.valid_flag = ops_.valid_flag();

    Value* blk = 0;

    std::vector<Value*> args;
    if(block_on_stack_) {
      blk = ops_.stack_top();
      for(int i = count_; i >= 1; i--) {
        args.push_back(ops_.stack_back(i));
      }
    } else {
      blk = ops_.constant(cNil);
      for(int i = count_ - 1; i >= 0; i--) {
        args.push_back(ops_.stack_back(i));
      }
    }

    if(mcode->call_count >= 0) mcode->call_count /= 2;

    BasicBlock* entry = work.setup_inline(recv(), blk, args);

    if(!work.generate_body()) {
      rubinius::bug("LLVM failed to compile a function");
    }

    // Branch to the inlined method!
    ops_.create_branch(entry);

    // Set the basic block to continue with being the return_pad!
    ops_.set_block(info.return_pad());

    // Make the value available to the code that called inliner to
    // check and use.
    set_result(info.return_phi());

    context_.leave_inline();
  }

  void Inliner::emit_inline_block(JITInlineBlock* ib, Value* self) {
    context_.enter_inline();

    JITMethodInfo info(context_, ib->method(), ib->machine_code());

    prime_info(info);

    info.is_block = true;

    info.set_creator_info(creator_info_);
    info.set_block_info(block_info_);
    info.self_type = ops_.info().self_type;

    jit::RuntimeData* rd = new jit::RuntimeData(ib->method(), nil<Symbol>(), nil<Module>());
    context_.add_runtime_data(rd);

    jit::InlineBlockBuilder work(ops_.state(), info, rd);
    work.valid_flag = ops_.valid_flag();

    JITStackArgs args(count_);
    if(from_unboxed_array_) args.set_from_unboxed_array();

    for(int i = count_ - 1, j = 0; i >= 0; i--, j++) {
      args.put(j, ops_.stack_back(i));
    }

    info.stack_args = &args;

    if(ib->machine_code()->call_count >= 0) ib->machine_code()->call_count /= 2;

    BasicBlock* entry = work.setup_inline_block(self,
        ops_.constant(cNil, ops_.state()->ptr_type("Module")), args);

    if(!work.generate_body()) {
      rubinius::bug("LLVM failed to compile a function");
    }

    // Branch to the inlined block!
    ops_.create_branch(entry);

    // Set the basic block to continue with being the return_pad!
    ops_.set_block(info.return_pad());

    // Make the value available to the code that called inliner to
    // check and use.
    set_result(info.return_phi());

    context_.leave_inline();
  }

  Type* find_type(JITOperations& ops_, size_t type) {
    switch(type) {
      case RBX_FFI_TYPE_CHAR:
      case RBX_FFI_TYPE_UCHAR:
        return ops_.state()->Int8Ty;

      case RBX_FFI_TYPE_SHORT:
      case RBX_FFI_TYPE_USHORT:
        return ops_.state()->Int16Ty;

      case RBX_FFI_TYPE_INT:
      case RBX_FFI_TYPE_UINT:
        return ops_.state()->Int32Ty;

      case RBX_FFI_TYPE_LONG:
      case RBX_FFI_TYPE_ULONG:
#ifdef IS_X8664
        return ops_.state()->Int64Ty;
#else
        return ops_.state()->Int32Ty;
#endif

      case RBX_FFI_TYPE_LONG_LONG:
      case RBX_FFI_TYPE_ULONG_LONG:
        return ops_.state()->Int64Ty;

      case RBX_FFI_TYPE_FLOAT:
        return ops_.state()->FloatTy;

      case RBX_FFI_TYPE_DOUBLE:
        return ops_.state()->DoubleTy;

      case RBX_FFI_TYPE_OBJECT:
      case RBX_FFI_TYPE_STRING:
      case RBX_FFI_TYPE_STRPTR:
      case RBX_FFI_TYPE_PTR:
      case RBX_FFI_TYPE_VOID:
        return llvm::PointerType::getUnqual(ops_.state()->Int8Ty);
    }

    assert(0 && "unknown type to return!");

    return 0;
  }

  bool Inliner::inline_ffi(Class* klass, NativeFunction* nf) {

    for(size_t i = 0; i < nf->ffi_data->arg_count; i++) {
        if(nf->ffi_data->args_info[i].type==RBX_FFI_TYPE_ENUM || nf->ffi_data->args_info[i].type==RBX_FFI_TYPE_CALLBACK) {
            return false;
        }
    }
    if(nf->ffi_data->ret_info.type==RBX_FFI_TYPE_ENUM || nf->ffi_data->ret_info.type==RBX_FFI_TYPE_CALLBACK) {
        return false;
    }

    check_recv(klass);

    ///

    std::vector<Value*> ffi_args;
    std::vector<Type*> ffi_type;

    std::vector<Type*> struct_types;
    struct_types.push_back(ops_.state()->Int32Ty);
    struct_types.push_back(ops_.state()->Int1Ty);

    for(size_t i = 0; i < nf->ffi_data->arg_count; i++) {
      Value* current_arg = arg(i);
      Value* call_args[] = { ops_.vm(), current_arg, ops_.valid_flag() };

      switch(nf->ffi_data->args_info[i].type) {
      case RBX_FFI_TYPE_CHAR:
      case RBX_FFI_TYPE_UCHAR:
      case RBX_FFI_TYPE_SHORT:
      case RBX_FFI_TYPE_USHORT:
      case RBX_FFI_TYPE_INT:
      case RBX_FFI_TYPE_UINT:
      case RBX_FFI_TYPE_LONG:
      case RBX_FFI_TYPE_ULONG: {
        Signature sig(ops_.state(), ops_.NativeIntTy);
        sig << "State";
        sig << "Object";
        sig << llvm::PointerType::getUnqual(ops_.state()->Int1Ty);

        Value* val = sig.call("rbx_ffi_to_int", call_args, 3, "to_int",
                              ops_.b());

        Type* type = find_type(ops_, nf->ffi_data->args_info[i].type);
        ffi_type.push_back(type);

        if(type != ops_.NativeIntTy) {
          val = ops_.b().CreateTrunc(val, type, "truncated");
        }

        ffi_args.push_back(val);

        Value* valid = ops_.create_load(ops_.valid_flag());

        BasicBlock* cont = ops_.new_block("ffi_continue");
        ops_.create_conditional_branch(cont, failure(), valid);

        ops_.set_block(cont);
        break;
      }

      case RBX_FFI_TYPE_FLOAT: {
        Signature sig(ops_.state(), ops_.state()->FloatTy);
        sig << "State";
        sig << "Object";
        sig << llvm::PointerType::getUnqual(ops_.state()->Int1Ty);

        Value* val = sig.call("rbx_ffi_to_float", call_args, 3, "to_float",
                              ops_.b());

        ffi_type.push_back(val->getType());
        ffi_args.push_back(val);

        Value* valid = ops_.create_load(ops_.valid_flag());

        BasicBlock* cont = ops_.new_block("ffi_continue");
        ops_.create_conditional_branch(cont, failure(), valid);

        ops_.set_block(cont);
        break;
      }

      case RBX_FFI_TYPE_DOUBLE: {
        Signature sig(ops_.state(), ops_.state()->DoubleTy);
        sig << "State";
        sig << "Object";
        sig << llvm::PointerType::getUnqual(ops_.state()->Int1Ty);

        Value* val = sig.call("rbx_ffi_to_double", call_args, 3, "to_double",
                              ops_.b());

        ffi_type.push_back(val->getType());
        ffi_args.push_back(val);

        Value* valid = ops_.create_load(ops_.valid_flag());

        BasicBlock* cont = ops_.new_block("ffi_continue");
        ops_.create_conditional_branch(cont, failure(), valid);

        ops_.set_block(cont);
        break;
      }

      case RBX_FFI_TYPE_LONG_LONG:
      case RBX_FFI_TYPE_ULONG_LONG: {
        Signature sig(ops_.state(), ops_.state()->Int64Ty);
        sig << "State";
        sig << "Object";
        sig << llvm::PointerType::getUnqual(ops_.state()->Int1Ty);

        Value* val = sig.call("rbx_ffi_to_int64", call_args, 3, "to_int64",
                              ops_.b());

        ffi_type.push_back(val->getType());
        ffi_args.push_back(val);

        Value* valid = ops_.create_load(ops_.valid_flag());

        BasicBlock* cont = ops_.new_block("ffi_continue");
        ops_.create_conditional_branch(cont, failure(), valid);

        ops_.set_block(cont);
        break;
      }

      case RBX_FFI_TYPE_STATE:
        ffi_type.push_back(ops_.vm()->getType());
        ffi_args.push_back(ops_.vm());
        break;

      case RBX_FFI_TYPE_OBJECT:
        ffi_type.push_back(current_arg->getType());
        ffi_args.push_back(current_arg);
        break;

      case RBX_FFI_TYPE_PTR: {
        Type* type = llvm::PointerType::getUnqual(ops_.state()->Int8Ty);

        Signature sig(ops_.state(), type);
        sig << "State";
        sig << "Object";
        sig << llvm::PointerType::getUnqual(ops_.state()->Int1Ty);

        Value* val = sig.call("rbx_ffi_to_ptr", call_args, 3, "to_ptr",
                              ops_.b());

        ffi_type.push_back(type);
        ffi_args.push_back(val);

        Value* valid = ops_.create_load(ops_.valid_flag());

        BasicBlock* cont = ops_.new_block("ffi_continue");
        ops_.create_conditional_branch(cont, failure(), valid);

        ops_.set_block(cont);
        break;
      }

      case RBX_FFI_TYPE_STRING: {
        Type* type = llvm::PointerType::getUnqual(ops_.state()->Int8Ty);

        Signature sig(ops_.state(), type);
        sig << "State";
        sig << "Object";
        sig << llvm::PointerType::getUnqual(ops_.state()->Int1Ty);

        Value* val = sig.call("rbx_ffi_to_string", call_args, 3, "to_string",
                              ops_.b());

        ffi_type.push_back(type);
        ffi_args.push_back(val);

        Value* valid = ops_.create_load(ops_.valid_flag());

        BasicBlock* cont = ops_.new_block("ffi_continue");
        ops_.create_conditional_branch(cont, failure(), valid);

        ops_.set_block(cont);
        break;
      }

      default:
        rubinius::bug("Unknown FFI type in JIT FFI inliner");
      }
    }

    Signature check(ops_.state(), ops_.NativeIntTy);
    check << "State";
    check << "CallFrame";

    Value* check_args[] = { ops_.vm(), ops_.call_frame() };
    check.call("rbx_enter_unmanaged", check_args, 2, "unused", ops_.b());

    Type* return_type = find_type(ops_, nf->ffi_data->ret_info.type);

    FunctionType* ft = FunctionType::get(return_type, ffi_type, false);
    Value* ep_ptr = ops_.b().CreateIntToPtr(
            ConstantInt::get(ops_.state()->IntPtrTy, (intptr_t)nf->ffi_data->ep),
            llvm::PointerType::getUnqual(ft), "cast_to_function");

    Value* ffi_result = ops_.b().CreateCall(ep_ptr, ffi_args, "ffi_result");

    check.call("rbx_exit_unmanaged", check_args, 2, "unused", ops_.b());

    Value* res_args[] = { ops_.vm(), ffi_result };

    Value* result;
    switch(nf->ffi_data->ret_info.type) {
    case RBX_FFI_TYPE_CHAR:
    case RBX_FFI_TYPE_UCHAR:
    case RBX_FFI_TYPE_SHORT:
    case RBX_FFI_TYPE_USHORT:
    case RBX_FFI_TYPE_INT:
    case RBX_FFI_TYPE_UINT:
#ifndef IS_X8664
    case RBX_FFI_TYPE_LONG:
    case RBX_FFI_TYPE_ULONG:
#endif
    {
      Signature sig(ops_.state(), ops_.ObjType);
      sig << "State";
      sig << ops_.state()->Int32Ty;

      res_args[1] = ops_.b().CreateSExtOrBitCast(res_args[1],
                               ops_.state()->Int32Ty);

      result = sig.call("rbx_ffi_from_int32", res_args, 2, "to_obj",
                        ops_.b());
      break;
    }

    case RBX_FFI_TYPE_LONG_LONG:
    case RBX_FFI_TYPE_ULONG_LONG:
#ifdef IS_X8664
    case RBX_FFI_TYPE_LONG:
    case RBX_FFI_TYPE_ULONG:
#endif
    {
      Signature sig(ops_.state(), ops_.ObjType);
      sig << "State";
      sig << ops_.state()->Int64Ty;

      result = sig.call("rbx_ffi_from_int64", res_args, 2, "to_obj",
                        ops_.b());
      break;
    }

    case RBX_FFI_TYPE_FLOAT: {
      Signature sig(ops_.state(), ops_.ObjType);
      sig << "State";
      sig << ops_.state()->FloatTy;

      result = sig.call("rbx_ffi_from_float", res_args, 2, "to_obj",
                        ops_.b());
      break;
    }

    case RBX_FFI_TYPE_DOUBLE: {
      Signature sig(ops_.state(), ops_.ObjType);
      sig << "State";
      sig << ops_.state()->DoubleTy;

      result = sig.call("rbx_ffi_from_double", res_args, 2, "to_obj",
                        ops_.b());
      break;
    }

    case RBX_FFI_TYPE_PTR: {
      Signature sig(ops_.state(), ops_.ObjType);
      sig << "State";
      sig << llvm::PointerType::getUnqual(ops_.state()->Int8Ty);

      result = sig.call("rbx_ffi_from_ptr", res_args, 2, "to_obj",
                        ops_.b());
      break;
    }

    case RBX_FFI_TYPE_OBJECT:
      result = ffi_result;
      break;

    case RBX_FFI_TYPE_STRING: {
      Signature sig(ops_.state(), ops_.ObjType);
      sig << "State";
      sig << llvm::PointerType::getUnqual(ops_.state()->Int8Ty);

      result = sig.call("rbx_ffi_from_string", res_args, 2, "to_obj",
                        ops_.b());
      break;
    }

    case RBX_FFI_TYPE_STRPTR: {
      Signature sig(ops_.state(), ops_.ObjType);
      sig << "State";
      sig << llvm::PointerType::getUnqual(ops_.state()->Int8Ty);

      result = sig.call("rbx_ffi_from_string_with_pointer", res_args, 2, "to_obj",
                        ops_.b());
      break;
    }

    case RBX_FFI_TYPE_VOID:
      result = ops_.constant(cNil);
      break;

    default:
      result = 0;
      rubinius::bug("Invalid FFI type in JIT");
    }

    exception_safe();
    set_result(result);

    return true;
  }
}

#endif
