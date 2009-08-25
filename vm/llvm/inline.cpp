#ifdef ENABLE_LLVM

#include "llvm/jit.hpp"
#include "llvm/inline.hpp"
#include "llvm/jit_workhorse.hpp"

#include "builtin/methodtable.hpp"
#include "builtin/nativefunction.hpp"

#include "ffi_util.hpp"

namespace rubinius {
  bool Inliner::consider() {
    Class* klass = cache_->dominating_class();
    if(!klass) {
      if(ops_.state()->config().jit_inline_debug) {
        std::cerr << "NOT inlining: "
          << ops_.state()->symbol_cstr(cache_->name)
          << ". Cache contains " << cache_->classes_seen() << "\n";

        for(int i = 0; i < cache_->classes_seen(); i++) {
          std::cerr << "  " << ops_.state()->symbol_cstr(cache_->tracked_class(i)->name())
            << " " << cache_->tracked_class_hits(i) << "\n";
        }
      }

      return false;
    }

    // If the cache has a dominating class, inline!

    AccessManagedMemory memguard(ops_.state());

    Executable* meth = klass->find_method(cache_->name);
    if(!meth) {
      if(ops_.state()->config().jit_inline_debug) {
        std::cerr << "NOT inlining: "
          << ops_.state()->symbol_cstr(cache_->name)
          << ". Inliner error, method missing.\n";
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
      } else if(detect_trivial_method(cm)) {
        inline_trivial_method(klass, cm);
      } else if(ops_.state()->config().jit_inline_generic) {
        InlineDecision decision = ops_.should_inline_p(vmm);
        if(decision != cInline) {
          if(ops_.state()->config().jit_inline_debug) {
            InlinePolicy* policy = ops_.inline_policy();

            std::cerr << "NOT inlining: "
              << ops_.state()->symbol_cstr(cm->scope()->module()->name())
              << "#"
              << ops_.state()->symbol_cstr(cm->name())
              << " into "
              << ops_.state()->symbol_cstr(ops_.vmmethod()->original->name())
              << ". ";

            if(decision == cTooBig) {
              std::cerr << policy->current_size() << " + "
                << vmm->total << " > "
                << policy->max_size();
            } else if(decision == cTooComplex) {
              std::cerr << "too complex";
            } else {
              std::cerr << "no policy";
            }
            std::cerr << "\n";
          }
          return false;
        }

        if(ops_.state()->config().jit_inline_debug) {
          std::cerr << "inlining: "
            << ops_.state()->symbol_cstr(cm->scope()->module()->name())
            << "#"
            << ops_.state()->symbol_cstr(cm->name())
            << " into "
            << ops_.state()->symbol_cstr(ops_.vmmethod()->original->name())
            << " (" << ops_.state()->symbol_cstr(klass->name()) << ")\n";
        }

        NoAccessManagedMemory unmemguard(ops_.state());
        inline_generic_method(klass, vmm);
      } else {
        if(ops_.state()->config().jit_inline_debug) {
          std::cerr << "NOT inlining: "
            << ops_.state()->symbol_cstr(cm->scope()->module()->name())
            << "#"
            << ops_.state()->symbol_cstr(cm->name())
            << " into "
            << ops_.state()->symbol_cstr(ops_.vmmethod()->original->name())
            << ". generic inlining disabled\n";
        }

        return false;
      }
    } else if(NativeFunction* nf = try_as<NativeFunction>(meth)) {
      if(inline_ffi(klass, nf)) {
        if(ops_.state()->config().jit_inline_debug) {
          std::cerr << "inlining: FFI "
            << ops_.state()->symbol_cstr(nf->name())
            << " into "
            << ops_.state()->symbol_cstr(ops_.vmmethod()->original->name())
            << " (" << ops_.state()->symbol_cstr(klass->name()) << ")\n";
        }
      } else {
        return false;
      }
    } else {
      if(ops_.state()->config().jit_inline_debug) {
        std::cerr << "NOT inlining: "
          << ops_.state()->symbol_cstr(klass->name())
          << "#"
          << ops_.state()->symbol_cstr(cache_->name)
          << " into "
          << ops_.state()->symbol_cstr(ops_.vmmethod()->original->name())
          << ". unhandled executable type\n";
      }
      return false;
    }

    meth->add_inliner(ops_.root_vmmethod());

    return true;
  }

