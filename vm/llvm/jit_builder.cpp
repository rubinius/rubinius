#ifdef ENABLE_LLVM

#include "llvm/jit_builder.hpp"
#include "call_frame.hpp"
#include "machine_code.hpp"

#include "llvm/jit_context.hpp"
#include "llvm/jit_visit.hpp"
#include "llvm/control_flow.hpp"
#include "llvm/cfg.hpp"

#include "instruments/tooling.hpp"
#include <llvm/Analysis/CaptureTracking.h>
#if RBX_LLVM_API_VER > 301
#include <llvm/DebugInfo.h>
#else
#include <llvm/Analysis/DebugInfo.h>
#endif

// There is no official language identifier for Ruby.
// Define it as an user-defined one.
// Check that this constant isn't conflicted any other recognized user-defined
// language identifier:
//   http://llvm.org/docs/doxygen/html/namespacellvm_1_1dwarf.html#a85bda042c02722848a3411b67924eb47a8f16e9a8f3582985075ffd88f363d616
#define DW_LANG_Ruby (llvm::dwarf::DW_LANG_lo_user + 2)

namespace rubinius {
namespace jit {

  Builder::Builder(Context* ctx, JITMethodInfo& i)
    : ctx_(ctx)
    , machine_code_(i.machine_code)
    , builder_(ctx->llvm_context(), llvm::ConstantFolder(), IRBuilderInserterWithDebug(this))
    , use_full_scope_(false)
    , import_args_(0)
    , body_(0)
    , info_(i)
    , runtime_data_(0)
    , debug_builder_(*ctx->module())
  {
    llvm::Module* mod = ctx->module();
    cf_type = mod->getTypeByName("struct.rubinius::CallFrame");
    vars_type = mod->getTypeByName("struct.rubinius::VariableScope");
    stack_vars_type = mod->getTypeByName("struct.rubinius::StackVariables");
    obj_type = ctx->ptr_type("Object");
    obj_ary_type = llvm::PointerType::getUnqual(obj_type);
    check_global_interrupts_pos = b().CreateIntToPtr(
          llvm::ConstantInt::get(ctx_->IntPtrTy, (intptr_t)ctx_->llvm_state()->shared().check_global_interrupts_address()),
          llvm::PointerType::getUnqual(ctx_->Int8Ty), "cast_to_intptr");
    set_definition_location();
  }

  void Builder::set_definition_location() {
    std::string file_str = ctx_->llvm_state()->symbol_debug_str(info_.method()->file());
    debug_builder_.createCompileUnit(DW_LANG_Ruby, file_str,
        "", "rubinius", true, "", 0);
    DIFile file = debug_builder().createFile(file_str, "");

#if RBX_LLVM_API_VER > 303
    DIType dummy_return_type = debug_builder().createNullPtrType();
    Value* dummy_signature[] = {
      &*dummy_return_type,
    };
    DICompositeType dummy_subroutine_type = debug_builder().createSubroutineType(file,
        debug_builder().getOrCreateArray(dummy_signature));
#else
    DIType dummy_return_type = debug_builder().createNullPtrType("dummy type");
    Value* dummy_signature[] = {
      &*dummy_return_type,
    };
    DIType dummy_subroutine_type = debug_builder().createSubroutineType(file,
        debug_builder().getOrCreateArray(dummy_signature));
#endif

#if RBX_LLVM_API_VER > 300
    DISubprogram subprogram = debug_builder().createFunction(file, "", "",
        file, info_.method()->start_line(), dummy_subroutine_type, false, true, 0, 0,
        false, info_.function());
#else
    DISubprogram subprogram = debug_builder().createFunction(file, "", "",
        file, info_.method()->start_line(), dummy_subroutine_type, false, true, 0,
        false, info_.function());
#endif
    // FnSpecificMDNode is used in finalize(), so to avoid memory leak,
    // initialize it with empty one by inserting here.
    llvm::getOrInsertFnSpecificMDNode(*ctx_->module(), subprogram);

    b().SetCurrentDebugLocation(llvm::DebugLoc::get(info_.method()->start_line(), 0,
                                subprogram));
    debug_builder_.finalize();
  }

