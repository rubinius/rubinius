#ifdef ENABLE_LLVM

#include "llvm/jit.hpp"
#include "llvm/inline.hpp"
#include "llvm/jit_workhorse.hpp"

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
    Executable* meth = cache_->method;

    if(AccessVariable* acc = try_as<AccessVariable>(meth)) {
      if(acc->write()->true_p()) {
        inline_ivar_write(klass, acc);
      } else {
        inline_ivar_access(klass, acc);
      }
      return true;
    } else if(CompiledMethod* cm = try_as<CompiledMethod>(meth)) {
      VMMethod* vmm = cm->backend_method_;

      if(!cm->primitive()->nil_p()) {
        return inline_primitive(klass, cm, cache_->method->execute);
      } else if(detect_trivial_method(cm)) {
        inline_trivial_method(klass, cm);
        return true;
      } else if(ops_.state()->config().jit_inline_generic) {
        InlineDecision decision = ops_.should_inline_p(vmm);
        if(decision == cInline) {
          if(ops_.state()->config().jit_inline_debug) {
            std::cerr << "inlining: "
              << ops_.state()->symbol_cstr(cm->scope()->module()->name())
              << "#"
              << ops_.state()->symbol_cstr(cm->name())
              << " into "
              << ops_.state()->symbol_cstr(ops_.vmmethod()->original->name())
              << "\n";
          }

          NoAccessManagedMemory unmemguard(ops_.state());
          inline_generic_method(klass, vmm);
          return true;
        } else {
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
      } else if(ops_.state()->config().jit_inline_debug) {
        std::cerr << "NOT inlining: "
          << ops_.state()->symbol_cstr(cm->scope()->module()->name())
          << "#"
          << ops_.state()->symbol_cstr(cm->name())
          << " into "
          << ops_.state()->symbol_cstr(ops_.vmmethod()->original->name())
          << ". generic inlining disabled\n";
      }
    }

    return false;
  }

  bool Inliner::detect_trivial_method(CompiledMethod* cm) {
    VMMethod* vmm = cm->backend_method_;

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
      // case InstructionSequence::insn_meta_push_1:
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
    cm->add_inliner(ops_.vmmethod());

    VMMethod* vmm = cm->backend_method_;

    Value* self = ops_.stack_top();

    BasicBlock* use_send = ops_.new_block("use_send");
    ops_.check_class(self, klass, use_send);

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
    ops_.stack_set_top(val);

    ops_.create_branch(after_);
    ops_.set_block(use_send);

    after_->moveAfter(use_send);
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

    acc->add_inliner(ops_.vmmethod());

    ops_.state()->add_accessor_inlined();

    Value* val  = ops_.stack_top();
    Value* self = ops_.stack_back(1);

    BasicBlock* use_send = ops_.new_block("use_send");
    ops_.check_reference_class(self, klass->class_id(), use_send);

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
          ops_.current_block());
    }

    ops_.stack_remove(1);
    ops_.stack_set_top(val);

    ops_.create_branch(after_);
    ops_.set_block(use_send);

    after_->moveAfter(use_send);

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

    acc->add_inliner(ops_.vmmethod());

    ops_.state()->add_accessor_inlined();

    Value* self = ops_.stack_top();

    BasicBlock* use_send = ops_.new_block("use_send");
    ops_.check_reference_class(self, klass->class_id(), use_send);

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
          ops_.current_block());
    }

    ops_.stack_set_top(ivar);

    ops_.create_branch(after_);
    ops_.set_block(use_send);

    after_->moveAfter(use_send);
  }

  void Inliner::inline_generic_method(Class* klass, VMMethod* vmm) {
    LLVMWorkHorse work(ops_.state());

    Value* self = ops_.stack_back(count_);

    BasicBlock* use_send = ops_.new_block("use_send");
    ops_.check_class(self, klass, use_send);

    std::vector<Value*> args;
    for(int i = count_ - 1; i >= 0; i--) {
      args.push_back(ops_.stack_back(i));
    }

    BasicBlock* entry = work.setup_inline(vmm, ops_.function(), ops_.vm(), ops_.call_frame(),
        self, ops_.constant(Qnil, ops_.state()->ptr_type("Module")), args);

    BasicBlock* on_return = ops_.new_block("inline_return");
    JITMethodInfo info(vmm);
    info.inline_return = on_return;
    info.inline_policy = ops_.inline_policy();
    info.called_args = count_;

    assert(work.generate_body(info));

    on_return->moveAfter(info.fin_block);
    use_send->moveBefore(entry);

    ops_.create_branch(entry);

    ops_.set_block(on_return);

    ops_.current_block()->getInstList().push_back(cast<Instruction>(info.return_value));
    ops_.stack_remove(count_ + 1);
    ops_.check_for_exception(info.return_value);
    ops_.stack_push(info.return_value);

    ops_.create_branch(after_);

    ops_.set_block(use_send);

    use_send->moveAfter(entry);
  }
}

#endif