  bool Inliner::detect_trivial_method(CompiledMethod* cm) {
    VMMethod* vmm = cm->backend_method();

    opcode* stream = vmm->opcodes;
    size_t size_max = 2;
    switch(stream[0]) {
    case InstructionSequence::insn_push_int:
      size_max++;
      break;
    case InstructionSequence::insn_push_literal:
      if(kind_of<Symbol>(cm->literals()->at(stream[1]))) {
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
    VMMethod* vmm = cm->backend_method();

    Value* self = recv();

    ops_.check_class(self, klass, failure());

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
      std::cerr << "inlining: writer to '"
        << ops_.state()->symbol_cstr(acc->name())
        << "' on "
        << ops_.state()->symbol_cstr(klass->name())
        << " in "
        << "#"
        << ops_.state()->symbol_cstr(ops_.vmmethod()->original->name())
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
      sig2 << "Object";
      sig2 << "Object";
      sig2 << "Object";

      Value* call_args2[] = {
        ops_.vm(),
        self,
        ops_.constant(acc->name()),
        val
      };

      sig2.call("rbx_set_table_ivar", call_args2, 4, "ivar",
          ops_.b());
    }

    exception_safe();
    set_result(val);
  }

  void Inliner::inline_ivar_access(Class* klass, AccessVariable* acc) {
    if(count_ != 0) return;

    if(ops_.state()->config().jit_inline_debug) {
      std::cerr << "inlining: read to '"
        << ops_.state()->symbol_cstr(acc->name())
        << "' on "
        << ops_.state()->symbol_cstr(klass->name())
        << " in "
        << "#"
        << ops_.state()->symbol_cstr(ops_.vmmethod()->original->name())
        << "\n";
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
    } else {
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

    exception_safe();
    set_result(ivar);
  }

  void Inliner::inline_generic_method(Class* klass, VMMethod* vmm) {
    LLVMWorkHorse work(ops_.state(), vmm);
    work.valid_flag = ops_.valid_flag();

    Value* self = recv();

    ops_.check_class(self, klass, failure());

    std::vector<Value*> args;
    for(int i = count_ - 1; i >= 0; i--) {
      args.push_back(ops_.stack_back(i));
    }

    vmm->call_count /= 2;

    BasicBlock* entry = work.setup_inline(ops_.function(), ops_.vm(), ops_.call_frame(),
        self, ops_.constant(Qnil, ops_.state()->ptr_type("Module")), args);

    BasicBlock* on_return = ops_.new_block("inline_return");
    JITMethodInfo info(vmm);
    info.inline_return = on_return;
    info.inline_policy = ops_.inline_policy();
    info.called_args = count_;
    info.root = ops_.root_method_info();

    assert(work.generate_body(info));

    on_return->moveAfter(info.fin_block);

    ops_.create_branch(entry);

    ops_.set_block(on_return);

    ops_.b().Insert(cast<Instruction>(info.return_value));

    set_result(info.return_value);
  }

  const Type* find_type(size_t type) {
    switch(type) {
      case RBX_FFI_TYPE_CHAR:
      case RBX_FFI_TYPE_UCHAR:
        return Type::Int8Ty;

      case RBX_FFI_TYPE_SHORT:
      case RBX_FFI_TYPE_USHORT:
        return Type::Int16Ty;

      case RBX_FFI_TYPE_INT:
      case RBX_FFI_TYPE_UINT:
        return Type::Int32Ty;

      case RBX_FFI_TYPE_LONG:
      case RBX_FFI_TYPE_ULONG:
#ifdef IS_X8664
        return Type::Int64Ty;
#else
        return Type::Int32Ty;
#endif

      case RBX_FFI_TYPE_LONG_LONG:
      case RBX_FFI_TYPE_ULONG_LONG:
        return Type::Int64Ty;

      case RBX_FFI_TYPE_FLOAT:
        return Type::FloatTy;

      case RBX_FFI_TYPE_DOUBLE:
        return Type::DoubleTy;

      case RBX_FFI_TYPE_OBJECT:
      case RBX_FFI_TYPE_STRING:
      case RBX_FFI_TYPE_STRPTR:
      case RBX_FFI_TYPE_PTR:
        return PointerType::getUnqual(Type::Int8Ty);
    }

    return 0;
  }

  bool Inliner::inline_ffi(Class* klass, NativeFunction* nf) {
    Value* self = recv();

    ops_.check_class(self, klass, failure());

    ///

    std::vector<Value*> ffi_args;
    std::vector<const Type*> ffi_type;

    std::vector<const Type*> struct_types;
    struct_types.push_back(Type::Int32Ty);
    struct_types.push_back(Type::Int1Ty);

    for(size_t i = 0; i < nf->arg_count; i++) {
      Value* current_arg = arg(i);
      Value* call_args[] = { ops_.vm(), current_arg, ops_.valid_flag() };

      switch(nf->arg_types[i]) {
      case RBX_FFI_TYPE_CHAR:
      case RBX_FFI_TYPE_UCHAR:
      case RBX_FFI_TYPE_SHORT:
      case RBX_FFI_TYPE_USHORT:
      case RBX_FFI_TYPE_INT:
      case RBX_FFI_TYPE_UINT:
      case RBX_FFI_TYPE_LONG:
      case RBX_FFI_TYPE_ULONG: {
        Signature sig(ops_.state(), Type::Int32Ty);
        sig << "VM";
        sig << "Object";
        sig << PointerType::getUnqual(Type::Int1Ty);

        Value* val = sig.call("rbx_ffi_to_int", call_args, 3, "to_int",
                              ops_.b());

        const Type* type = find_type(nf->arg_types[i]);
        ffi_type.push_back(type);

        if(type != Type::Int32Ty) {
          ops_.b().CreateTrunc(val, type, "truncated");
        }

        ffi_args.push_back(val);

        Value* valid = ops_.create_load(ops_.valid_flag());

        BasicBlock* cont = ops_.new_block("ffi_continue");
        ops_.create_conditional_branch(cont, failure(), valid);

        ops_.set_block(cont);
        break;
      }

      case RBX_FFI_TYPE_FLOAT: {
        Signature sig(ops_.state(), Type::FloatTy);
        sig << "VM";
        sig << "Object";
        sig << PointerType::getUnqual(Type::Int1Ty);

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
        Signature sig(ops_.state(), Type::DoubleTy);
        sig << "VM";
        sig << "Object";
        sig << PointerType::getUnqual(Type::Int1Ty);

        Value* val = sig.call("rbx_ffi_to_double", call_args, 3, "to_double",
                              ops_.b());

        ffi_type.push_back(val->getType());
        ffi_args.push_back(val);

        Value* valid = ops_.create_load(ops_.valid_flag());

        BasicBlock* cont = ops_.new_block("ffi_continue");
        ops_.create_conditional_branch(cont, failure(), valid);

        ops_.set_block(cont);
      }

      case RBX_FFI_TYPE_LONG_LONG:
      case RBX_FFI_TYPE_ULONG_LONG: {
        Signature sig(ops_.state(), Type::Int64Ty);
        sig << "VM";
        sig << "Object";
        sig << PointerType::getUnqual(Type::Int1Ty);

        Value* val = sig.call("rbx_ffi_to_int64", call_args, 3, "to_int64",
                              ops_.b());

        ffi_type.push_back(val->getType());
        ffi_args.push_back(val);

        Value* valid = ops_.create_load(ops_.valid_flag());

        BasicBlock* cont = ops_.new_block("ffi_continue");
        ops_.create_conditional_branch(cont, failure(), valid);

        ops_.set_block(cont);
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
        const Type* type = PointerType::getUnqual(Type::Int8Ty);

        Signature sig(ops_.state(), type);
        sig << "VM";
        sig << "Object";
        sig << PointerType::getUnqual(Type::Int1Ty);

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
        const Type* type = PointerType::getUnqual(Type::Int8Ty);

        Signature sig(ops_.state(), type);
        sig << "VM";
        sig << "Object";
        sig << PointerType::getUnqual(Type::Int1Ty);

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

    const Type* return_type = find_type(nf->ret_type);

    FunctionType* ft = FunctionType::get(return_type, ffi_type, false);
    Value* ep_ptr = ops_.b().CreateIntToPtr(
            ConstantInt::get(ops_.IntPtrTy, (intptr_t)nf->ep),
            PointerType::getUnqual(ft), "cast_to_function");

    Value* ffi_result = ops_.b().CreateCall(ep_ptr, ffi_args.begin(),
                           ffi_args.end(), "ffi_result");

    Value* res_args[] = { ops_.vm(), ffi_result };

    Value* result;
    switch(nf->ret_type) {
    case RBX_FFI_TYPE_CHAR:
    case RBX_FFI_TYPE_UCHAR:
    case RBX_FFI_TYPE_SHORT:
    case RBX_FFI_TYPE_USHORT:
    case RBX_FFI_TYPE_INT:
    case RBX_FFI_TYPE_UINT:
    case RBX_FFI_TYPE_LONG:
    case RBX_FFI_TYPE_ULONG: {
      // TODO this won't promote to bignum, so we'll get
      // invalidate results for large numbers!
      result = ops_.fixnum_tag(ffi_result);
      break;
    }

    case RBX_FFI_TYPE_LONG_LONG:
    case RBX_FFI_TYPE_ULONG_LONG: {
      Signature sig(ops_.state(), ops_.ObjType);
      sig << "VM";
      sig << Type::Int64Ty;

      result = sig.call("rbx_ffi_from_int64", res_args, 2, "to_obj",
                        ops_.b());
      break;
    }

    case RBX_FFI_TYPE_FLOAT: {
      Signature sig(ops_.state(), ops_.ObjType);
      sig << "VM";
      sig << Type::FloatTy;

      result = sig.call("rbx_ffi_from_float", res_args, 2, "to_obj",
                        ops_.b());
      break;
    }

    case RBX_FFI_TYPE_DOUBLE: {
      Signature sig(ops_.state(), ops_.ObjType);
      sig << "VM";
      sig << Type::DoubleTy;

      result = sig.call("rbx_ffi_from_double", res_args, 2, "to_obj",
                        ops_.b());
      break;
    }

    case RBX_FFI_TYPE_PTR: {
      Signature sig(ops_.state(), ops_.ObjType);
      sig << "VM";
      sig << PointerType::getUnqual(Type::Int8Ty);

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
      sig << PointerType::getUnqual(Type::Int8Ty);

      result = sig.call("rbx_ffi_from_string", res_args, 2, "to_obj",
                        ops_.b());
      break;
    }

    case RBX_FFI_TYPE_STRPTR: {
      Signature sig(ops_.state(), ops_.ObjType);
      sig << "VM";
      sig << PointerType::getUnqual(Type::Int8Ty);

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
