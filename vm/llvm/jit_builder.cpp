#ifdef ENABLE_LLVM

#include "llvm/jit_builder.hpp"
#include "call_frame.hpp"
#include "vmmethod.hpp"

#include "llvm/jit_visit.hpp"
#include "llvm/control_flow.hpp"
#include "llvm/cfg.hpp"

#include <llvm/Analysis/CaptureTracking.h>

namespace rubinius {
namespace jit {

  Builder::Builder(LLVMState* ls, JITMethodInfo& i)
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
    obj_ary_type = llvm::PointerType::getUnqual(obj_type);
  }

  Value* Builder::get_field(Value* val, int which) {
    return b().CreateConstGEP2_32(val, 0, which);
  }

  void Builder::nil_stack(int size, Value* nil) {
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

  void Builder::nil_locals() {
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


  void Builder::check_self_type() {
    int klass_id = 0;
    {
      if(Class* cls = try_as<Class>(info_.method()->scope()->module())) {
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

    CFGCalculator& cfg_;

  public:

    PassOne(LLVMState* ls, BlockMap& map, Function* func, BasicBlock* start,
            CFGCalculator& cfg)
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
      , cfg_(cfg)
    {
      JITBasicBlock& jbb = map_[0];
      jbb.reachable = true;
      jbb.block = start;

      current_block_ = &jbb;

      s_eval_ = ls->symbol("eval");
      s_binding_ = ls->symbol("binding");
      s_class_eval_ = ls->symbol("class_eval");
      s_module_eval_ = ls->symbol("module_eval");
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
    }

    const static int cUnknown = -10;
    const static bool cDebugStack = false;

#include "gen/instruction_effects.hpp"

    bool before(opcode op, opcode arg1=0, opcode arg2=0) {
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

        CFGBlock* cfg_block = cfg_.find_block(ip);
        assert(cfg_block);

        jbb.exception_type = cfg_block->exception_type();

        if(CFGBlock* handler = cfg_block->exception_handler()) {
          BlockMap::iterator hi = map_.find(handler->start_ip());
          assert(hi != map_.end());

          jbb.exception_handler = &hi->second;
        }

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

      break_at(which);

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

    void visit_zsuper(opcode which) {
      // HACK. zsuper accesses scope.
      calls_evalish_ = true;
      number_of_sends_++;
    }
  };

  void Builder::pass_one(BasicBlock* body) {
    CFGCalculator cfg(vmm_);
    cfg.build();

    // Pass 1, detect BasicBlock boundaries
    PassOne finder(ls_, block_map_, func, body, cfg);
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
      v_.dispatch(iter.ip());

      if(v_.b().GetInsertBlock()->getTerminator() == NULL) {
        BlockMap::iterator i = map_.find(iter.next_ip());
        if(i != map_.end()) {
          v_.b().CreateBr(i->second.block);
        }
      }
    }
  };

  bool Builder::generate_body() {
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
    visitor.set_stream(info_.vmm);

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

    info_.return_pad()->moveAfter(visitor.current_block());

    info_.fin_block = visitor.current_block();
    return true;
  }

  void Builder::generate_hard_return() {
    b().SetInsertPoint(info_.return_pad());
    b().CreateRet(info_.return_phi());
  }
}
}

#endif