  void Builder::set_current_location(opcode ip) {
    int line = info_.method()->line(ip);
    DISubprogram subprogram(b().getCurrentDebugLocation().getScope(ctx_->llvm_context()));
    b().SetCurrentDebugLocation(llvm::DebugLoc::get(line, 0, subprogram));
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

    Value* max = cint(size);
    Value* one = cint(1);

    BasicBlock* top = info_.new_block("stack_nil");
    BasicBlock* cont = info_.new_block("bottom");

    b().CreateStore(cint(0), info_.counter());

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
    Value* nil = constant(cNil, obj_type);
    int size = machine_code_->number_of_locals;

    if(size == 0) return;
    // Stack size 5 or less, do 5 stores in a row rather than
    // the loop.
    if(size <= 5) {
      for(int i = 0; i < size; i++) {
        Value* idx[] = {
          cint(0),
          cint(offset::StackVariables::locals),
          cint(i)
        };

        Value* gep = b().CreateGEP(vars, idx, "local_pos");
        b().CreateStore(nil, gep);
      }
      return;
    }

    Value* max = cint(size);
    Value* one = cint(1);

    BasicBlock* top = info_.new_block("locactx_nil");
    BasicBlock* cont = info_.new_block("bottom");

    b().CreateStore(cint(0), info_.counter());

    b().CreateBr(top);

    b().SetInsertPoint(top);

    Value* cur = b().CreateLoad(info_.counter(), "counter");
    Value* idx[] = {
      cint(0),
      cint(offset::StackVariables::locals),
      cur
    };

    Value* gep = b().CreateGEP(vars, idx, "local_pos");
    b().CreateStore(nil, gep);

    Value* added = b().CreateAdd(cur, one, "added");
    b().CreateStore(added, info_.counter());

    Value* cmp = b().CreateICmpEQ(added, max, "loop_check");
    b().CreateCondBr(cmp, cont, top);

    b().SetInsertPoint(cont);
  }

  class PassOne : public VisitInstructions<PassOne> {
    Context* ctx_;
    BlockMap& map_;
    Function* function_;
    opcode current_ip_;
    int force_break_;
    bool creates_blocks_;
    int number_of_sends_;
    bool loops_;
    int sp_;
    JITBasicBlock* current_block_;
    bool calctx_evalish_;

    Symbol* s_eval_;
    Symbol* s_binding_;
    Symbol* s_class_eval_;
    Symbol* s_module_eval_;

    CFGCalculator& cfg_;
    JITMethodInfo& info_;

  public:

    PassOne(Context* ctx, BlockMap& map, Function* func, BasicBlock* start,
            CFGCalculator& cfg, JITMethodInfo& info)
      : ctx_(ctx)
      , map_(map)
      , function_(func)
      , current_ip_(0)
      , force_break_(false)
      , creates_blocks_(false)
      , number_of_sends_(0)
      , loops_(false)
      , sp_(-1)
      , calctx_evalish_(false)
      , cfg_(cfg)
      , info_(info)
    {
      JITBasicBlock& jbb = map_[0];
      jbb.reachable = true;
      jbb.block = start;

      current_block_ = &jbb;

      s_eval_ = ctx->llvm_state()->symbol("eval");
      s_binding_ = ctx->llvm_state()->symbol("binding");
      s_class_eval_ = ctx->llvm_state()->symbol("class_eval");
      s_module_eval_ = ctx->llvm_state()->symbol("module_eval");
    }

    bool calctx_evalish() {
      return calctx_evalish_;
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
        jbb.block = BasicBlock::Create(ctx_->llvm_context(), ss.str(), function_);
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
      CallSite* call_site = reinterpret_cast<CallSite*>(which);
      if(call_site->name() == s_eval_ ||
         call_site->name() == s_binding_ ||
         call_site->name() == s_class_eval_ ||
         call_site->name() == s_module_eval_) {
        calctx_evalish_ = true;
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
      calctx_evalish_ = true;
      number_of_sends_++;
    }

    void visit_push_local(opcode which) {
      LocalInfo* li = info_.get_local(which);
      li->inc_push();
    }

    void visit_set_local(opcode which) {
      LocalInfo* li = info_.get_local(which);
      li->inc_set();
    }
  };

  void Builder::pass_one(BasicBlock* body) {
    CFGCalculator cfg(machine_code_);
    cfg.build();

    // Pass 1, detect BasicBlock boundaries
    PassOne finder(ctx_, block_map_, info_.function(), body, cfg, info_);
    finder.drive(machine_code_);

    if(finder.creates_blocks() || finder.calctx_evalish()) {
      info_.set_use_full_scope();
      use_full_scope_ = true;
    }

    number_of_sends_ = finder.number_of_sends();
    loops_ = finder.loops_p();
  }

  class Walker {
    JITVisit& v_;
    BlockMap& map_;
    Builder& builder_;

  public:
    Walker(JITVisit& v, BlockMap& map, Builder& builder)
      : v_(v)
      , map_(map)
      , builder_(builder)
    {}

