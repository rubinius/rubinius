#ifdef ENABLE_LLVM

#include "llvm/jit.hpp"
#include "llvm/inline.hpp"
#include "llvm/jit_inline_method.hpp"
#include "llvm/jit_inline_block.hpp"
#include "llvm/jit_runtime.hpp"
#include "llvm/jit_context.hpp"

#include "builtin/methodtable.hpp"
#include "builtin/nativefunction.hpp"
#include "builtin/lookuptable.hpp"

#include "ffi_util.hpp"

namespace rubinius {
  bool Inliner::consider() {
    Class* klass = cache_->dominating_class();
    if(!klass) {
      if(ops_.state()->config().jit_inline_debug) {
        context_.inline_log("NOT inlining")
          << ops_.state()->symbol_cstr(cache_->name)
          << ". Cache contains " << cache_->classes_seen() << " entries\n";

        for(int i = 0; i < cache_->classes_seen(); i++) {
          context_.inline_log("class")
            << ops_.state()->symbol_cstr(cache_->tracked_class(i)->name())
            << " " << cache_->tracked_class_hits(i) << "\n";
        }
      }

      return false;
    }

    // If the cache has a dominating class, inline!

    Module* defined_in = 0;
    Executable* meth = klass->find_method(cache_->name, &defined_in);

    if(!meth) {
      if(ops_.state()->config().jit_inline_debug) {
        context_.inline_log("NOT inlining")
          << ops_.state()->symbol_cstr(cache_->name)
          << ". Inliner error, method missing.\n";
      }
      return false;
    }

    if(instance_of<Module>(defined_in)) {
      if(ops_.state()->config().jit_inline_debug) {
        context_.inline_log("NOT inlining")
          << ops_.state()->symbol_cstr(cache_->name)
          << ". Not inlining methods defined in Modules.\n";
      }
      return false;
    }

    if(AccessVariable* acc = try_as<AccessVariable>(meth)) {
      if(acc->write()->true_p()) {
        inline_ivar_write(klass, acc);
      } else {
        inline_ivar_access(klass, acc);
      }
    } else if(CompiledMethod* cm = try_as<CompiledMethod>(meth)) {
      VMMethod* vmm = cm->backend_method();

      if(!cm->primitive()->nil_p()) {
        if(!inline_primitive(klass, cm, meth->execute)) return false;
        goto remember;
      }

      // Not yet sure why we'd hit a CompiledMethod that hasn't been
      // internalized, but protect against that case none the less.
      if(!vmm) return false;

      if(detect_trivial_method(vmm, cm)) {
        inline_trivial_method(klass, cm);
      } else if(int which = detect_jit_intrinsic(klass, cm)) {
        inline_intrinsic(klass, cm, which);
      } else if(ops_.state()->config().jit_inline_generic) {
        InlineDecision decision;
        InlineOptions opts;

        InlinePolicy* policy = ops_.inline_policy();
        assert(policy);

        if(vmm->no_inline_p()) {
          decision = cInlineDisabled;
        } else {
          decision = policy->inline_p(vmm, opts);
        }

        if(decision != cInline) {
          if(ops_.state()->config().jit_inline_debug) {

            context_.inline_log("NOT inlining")
              << ops_.state()->enclosure_name(cm)
              << "#"
              << ops_.state()->symbol_cstr(cm->name())
              << " into "
              << ops_.state()->symbol_cstr(ops_.method_name())
              << ". ";

            switch(decision) {
            case cInlineDisabled:
              ops_.state()->log() << "inlining disabled by request";
              break;
            case cTooBig:
              ops_.state()->log() << policy->current_size() << " + "
                << vmm->total << " > "
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
            ops_.state()->log() << "\n";
          }
          return false;
        }

        if(ops_.state()->config().jit_inline_debug) {
          context_.inline_log("inlining")
            << ops_.state()->enclosure_name(cm)
            << "#"
            << ops_.state()->symbol_cstr(cm->name())
            << " into "
            << ops_.state()->symbol_cstr(ops_.method_name());

          StaticScope* ss = cm->scope();
          if(kind_of<StaticScope>(ss) && klass != ss->module() && !klass->name()->nil_p()) {
            ops_.state()->log() << " ("
              << ops_.state()->symbol_cstr(klass->name()) << ")";
          }

          if(inline_block_) {
            ops_.state()->log() << " (w/ inline block)";
          }

          ops_.state()->log() << "\n";
        }

        policy->increase_size(vmm);
        meth->add_inliner(ops_.state()->shared().om, ops_.root_method_info()->method());

        inline_generic_method(klass, defined_in, cm, vmm);
        return true;
      } else {
        if(ops_.state()->config().jit_inline_debug) {
          context_.inline_log("NOT inlining")
            << ops_.state()->enclosure_name(cm)
            << "#"
            << ops_.state()->symbol_cstr(cm->name())
            << " into "
            << ops_.state()->symbol_cstr(ops_.method_name())
            << ". generic inlining disabled\n";
        }

        return false;
      }
    } else if(NativeFunction* nf = try_as<NativeFunction>(meth)) {
      if(inline_ffi(klass, nf)) {
        if(ops_.state()->config().jit_inline_debug) {
          context_.inline_log("inlining")
            << "FFI call to "
            << ops_.state()->symbol_cstr(nf->name())
            << "() into "
            << ops_.state()->symbol_cstr(ops_.method_name())
            << " (" << ops_.state()->symbol_cstr(klass->name()) << ")\n";
        }
      } else {
        return false;
      }
    } else {
      if(ops_.state()->config().jit_inline_debug) {
        context_.inline_log("NOT inlining")
          << ops_.state()->symbol_cstr(klass->name())
          << "#"
          << ops_.state()->symbol_cstr(cache_->name)
          << " into "
          << ops_.state()->symbol_cstr(ops_.method_name())
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
        << ops_.state()->symbol_cstr(ops_.method_name())
        << "\n";
    }

    emit_inline_block(ib, self);
  }

  bool Inliner::detect_trivial_method(VMMethod* vmm, CompiledMethod* cm) {
    opcode* stream = vmm->opcodes;
    size_t size_max = 2;
    switch(stream[0]) {
    case InstructionSequence::insn_push_int:
      size_max++;
      break;
    case InstructionSequence::insn_push_literal:
      if(cm && kind_of<Symbol>(cm->literals()->at(stream[1]))) {
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

    if(vmm->total == size_max &&
        count_ == 0 &&
        vmm->required_args == vmm->total_args &&
        vmm->total_args == 0) return true;
    return false;
  }

  void Inliner::inline_trivial_method(Class* klass, CompiledMethod* cm) {
    if(ops_.state()->config().jit_inline_debug) {
      context_.inline_log("inlining")
        << ops_.state()->enclosure_name(cm)
        << "#"
        << ops_.state()->symbol_cstr(cm->name())
        << " into "
        << ops_.state()->symbol_cstr(ops_.method_name())
        << " (" << ops_.state()->symbol_cstr(klass->name()) << ") trivial\n";
    }

    VMMethod* vmm = cm->backend_method();

    Value* self = recv();

    if(klass) {
      ops_.check_class(self, klass, failure());
    }

    Value* val = 0;
    /////

    opcode* stream = vmm->opcodes;
    switch(stream[0]) {
    case InstructionSequence::insn_push_int:
      val = ops_.constant(Fixnum::from(stream[1]));
      break;
    case InstructionSequence::insn_push_literal: {
      Symbol* sym = try_as<Symbol>(cm->literals()->at(stream[1]));
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
      val = self;
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
        << ops_.state()->symbol_cstr(acc->name())
        << "' on "
        << ops_.state()->symbol_cstr(klass->name())
        << " in "
        << "#"
        << ops_.state()->symbol_cstr(ops_.method_name())
        << "\n";
    }

    ops_.state()->add_accessor_inlined();

    Value* val  = arg(0);
    Value* self = recv();

    ops_.check_reference_class(self, klass->class_id(), failure());

    // Figure out if we should use the table ivar lookup or
    // the slot ivar lookup.

    TypeInfo* ti = klass->type_info();
    TypeInfo::Slots::iterator it = ti->slots.find(acc->name()->index());

    if(it != ti->slots.end()) {
      int offset = ti->slot_locations[it->second];
      ops_.set_object_slot(self, offset, val);
    } else {
      Signature sig2(ops_.state(), "Object");
      sig2 << "VM";
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
  }

  void Inliner::inline_ivar_access(Class* klass, AccessVariable* acc) {
    if(count_ != 0) return;

    if(ops_.state()->config().jit_inline_debug) {
      context_.inline_log("inlining")
        << "read to '"
        << ops_.state()->symbol_cstr(acc->name())
        << "' on "
        << ops_.state()->symbol_cstr(klass->name())
        << " in "
        << "#"
        << ops_.state()->symbol_cstr(ops_.method_name());
    }

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

          Value* cmp = ops_.b().CreateICmpEQ(slot_val, ops_.constant(Qundef), "prune_undef");

          ivar = ops_.b().CreateSelect(cmp,
                                       ops_.constant(Qnil), slot_val,
                                       "select ivar");

          if(ops_.state()->config().jit_inline_debug) {
            ops_.state()->log() << " (packed index: " << index << ")";
          }
        }
      }

      if(!ivar) {
        Signature sig2(ops_.state(), "Object");
        sig2 << "VM";
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

  }

  void Inliner::inline_generic_method(Class* klass, Module* defined_in,
                                      CompiledMethod* cm, VMMethod* vmm) {
    context_.enter_inline();
    Value* self = recv();
    ops_.check_class(self, klass, failure());

    JITMethodInfo info(context_, cm, vmm);
    info.set_parent_info(ops_.info());

    info.inline_policy = ops_.inline_policy();
    info.called_args = count_;
    info.root = ops_.root_method_info();
    info.set_inline_block(inline_block_);

    jit::RuntimeData* rd = new jit::RuntimeData(cm, cache_->name, defined_in);
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
      blk = ops_.constant(Qnil);
      for(int i = count_ - 1; i >= 0; i--) {
        args.push_back(ops_.stack_back(i));
      }
    }

    if(vmm->call_count >= 0) vmm->call_count /= 2;

    BasicBlock* entry = work.setup_inline(self, blk, args);

    if(!work.generate_body()) { abort(); }

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

    JITMethodInfo info(context_, ib->method(), ib->code());
    info.set_parent_info(ops_.info());

    info.inline_policy = ops_.inline_policy();
    info.called_args = count_;
    info.root = ops_.root_method_info();
    info.is_block = true;

    info.set_creator_info(creator_info_);
    info.set_inline_block(inline_block_);
    info.set_block_info(block_info_);

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

    if(ib->code()->call_count >= 0) ib->code()->call_count /= 2;

    BasicBlock* entry = work.setup_inline_block(self,
        ops_.constant(Qnil, ops_.state()->ptr_type("Module")));

    if(!work.generate_body()) { abort(); }

    // Branch to the inlined block!
    ops_.create_branch(entry);

    // Set the basic block to continue with being the return_pad!
    ops_.set_block(info.return_pad());

    // Make the value available to the code that called inliner to
    // check and use.
    set_result(info.return_phi());

    context_.leave_inline();
  }

  const Type* find_type(JITOperations& ops_, size_t type) {
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
    Value* self = recv();

    ops_.check_class(self, klass, failure());

    ///

    std::vector<Value*> ffi_args;
    std::vector<const Type*> ffi_type;

    std::vector<const Type*> struct_types;
    struct_types.push_back(ops_.state()->Int32Ty);
    struct_types.push_back(ops_.state()->Int1Ty);

    for(size_t i = 0; i < nf->ffi_data->arg_count; i++) {
      Value* current_arg = arg(i);
      Value* call_args[] = { ops_.vm(), current_arg, ops_.valid_flag() };

      switch(nf->ffi_data->arg_types[i]) {
      case RBX_FFI_TYPE_CHAR:
      case RBX_FFI_TYPE_UCHAR:
      case RBX_FFI_TYPE_SHORT:
      case RBX_FFI_TYPE_USHORT:
      case RBX_FFI_TYPE_INT:
      case RBX_FFI_TYPE_UINT:
      case RBX_FFI_TYPE_LONG:
      case RBX_FFI_TYPE_ULONG: {
        Signature sig(ops_.state(), ops_.NativeIntTy);
        sig << "VM";
        sig << "Object";
        sig << llvm::PointerType::getUnqual(ops_.state()->Int1Ty);

        Value* val = sig.call("rbx_ffi_to_int", call_args, 3, "to_int",
                              ops_.b());

        const Type* type = find_type(ops_, nf->ffi_data->arg_types[i]);
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
        sig << "VM";
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
        sig << "VM";
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
        sig << "VM";
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
        const Type* type = llvm::PointerType::getUnqual(ops_.state()->Int8Ty);

        Signature sig(ops_.state(), type);
        sig << "VM";
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
        const Type* type = llvm::PointerType::getUnqual(ops_.state()->Int8Ty);

        Signature sig(ops_.state(), type);
        sig << "VM";
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
        abort();
      }
    }

    Signature check(ops_.state(), ops_.NativeIntTy);
    check << "VM";
    check << "CallFrame";

    Value* check_args[] = { ops_.vm(), ops_.call_frame() };
    check.call("rbx_enter_unmanaged", check_args, 2, "unused", ops_.b());

    const Type* return_type = find_type(ops_, nf->ffi_data->ret_type);

    FunctionType* ft = FunctionType::get(return_type, ffi_type, false);
    Value* ep_ptr = ops_.b().CreateIntToPtr(
            ConstantInt::get(ops_.state()->IntPtrTy, (intptr_t)nf->ffi_data->ep),
            llvm::PointerType::getUnqual(ft), "cast_to_function");

    Value* ffi_result = ops_.b().CreateCall(ep_ptr, ffi_args.begin(),
                           ffi_args.end(), "ffi_result");

    check.call("rbx_exit_unmanaged", check_args, 2, "unused", ops_.b());

    Value* res_args[] = { ops_.vm(), ffi_result };

    Value* result;
    switch(nf->ffi_data->ret_type) {
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
      sig << "VM";
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
      sig << "VM";
      sig << ops_.state()->Int64Ty;

      result = sig.call("rbx_ffi_from_int64", res_args, 2, "to_obj",
                        ops_.b());
      break;
    }

    case RBX_FFI_TYPE_FLOAT: {
      Signature sig(ops_.state(), ops_.ObjType);
      sig << "VM";
      sig << ops_.state()->FloatTy;

      result = sig.call("rbx_ffi_from_float", res_args, 2, "to_obj",
                        ops_.b());
      break;
    }

    case RBX_FFI_TYPE_DOUBLE: {
      Signature sig(ops_.state(), ops_.ObjType);
      sig << "VM";
      sig << ops_.state()->DoubleTy;

      result = sig.call("rbx_ffi_from_double", res_args, 2, "to_obj",
                        ops_.b());
      break;
    }

    case RBX_FFI_TYPE_PTR: {
      Signature sig(ops_.state(), ops_.ObjType);
      sig << "VM";
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
      sig << "VM";
      sig << llvm::PointerType::getUnqual(ops_.state()->Int8Ty);

      result = sig.call("rbx_ffi_from_string", res_args, 2, "to_obj",
                        ops_.b());
      break;
    }

    case RBX_FFI_TYPE_STRPTR: {
      Signature sig(ops_.state(), ops_.ObjType);
      sig << "VM";
      sig << llvm::PointerType::getUnqual(ops_.state()->Int8Ty);

      result = sig.call("rbx_ffi_from_string_with_pointer", res_args, 2, "to_obj",
                        ops_.b());
      break;
    }

    case RBX_FFI_TYPE_VOID:
      result = ops_.constant(Qnil);
      break;

    default:
      result = 0;
      std::cout << "Invalid return type.\n";
      abort();

    }

    exception_safe();
    set_result(result);

    return true;
  }
}

#endif
