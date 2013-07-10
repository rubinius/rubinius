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
#include "builtin/mono_inline_cache.hpp"
#include "builtin/poly_inline_cache.hpp"

#include "ffi_util.hpp"

namespace rubinius {

  void Inliner::check_class(llvm::Value* recv, Class* klass, ClassData data) {
    guarded_type_ = ops_.check_class(recv, klass,
                                     data.f.class_id, data.f.serial_id,
                                     class_id_failure(), serial_id_failure());
    guarded_type_.inherit_source(ops_.context(), recv);
  }

  void Inliner::check_recv(Class* klass, ClassData data) {
    check_class(recv(), klass, data);
  }

  bool Inliner::consider_mono() {
    MonoInlineCache* cache = try_as<MonoInlineCache>(call_site_);
    if(!cache) return false;
    int hits = cache->hits();
    return inline_for_class(cache->receiver_class(), cache->receiver_data(), hits);
  }

  bool Inliner::consider_poly() {
    PolyInlineCache* cache = try_as<PolyInlineCache>(call_site_);

    if(!cache) return false;

    int classes_seen = cache->classes_seen();

    std::vector<InlineCacheEntry*> reachable_caches;

    for(int i = 0; i < classes_seen; ++i) {
      InlineCacheEntry* ice = cache->get_cache(i);

      // If we staticly know the type, don't inline code that can never
      // match by definition because of wrong class id's.
      type::KnownType type = type::KnownType::extract(ctx_, recv());
      if(type.known_p()) {
        if(type.fixnum_p()) {
          if(ice->receiver_class_id() != ops_.llvm_state()->fixnum_class_id()) continue;
        } else if(type.symbol_p()) {
          if(ice->receiver_class_id() != ops_.llvm_state()->symbol_class_id()) continue;
        } else if(type.instance_p()) {
          if(ice->receiver_class_id() != type.class_id()) continue;
        }
      }
      reachable_caches.push_back(ice);
    }

    if(reachable_caches.empty()) {
      if(ops_.llvm_state()->config().jit_inline_debug) {
        ctx_->inline_log("NOT inlining")
          << ops_.llvm_state()->symbol_debug_str(call_site_->name())
          << ". No reachable types found in IC.\n";
      }
      return false;
    }

    BasicBlock* fail = class_id_failure();
    BasicBlock* fallback = ops_.new_block("poly_fallback");
    BasicBlock* merge = ops_.new_block("merge");
    BasicBlock* current = ops_.current_block();

    ops_.set_block(merge);
    PHINode* phi = ops_.b().CreatePHI(ops_.ObjType, classes_seen, "poly_result");

    ops_.set_block(current);

    for(size_t i = 0; i < reachable_caches.size(); ++i) {
      InlineCacheEntry* ice = reachable_caches[i];

      // Fallback to the next for failure
      set_class_id_failure(fallback);

      if(!inline_for_class(ice->receiver_class(), ice->receiver_data(), ice->hits())) {
        // If we fail to inline this, emit a send to the method

        Value* call_site_ptr_const = ops_.b().CreateIntToPtr(
          ConstantInt::get(ops_.context()->IntPtrTy, (reinterpret_cast<uintptr_t>(call_site_ptr_))),
          ops_.ptr_type(ops_.ptr_type("CallSite")), "cast_to_call_site_ptr");

        Value* call_site_const = ops_.b().CreateLoad(call_site_ptr_const, "call_site_const");

        Value* execute_pos_idx[] = {
          ops_.context()->cint(0),
          ops_.context()->cint(offset::CallSite::executor),
        };

        Value* execute_pos = ops_.b().CreateGEP(call_site_const,
            execute_pos_idx, "execute_pos");

        Value* execute = ops_.b().CreateLoad(execute_pos, "execute");
        ops_.setup_out_args(call_site_->name(), count_);

        Value* call_args[] = {
          ops_.state(),
          call_site_const,
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
    set_class_id_failure(fail);

    set_result(phi);

    return true;
  }

  bool Inliner::inline_for_class(Class* klass, ClassData data, int hits) {
    if(!klass) return false;

    Module* defined_in = 0;
    Executable* meth = klass->find_method(call_site_->name(), &defined_in);

    if(!meth) {
      if(ops_.llvm_state()->config().jit_inline_debug) {
        ctx_->inline_log("NOT inlining")
          << ops_.llvm_state()->symbol_debug_str(call_site_->name())
          << ". Inliner error, method missing.\n";
      }
      return false;
    }

    if(Alias* alias = try_as<Alias>(meth)) {
      meth = alias->original_exec();
    }

    if(AccessVariable* acc = try_as<AccessVariable>(meth)) {
      if(acc->write()->true_p()) {
        inline_ivar_write(klass, data, acc);
      } else {
        inline_ivar_access(klass, data, acc);
      }
    } else if(CompiledCode* code = try_as<CompiledCode>(meth)) {
      MachineCode* mcode = code->machine_code();

      if(!code->primitive()->nil_p()) {
        if(!inline_primitive(klass, data, code, meth->execute)) return false;
        goto remember;
      }

      // Not yet sure why we'd hit a CompiledCode that hasn't been
      // internalized, but protect against that case none the less.
      if(!mcode) return false;

      if(detect_trivial_method(mcode, code)) {
        inline_trivial_method(klass, data, code);
      } else if(int which = detect_jit_intrinsic(klass, data, code)) {
        inline_intrinsic(klass, data, code, which);
      } else if(ops_.llvm_state()->config().jit_inline_generic) {
        InlineDecision decision;
        InlineOptions opts;

        InlinePolicy* policy = ops_.inline_policy();
        assert(policy);

        if(mcode->no_inline_p()) {
          decision = cInlineDisabled;
        } else if(ops_.info().hits / 10 > hits) {
          decision = cTooFewSends;
        } else {
          decision = policy->inline_p(mcode, opts);
        }

        if(decision != cInline) {
          if(ops_.llvm_state()->config().jit_inline_debug) {

            ctx_->inline_log("NOT inlining")
              << ops_.llvm_state()->enclosure_name(code)
              << "#"
              << ops_.llvm_state()->symbol_debug_str(code->name())
              << " into "
              << ops_.llvm_state()->symbol_debug_str(ops_.method_name())
              << ". ";

            switch(decision) {
            case cInlineDisabled:
              ops_.llvm_state()->log() << "inlining disabled by request";
              break;
            case cTooBig:
              ops_.llvm_state()->log() << policy->current_size() << " + "
                << mcode->total << " > "
                << policy->max_size();
              break;
            case cTooComplex:
              ops_.llvm_state()->log() << "too complex";
              if(!opts.allow_blocks) {
                ops_.llvm_state()->log() << " (block not allowed)";
              }
              break;
            case cTooFewSends:
              ops_.llvm_state()->log() << "too few sends: (" << hits << " / " << ops_.info().hits << ")";
              break;
            default:
              ops_.llvm_state()->log() << "no policy";
            }

            ops_.llvm_state()->log() << " ("
              << ops_.llvm_state()->symbol_debug_str(klass->module_name()) << ")";

            if(mcode->jitted()) {
              ops_.llvm_state()->log() << " (jitted)\n";
            } else {
              ops_.llvm_state()->log() << " (interp)\n";
            }
          }
          return false;
        }

        if(ops_.llvm_state()->config().jit_inline_debug) {
          ctx_->inline_log("inlining")
            << ops_.llvm_state()->enclosure_name(code)
            << "#"
            << ops_.llvm_state()->symbol_debug_str(code->name())
            << " into "
            << ops_.llvm_state()->symbol_debug_str(ops_.method_name());

          ConstantScope* cs = code->scope();
          if(kind_of<ConstantScope>(cs) && klass != cs->module() && !klass->module_name()->nil_p()) {
            ops_.llvm_state()->log() << " ("
              << ops_.llvm_state()->symbol_debug_str(klass->module_name()) << ")";
          }

          if(inline_block_) {
            ops_.llvm_state()->log() << " (w/ inline block)";
          }

          ops_.llvm_state()->log() << "\n";
        }

        policy->increase_size(mcode);
        meth->add_inliner(ops_.llvm_state()->shared().om, ops_.root_method_info()->method());

        inline_generic_method(klass, data, defined_in, code, mcode, hits);
        return true;
      } else {
        if(ops_.llvm_state()->config().jit_inline_debug) {
          ctx_->inline_log("NOT inlining")
            << ops_.llvm_state()->enclosure_name(code)
            << "#"
            << ops_.llvm_state()->symbol_debug_str(code->name())
            << " into "
            << ops_.llvm_state()->symbol_debug_str(ops_.method_name())
            << ". generic inlining disabled\n";
        }

        return false;
      }
    } else if(NativeFunction* nf = try_as<NativeFunction>(meth)) {
      if(inline_ffi(klass, data, nf)) {
        if(ops_.llvm_state()->config().jit_inline_debug) {
          ctx_->inline_log("inlining")
            << "FFI call to "
            << ops_.llvm_state()->symbol_debug_str(nf->name())
            << "() into "
            << ops_.llvm_state()->symbol_debug_str(ops_.method_name())
            << " (" << ops_.llvm_state()->symbol_debug_str(klass->module_name()) << ")\n";
        }
      } else {
        return false;
      }
    } else {
      if(ops_.llvm_state()->config().jit_inline_debug) {
        ctx_->inline_log("NOT inlining")
          << ops_.llvm_state()->symbol_debug_str(klass->module_name())
          << "#"
          << ops_.llvm_state()->symbol_debug_str(call_site_->name())
          << " into "
          << ops_.llvm_state()->symbol_debug_str(ops_.method_name())
          << ". unhandled executable type\n";
      }
      return false;
    }

remember:
    meth->add_inliner(ops_.llvm_state()->shared().om, ops_.root_method_info()->method());

    return true;
  }

  void Inliner::inline_block(JITInlineBlock* ib, Value* self) {
    if(ops_.llvm_state()->config().jit_inline_debug) {
      ctx_->inline_log("inlining block into")
        << ops_.llvm_state()->symbol_debug_str(ops_.method_name())
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

  void Inliner::inline_trivial_method(Class* klass, ClassData data, CompiledCode* code) {
    if(ops_.llvm_state()->config().jit_inline_debug) {
      ctx_->inline_log("inlining")
        << ops_.llvm_state()->enclosure_name(code)
        << "#"
        << ops_.llvm_state()->symbol_debug_str(code->name())
        << " into "
        << ops_.llvm_state()->symbol_debug_str(ops_.method_name())
        << " (" << ops_.llvm_state()->symbol_debug_str(klass->module_name()) << ") trivial\n";
    }

    MachineCode* mcode = code->machine_code();

    check_recv(klass, data);

    Value* val = 0;

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

  void Inliner::inline_ivar_write(Class* klass, ClassData data, AccessVariable* acc) {
    if(count_ != 1) return;

    if(ops_.llvm_state()->config().jit_inline_debug) {
      ctx_->inline_log("inlining")
        << "writer to '"
        << ops_.llvm_state()->symbol_debug_str(acc->name())
        << "' on "
        << ops_.llvm_state()->symbol_debug_str(klass->module_name())
        << " in "
        << "#"
        << ops_.llvm_state()->symbol_debug_str(ops_.method_name());
    }

    ctx_->enter_inline();
    ops_.llvm_state()->add_accessor_inlined();

    check_recv(klass, data);

    Value* val  = arg(0);

    Value* self = recv();

    // Figure out if we should use the table ivar lookup or
    // the slot ivar lookup.

    TypeInfo* ti = klass->type_info();
    TypeInfo::Slots::iterator it = ti->slots.find(acc->name()->index());

    bool found = false;

    if(it != ti->slots.end()) {
      int field = it->second;
      int offset = ti->slot_locations[field];
      ops_.set_object_type_slot(self, field, offset, ti->slot_types[field], val);
      found = true;
      if(ops_.llvm_state()->config().jit_inline_debug) {
        ops_.llvm_state()->log() << " (slot: " << it->second << ")\n";
      }
    } else {
      LookupTable* pii = klass->packed_ivar_info();
      if(!pii->nil_p()) {
        Fixnum* which = try_as<Fixnum>(pii->fetch(0, acc->name(), &found));
        if(found) {
          int index = which->to_native();
          int offset = sizeof(Object) + (sizeof(Object*) * index);

          ops_.set_object_slot(self, offset, val);

          if(ops_.llvm_state()->config().jit_inline_debug) {
            ops_.llvm_state()->log() << " (packed index: " << index << ", " << offset << ")";
          }
        }
      }

      if(!found) {
        Signature sig2(ops_.context(), "Object");
        sig2 << "State";
        sig2 << "CallFrame";
        sig2 << "Object";
        sig2 << "Object";
        sig2 << "Object";

        Value* call_args2[] = {
          ops_.state(),
          ops_.call_frame(),
          self,
          ops_.constant(acc->name()),
          val
        };

        sig2.call("rbx_set_ivar", call_args2, 5, "ivar",
            ops_.b());
      }
    }

    exception_safe();
    set_result(val);

    if(ops_.llvm_state()->config().jit_inline_debug) {
      ops_.llvm_state()->log() << "\n";
    }

    ctx_->leave_inline();
  }

  void Inliner::inline_ivar_access(Class* klass, ClassData data, AccessVariable* acc) {
    if(count_ != 0) return;

    if(ops_.llvm_state()->config().jit_inline_debug) {
      ctx_->inline_log("inlining")
        << "read to '"
        << ops_.llvm_state()->symbol_debug_str(acc->name())
        << "' on "
        << ops_.llvm_state()->symbol_debug_str(klass->module_name())
        << " in "
        << "#"
        << ops_.llvm_state()->symbol_debug_str(ops_.method_name());
    }

    ctx_->enter_inline();
    ops_.llvm_state()->add_accessor_inlined();

    check_recv(klass, data);

    // Figure out if we should use the table ivar lookup or
    // the slot ivar lookup.

    TypeInfo* ti = klass->type_info();
    TypeInfo::Slots::iterator it = ti->slots.find(acc->name()->index());

    Value* ivar = 0;
    Value* self = recv();

    if(it != ti->slots.end()) {
      int offset = ti->slot_locations[it->second];
      ivar = ops_.get_object_slot(self, offset);
      if(ops_.llvm_state()->config().jit_inline_debug) {
        ops_.llvm_state()->log() << " (slot: " << it->second << ")";
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

          if(ops_.llvm_state()->config().jit_inline_debug) {
            ops_.llvm_state()->log() << " (packed index: " << index << ", " << offset << ")";
          }
        }
      }

      if(!ivar) {
        Signature sig2(ops_.context(), "Object");
        sig2 << "State";
        sig2 << "Object";
        sig2 << "Object";

        Value* call_args2[] = {
          ops_.state(),
          self,
          ops_.constant(acc->name())
        };

        CallInst* ivar_call = sig2.call("rbx_push_ivar", call_args2, 3, "ivar", ops_.b());
        ivar_call->setOnlyReadsMemory();
        ivar_call->setDoesNotThrow();
        ivar = ivar_call;
      }
    }

    exception_safe();
    set_result(ivar);

    if(ops_.llvm_state()->config().jit_inline_debug) {
      ops_.llvm_state()->log() << "\n";
    }

    ctx_->leave_inline();

  }

  void Inliner::prime_info(JITMethodInfo& info) {
    info.set_parent_info(ops_.info());

    info.inline_policy = ops_.inline_policy();
    info.called_args = count_;
    info.root = ops_.root_method_info();
    info.set_inline_block(inline_block_);
  }

  void Inliner::inline_generic_method(Class* klass, ClassData data, Module* defined_in,
                                      CompiledCode* code, MachineCode* mcode, int hits) {

    ctx_->enter_inline();

    check_recv(klass, data);

    JITMethodInfo info(ctx_, code, mcode);

    prime_info(info);
    info.hits = hits;

    info.self_type = guarded_type_;

    info.set_self_class(klass);

    jit::RuntimeData* rd = new jit::RuntimeData(code, call_site_->name(), defined_in);
    ctx_->add_runtime_data(rd);

    jit::InlineMethodBuilder work(ops_.context(), info, rd);
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

    ctx_->leave_inline();
  }

  void Inliner::emit_inline_block(JITInlineBlock* ib, Value* self) {
    ctx_->enter_inline();

    JITMethodInfo info(ctx_, ib->method(), ib->machine_code());

    prime_info(info);

    info.is_block = true;
    info.hits = 0;

    info.set_creator_info(creator_info_);
    info.set_block_info(block_info_);
    info.self_type = ops_.info().self_type;
    info.set_self_class(creator_info_->self_class());

    jit::RuntimeData* rd = new jit::RuntimeData(ib->method(), nil<Symbol>(), nil<Module>());
    ctx_->add_runtime_data(rd);

    jit::InlineBlockBuilder work(ops_.context(), info, rd);
    work.valid_flag = ops_.valid_flag();

    JITStackArgs args(count_);
    if(from_unboxed_array_) args.set_from_unboxed_array();

    for(int i = count_ - 1, j = 0; i >= 0; i--, j++) {
      args.put(j, ops_.stack_back(i));
    }

    info.stack_args = &args;

    if(ib->machine_code()->call_count >= 0) ib->machine_code()->call_count /= 2;

    BasicBlock* entry = work.setup_inline_block(self,
        ops_.constant(cNil, ops_.context()->ptr_type("Module")), args);

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

    ctx_->leave_inline();
  }

  Type* find_type(JITOperations& ops_, size_t type) {
    switch(type) {
      case RBX_FFI_TYPE_CHAR:
      case RBX_FFI_TYPE_UCHAR:
        return ops_.context()->Int8Ty;

      case RBX_FFI_TYPE_SHORT:
      case RBX_FFI_TYPE_USHORT:
        return ops_.context()->Int16Ty;

      case RBX_FFI_TYPE_INT:
      case RBX_FFI_TYPE_UINT:
        return ops_.context()->Int32Ty;

      case RBX_FFI_TYPE_LONG:
      case RBX_FFI_TYPE_ULONG:
#ifdef IS_X8664
        return ops_.context()->Int64Ty;
#else
        return ops_.context()->Int32Ty;
#endif

      case RBX_FFI_TYPE_LONG_LONG:
      case RBX_FFI_TYPE_ULONG_LONG:
        return ops_.context()->Int64Ty;

      case RBX_FFI_TYPE_FLOAT:
        return ops_.context()->FloatTy;

      case RBX_FFI_TYPE_DOUBLE:
        return ops_.context()->DoubleTy;

      case RBX_FFI_TYPE_STRING:
      case RBX_FFI_TYPE_STRPTR:
      case RBX_FFI_TYPE_PTR:
      case RBX_FFI_TYPE_VOID:
        return llvm::PointerType::getUnqual(ops_.context()->Int8Ty);
    }

    assert(0 && "unknown type to return!");

    return 0;
  }

  bool Inliner::inline_ffi(Class* klass, ClassData data, NativeFunction* nf) {

    for(size_t i = 0; i < nf->ffi_data->arg_count; i++) {
        if(nf->ffi_data->args_info[i].type==RBX_FFI_TYPE_ENUM ||
           nf->ffi_data->args_info[i].type==RBX_FFI_TYPE_CALLBACK ||
           nf->ffi_data->args_info[i].type == RBX_FFI_TYPE_VARARGS) {
            return false;
        }
    }
    if(nf->ffi_data->ret_info.type==RBX_FFI_TYPE_ENUM ||
       nf->ffi_data->ret_info.type==RBX_FFI_TYPE_CALLBACK) {
        return false;
    }

    check_recv(klass, data);

    std::vector<Value*> ffi_args;
    std::vector<Type*> ffi_type;

    std::vector<Type*> struct_types;
    struct_types.push_back(ops_.context()->Int32Ty);
    struct_types.push_back(ops_.context()->Int1Ty);

    for(size_t i = 0; i < nf->ffi_data->arg_count; i++) {
      Value* current_arg = arg(i);
      Value* call_args[] = { ops_.state(), current_arg, ops_.valid_flag() };

      switch(nf->ffi_data->args_info[i].type) {
      case RBX_FFI_TYPE_CHAR:
      case RBX_FFI_TYPE_UCHAR:
      case RBX_FFI_TYPE_SHORT:
      case RBX_FFI_TYPE_USHORT:
      case RBX_FFI_TYPE_INT:
      case RBX_FFI_TYPE_UINT:
      case RBX_FFI_TYPE_LONG:
      case RBX_FFI_TYPE_ULONG: {
        Signature sig(ops_.context(), ops_.NativeIntTy);
        sig << "State";
        sig << "Object";
        sig << llvm::PointerType::getUnqual(ops_.context()->Int1Ty);

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
        ops_.create_conditional_branch(cont, class_id_failure(), valid);

        ops_.set_block(cont);
        break;
      }

      case RBX_FFI_TYPE_FLOAT: {
        Signature sig(ops_.context(), ops_.context()->FloatTy);
        sig << "State";
        sig << "Object";
        sig << llvm::PointerType::getUnqual(ops_.context()->Int1Ty);

        Value* val = sig.call("rbx_ffi_to_float", call_args, 3, "to_float",
                              ops_.b());

        ffi_type.push_back(val->getType());
        ffi_args.push_back(val);

        Value* valid = ops_.create_load(ops_.valid_flag());

        BasicBlock* cont = ops_.new_block("ffi_continue");
        ops_.create_conditional_branch(cont, class_id_failure(), valid);

        ops_.set_block(cont);
        break;
      }

      case RBX_FFI_TYPE_DOUBLE: {
        Signature sig(ops_.context(), ops_.context()->DoubleTy);
        sig << "State";
        sig << "Object";
        sig << llvm::PointerType::getUnqual(ops_.context()->Int1Ty);

        Value* val = sig.call("rbx_ffi_to_double", call_args, 3, "to_double",
                              ops_.b());

        ffi_type.push_back(val->getType());
        ffi_args.push_back(val);

        Value* valid = ops_.create_load(ops_.valid_flag());

        BasicBlock* cont = ops_.new_block("ffi_continue");
        ops_.create_conditional_branch(cont, class_id_failure(), valid);

        ops_.set_block(cont);
        break;
      }

      case RBX_FFI_TYPE_LONG_LONG:
      case RBX_FFI_TYPE_ULONG_LONG: {
        Signature sig(ops_.context(), ops_.context()->Int64Ty);
        sig << "State";
        sig << "Object";
        sig << llvm::PointerType::getUnqual(ops_.context()->Int1Ty);

        Value* val = sig.call("rbx_ffi_to_int64", call_args, 3, "to_int64",
                              ops_.b());

        ffi_type.push_back(val->getType());
        ffi_args.push_back(val);

        Value* valid = ops_.create_load(ops_.valid_flag());

        BasicBlock* cont = ops_.new_block("ffi_continue");
        ops_.create_conditional_branch(cont, class_id_failure(), valid);

        ops_.set_block(cont);
        break;
      }

      case RBX_FFI_TYPE_PTR: {
        Type* type = llvm::PointerType::getUnqual(ops_.context()->Int8Ty);

        Signature sig(ops_.context(), type);
        sig << "State";
        sig << "Object";
        sig << llvm::PointerType::getUnqual(ops_.context()->Int1Ty);

        Value* val = sig.call("rbx_ffi_to_ptr", call_args, 3, "to_ptr",
                              ops_.b());

        ffi_type.push_back(type);
        ffi_args.push_back(val);

        Value* valid = ops_.create_load(ops_.valid_flag());

        BasicBlock* cont = ops_.new_block("ffi_continue");
        ops_.create_conditional_branch(cont, class_id_failure(), valid);

        ops_.set_block(cont);
        break;
      }

      case RBX_FFI_TYPE_STRING: {
        Type* type = llvm::PointerType::getUnqual(ops_.context()->Int8Ty);

        Signature sig(ops_.context(), type);
        sig << "State";
        sig << "Object";
        sig << llvm::PointerType::getUnqual(ops_.context()->Int1Ty);

        Value* val = sig.call("rbx_ffi_to_string", call_args, 3, "to_string",
                              ops_.b());

        ffi_type.push_back(type);
        ffi_args.push_back(val);

        Value* valid = ops_.create_load(ops_.valid_flag());

        BasicBlock* cont = ops_.new_block("ffi_continue");
        ops_.create_conditional_branch(cont, class_id_failure(), valid);

        ops_.set_block(cont);
        break;
      }

      default:
        rubinius::bug("Unknown FFI type in JIT FFI inliner");
      }
    }

    Signature check(ops_.context(), ops_.NativeIntTy);
    check << "State";
    check << "CallFrame";

    Value* check_args[] = { ops_.state(), ops_.call_frame() };
    check.call("rbx_enter_unmanaged", check_args, 2, "unused", ops_.b());

    Type* return_type = find_type(ops_, nf->ffi_data->ret_info.type);

    FunctionType* ft = FunctionType::get(return_type, ffi_type, false);
    Value* ep_ptr = ops_.b().CreateIntToPtr(
            ConstantInt::get(ops_.context()->IntPtrTy, (intptr_t)nf->ffi_data->ep),
            llvm::PointerType::getUnqual(ft), "cast_to_function");

    Value* ffi_result = ops_.b().CreateCall(ep_ptr, ffi_args, "ffi_result");

    check.call("rbx_exit_unmanaged", check_args, 2, "unused", ops_.b());

    Value* res_args[] = { ops_.state(), ffi_result };

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
      Signature sig(ops_.context(), ops_.ObjType);
      sig << "State";
      sig << ops_.context()->Int32Ty;

      res_args[1] = ops_.b().CreateSExtOrBitCast(res_args[1],
                               ops_.context()->Int32Ty);

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
      Signature sig(ops_.context(), ops_.ObjType);
      sig << "State";
      sig << ops_.context()->Int64Ty;

      result = sig.call("rbx_ffi_from_int64", res_args, 2, "to_obj",
                        ops_.b());
      break;
    }

    case RBX_FFI_TYPE_FLOAT: {
      Signature sig(ops_.context(), ops_.ObjType);
      sig << "State";
      sig << ops_.context()->FloatTy;

      result = sig.call("rbx_ffi_from_float", res_args, 2, "to_obj",
                        ops_.b());
      break;
    }

    case RBX_FFI_TYPE_DOUBLE: {
      Signature sig(ops_.context(), ops_.ObjType);
      sig << "State";
      sig << ops_.context()->DoubleTy;

      result = sig.call("rbx_ffi_from_double", res_args, 2, "to_obj",
                        ops_.b());
      break;
    }

    case RBX_FFI_TYPE_PTR: {
      Signature sig(ops_.context(), ops_.ObjType);
      sig << "State";
      sig << llvm::PointerType::getUnqual(ops_.context()->Int8Ty);

      result = sig.call("rbx_ffi_from_ptr", res_args, 2, "to_obj",
                        ops_.b());
      break;
    }

    case RBX_FFI_TYPE_STRING: {
      Signature sig(ops_.context(), ops_.ObjType);
      sig << "State";
      sig << llvm::PointerType::getUnqual(ops_.context()->Int8Ty);

      result = sig.call("rbx_ffi_from_string", res_args, 2, "to_obj",
                        ops_.b());
      break;
    }

    case RBX_FFI_TYPE_STRPTR: {
      Signature sig(ops_.context(), ops_.ObjType);
      sig << "State";
      sig << llvm::PointerType::getUnqual(ops_.context()->Int8Ty);

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