    void call(OpcodeIterator& iter) {
      builder_.set_current_location(iter.ip());
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
    JITVisit visitor(this, info_, block_map_, b().GetInsertBlock());

    if(info_.inline_policy) {
      visitor.set_policy(info_.inline_policy);
    } else {
      visitor.init_policy(ctx_);
    }

    assert(visitor.inline_policy());

    visitor.set_called_args(info_.called_args);

    visitor.set_valid_flag(valid_flag);

    if(use_full_scope_) visitor.use_full_scope();

    visitor.initialize();
    visitor.set_stream(info_.machine_code);

    // Pass 2, compile!
    // Drive by following the control flow.
    jit::ControlFlowWalker walker(info_.machine_code);
    Walker cb(visitor, block_map_, *this);

    try {
      walker.run<Walker>(cb);
    } catch(JITVisit::Unsupported &e) {
      return false;
    }

    // See if we should check interrupts now
    if(!info_.is_block && body_ && (visitor.sends_done() > 2 || loops_)) {
      BasicBlock* cur = b().GetInsertBlock();

      // Remove the branch to method_body
      import_args_->back().eraseFromParent();

      b().SetInsertPoint(import_args_);

      BasicBlock* ret_null = info_.new_block("ret_null");

      Value* idx_jit[] = {
        cint(0),
        cint(offset::State::vm_jit)
      };
      Value* vm_jit = b().CreateLoad(b().CreateGEP(info_.state(), idx_jit), "vm_jit");

      // Check stack overflow
      Value* idx_stk_limit[] = {
        cint(0),
        cint(offset::VMJIT::stack_limit)
      };

      Value* stack_limit = b().CreateLoad(b().CreateGEP(vm_jit, idx_stk_limit),
                                               "stack_limit");
      Value* stack_end = b().CreatePtrToInt(
        call_frame, ctx_->IntPtrTy, "stack_end");

      Value* stack_overflow = b().CreateICmpULT(stack_end, stack_limit);

      // Check local interrupts (signals, thread step etc.)
      Value* idx_interrupts[] = {
        cint(0),
        cint(offset::VMJIT::check_local_interrupts)
      };

      Value* check_interrupts = b().CreateLoad(b().CreateGEP(vm_jit, idx_interrupts),
                                               "check_interrupts");

      // Check global interrupts (GC, stop the world etc.)
      Value* check_global_interrupts = b().CreateLoad(check_global_interrupts_pos, "check_global_interrupts");

      Value* interrupts = b().CreateOr(check_interrupts, check_global_interrupts, "has_interrupts");

      Value* zero = ConstantInt::get(ctx_->Int8Ty, 0);
      Value* is_zero = b().CreateICmpNE(interrupts, zero, "needs_interrupts");

      Value* check = b().CreateOr(is_zero, stack_overflow, "needs_check");

      BasicBlock* prologue_check = info_.new_block("prologue_check");

      b().CreateCondBr(check, prologue_check, body_);
      b().SetInsertPoint(prologue_check);

      Signature sig(ctx_, obj_type);
      sig << "State";
      sig << "CallFrame";

      Value* call_args[] = { info_.state(), call_frame };

      Value* ret = sig.call("rbx_prologue_check", call_args, 2, "ci", b());
      b().CreateCondBr(
          b().CreateICmpEQ(ret, Constant::getNullValue(obj_type)),
          ret_null, body_);

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

  void Builder::alloc_frame(const char* body_name) {
    info_.setup_return();

    body_ = info_.new_block(body_name);

    pass_one(body_);

    info_.context()->init_variables(b());

    counter2_ = b().CreateAlloca(ctx_->Int32Ty, 0, "counter2");

    valid_flag = b().CreateAlloca(ctx_->Int1Ty, 0, "valid_flag");

    Value* cfstk = b().CreateAlloca(obj_type,
        cint((sizeof(CallFrame) / sizeof(Object*)) + machine_code_->stack_size),
        "cfstk");

    Value* var_mem = b().CreateAlloca(obj_type,
        cint((sizeof(StackVariables) / sizeof(Object*)) + machine_code_->number_of_locals),
        "var_mem");

    call_frame = b().CreateBitCast(
        cfstk,
        llvm::PointerType::getUnqual(cf_type), "call_frame");

    if(ctx_->llvm_state()->include_profiling()) {
      method_entry_ = b().CreateAlloca(ctx_->Int8Ty,
          cint(sizeof(tooling::MethodEntry)),
          "method_entry");

      info_.set_profiling_entry(method_entry_);
    }

    info_.set_call_frame(call_frame);

    stk = b().CreateConstGEP1_32(cfstk, sizeof(CallFrame) / sizeof(Object*), "stack");

    info_.set_stack(stk);

    vars = b().CreateBitCast(
        var_mem,
        llvm::PointerType::getUnqual(stack_vars_type), "vars");

    info_.set_variables(vars);

  }
}
}

#endif
