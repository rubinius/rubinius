#ifdef ENABLE_LLVM

#include "vmmethod.hpp"
#include "llvm/jit.hpp"

#include "builtin/fixnum.hpp"
#include "field_offset.hpp"

#include "call_frame.hpp"
#include "assembler/jit.hpp"
#include "configuration.hpp"

#include "instruments/profiler.hpp"

#include <llvm/Target/TargetData.h>
// #include <llvm/LinkAllPasses.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/CallingConv.h>

#include <sstream>

using namespace llvm;

namespace offset {
  const static int cf_previous = 0;
  const static int cf_static_scope = 1;
  const static int cf_msg = 2;
  const static int cf_cm = 3;
  const static int cf_flags = 4;
  const static int cf_ip = 5;
  const static int cf_top_scope = 6;
  const static int cf_scope = 7;
  const static int cf_stk = 8;

  const static int args_recv = 0;
  const static int args_block = 1;
  const static int args_total = 2;
  const static int args_ary = 3;

  const static int vars_block = 1;
  const static int vars_method = 2;
  const static int vars_module = 3;
  const static int vars_parent = 4;
  const static int vars_self = 5;
  const static int vars_num_locals = 6;
  const static int vars_tuple = 7;

  const static int tuple_full_size = 1;
  const static int tuple_field = 2;

  const static int cm_static_scope = 12;

  const static int msg_name = 0;
  const static int msg_module = 1;
  const static int msg_method = 2;
  const static int msg_method_missing = 3;

  const static int blockinv_flags = 0;
  const static int blockinv_self = 1;
  const static int blockinv_static_scope = 2;

  const static int blockenv_scope = 1;
  const static int blockenv_top_scope = 2;
  const static int blockenv_local_count = 3;
  const static int blockenv_method = 4;
  const static int blockenv_vmm = 5;
  const static int blockenv_execute = 6;
};

#include "llvm/jit_visit.hpp"

namespace autogen_types {
#include "llvm/types.cpp.gen"
}

namespace rubinius {

  LLVMState* LLVMState::get(STATE) {
    if(!state->shared.llvm_state) {
      state->shared.llvm_state = new LLVMState(state);
    }

    return state->shared.llvm_state;
  }

  void LLVMState::shutdown(STATE) {
    if(!state->shared.llvm_state) return;
    state->shared.llvm_state->shutdown_i();
  }

  void LLVMState::on_fork(STATE) {
    if(!state->shared.llvm_state) return;
    state->shared.llvm_state->on_fork_i();
  }

  void LLVMState::pause(STATE) {
    if(!state->shared.llvm_state) return;
    state->shared.llvm_state->pause_i();
  }

  void LLVMState::unpause(STATE) {
    if(!state->shared.llvm_state) return;
    state->shared.llvm_state->unpause_i();
  }

  const llvm::Type* LLVMState::ptr_type(std::string name) {
    std::string full_name = std::string("struct.rubinius::") + name;
    return PointerType::getUnqual(
        module_->getTypeByName(full_name.c_str()));
  }

  class BackgroundCompilerThread : public thread::Thread {
    enum State {
      cUnknown,
      cRunning,
      cPaused,
      cIdle
    };

    thread::Mutex mutex_;
    std::list<BackgroundCompileRequest*> pending_requests_;
    thread::Condition condition_;
    thread::Condition pause_condition_;

    LLVMState* ls_;
    bool show_machine_code_;

    State state;
    bool stop_;
    bool pause_;
    bool paused_;

  public:
    BackgroundCompilerThread(LLVMState* ls)
      : ls_(ls)
      , state(cUnknown)
      , stop_(false)
      , pause_(false)
      , paused_(false)
    {
      show_machine_code_ = ls->jit_dump_code() & cMachineCode;
    }

    void add(BackgroundCompileRequest* req) {
      thread::Mutex::LockGuard guard(mutex_);
      pending_requests_.push_back(req);
      condition_.signal();
    }

    void stop() {
      {
        thread::Mutex::LockGuard guard(mutex_);
        stop_ = true;

        if(state == cIdle) {
          condition_.signal();
        } else if(state == cPaused) {
          // TODO refactor common from unpause
          pause_ = false;
          condition_.signal();
        }
      }

      join();
    }

    void pause() {
      thread::Mutex::LockGuard guard(mutex_);

      // it's idle, ie paused.
      if(state == cIdle || state == cPaused) return;

      pause_ = true;

      while(!paused_) {
        pause_condition_.wait(mutex_);
      }
    }

    void unpause() {
      thread::Mutex::LockGuard guard(mutex_);

      // idle, just waiting for more work, ok, thats fine.
      if(state != cPaused) return;

      pause_ = false;

      condition_.signal();
    }

    void restart() {
      mutex_.init();
      condition_.init();
      pause_condition_.init();

      state = cUnknown;
      stop_ = false;
      pause_ = false;
      paused_ = false;

      run();
    }

    virtual void perform() {
      for(;;) { // forever

        BackgroundCompileRequest* req = 0;

        // Lock, wait, get a request, unlock
        {
          thread::Mutex::LockGuard guard(mutex_);

          if(pause_) {
            state = cPaused;

            paused_ = true;
            pause_condition_.signal();

            while(pause_) {
              condition_.wait(mutex_);
            }

            state = cUnknown;
            paused_ = false;
          }

          // If we've been asked to stop, do so now.
          if(stop_) return;

          while(pending_requests_.size() == 0) {
            state = cIdle;

            // unlock and wait...
            condition_.wait(mutex_);

            if(stop_) return;
          }

          // now locked again, shift a request
          req = pending_requests_.front();
          pending_requests_.pop_front();

          state = cRunning;
        }

        // mutex now unlock, allowing others to push more requests
        //


        LLVMCompiler* jit = new LLVMCompiler();

        {
          timer::Running timer(ls_->time_spent);
          jit->compile(ls_, req->vmmethod(), req->is_block());
          jit->generate_function(ls_);
        }

        if(show_machine_code_) {
          jit->show_machine_code();
        }

        // Ok, compiled, generated machine code, now update MachineMethod

        // Ok, now we are manipulating managed memory, so make
        // sure the GC doesn't run.
        ls_->shared().gc_dependent();

        req->vmmethod()->set_jitted(jit->llvm_function());

        if(req->is_block()) {
          BlockEnvironment* be = req->block_env();
          if(!be) {
            std::cout << "Fatal error in JIT. Expected a BlockEnvironment.\n";
          } else {
            be->set_native_function(jit->function_pointer());
          }
        } else {
          MachineMethod* mm = req->machine_method();
          if(!mm) {
            std::cout << "Fatal error in JIT. Expected a MachineMethod.\n";
          } else {
            mm->update(req->vmmethod(), jit);
            mm->activate();
          }
        }

        int which = ls_->add_jitted_method();
        if(ls_->config().jit_show_compiling) {
          std::cout << "[[[ JIT finished background compiling "
                    << which
                    << (req->is_block() ? " (block)" : " (method)")
                    << " ]]]\n";
        }

        delete req;

        // We don't depend on the GC here, so let it run independent
        // of us.
        ls_->shared().gc_independent();
      }
    }
  };

  namespace {
    void add_fast_passes(FunctionPassManager* passes) {
      // Eliminate unnecessary alloca.
      passes->add(createPromoteMemoryToRegisterPass());
      // Do simple "peephole" optimizations and bit-twiddling optzns.
      passes->add(createInstructionCombiningPass());
      // Reassociate expressions.
      passes->add(createReassociatePass());
      // Eliminate Common SubExpressions.
      passes->add(createGVNPass());
      passes->add(createDeadStoreEliminationPass());

      passes->add(createInstructionCombiningPass());

      passes->add(createScalarReplAggregatesPass());
      passes->add(createLICMPass());

      passes->add(createSCCPPass());
      passes->add(createInstructionCombiningPass());
      passes->add(createCondPropagationPass());

      // Simplify the control flow graph (deleting unreachable blocks, etc).
      passes->add(createCFGSimplificationPass());

      // passes->add(createGVNPass());
      // passes->add(createCFGSimplificationPass());
      passes->add(createDeadStoreEliminationPass());
      passes->add(createAggressiveDCEPass());
      // passes->add(createVerifierPass());
    }
  }

  void LLVMState::add_internal_functions() {
  }

  LLVMState::LLVMState(STATE)
    : config_(state->shared.config)
    , global_lock_(state->global_lock())
    , symbols_(state->symbols)
    , jitted_methods_(0)
    , queued_methods_(0)
    , accessors_inlined_(0)
    , uncommons_taken_(0)
    , shared_(state->shared)
    , include_profiling_(state->shared.config.jit_profile)
    , code_bytes_(0)
    , time_spent(0)
  {
    bool fast_code_passes = false;

    module_ = new llvm::Module("rubinius");

    autogen_types::makeLLVMModuleContents(module_);

    mp_ = new llvm::ExistingModuleProvider(module_);
    engine_ = ExecutionEngine::create(mp_);

    passes_ = new llvm::FunctionPassManager(mp_);
    passes_->add(new llvm::TargetData(*engine_->getTargetData()));

    if(fast_code_passes) {
      add_fast_passes(passes_);
    } else {
      // Eliminate unnecessary alloca.
      passes_->add(createPromoteMemoryToRegisterPass());
      // Do simple "peephole" optimizations and bit-twiddling optzns.
      passes_->add(createInstructionCombiningPass());
      // Reassociate expressions.
      passes_->add(createReassociatePass());
      // Eliminate Common SubExpressions.
      passes_->add(createGVNPass());
      passes_->add(createDeadStoreEliminationPass());

      passes_->add(createInstructionCombiningPass());

      // Simplify the control flow graph (deleting unreachable blocks, etc).
      passes_->add(createCFGSimplificationPass());

      // passes_->add(createGVNPass());
      // passes_->add(createCFGSimplificationPass());
      passes_->add(createDeadStoreEliminationPass());
      // passes_->add(createVerifierPass());
      passes_->add(createScalarReplAggregatesPass());
    }

    object_ = ptr_type("Object");

    profiling_ = new GlobalVariable(
        Type::Int1Ty,
        false,
        GlobalVariable::ExternalLinkage,
        0, "profiling_flag", module_);

    engine_->addGlobalMapping(profiling_,
        reinterpret_cast<void*>(state->shared.profiling_address()));

    add_internal_functions();

    background_thread_ = new BackgroundCompilerThread(this);
    background_thread_->run();
  }

  void LLVMState::shutdown_i() {
    background_thread_->stop();
  }

  void LLVMState::on_fork_i() {
    background_thread_->restart();
  }

  void LLVMState::pause_i() {
    background_thread_->pause();
  }

  void LLVMState::unpause_i() {
    background_thread_->unpause();
  }

  Symbol* LLVMState::symbol(const char* sym) {
    return symbols_.lookup(sym);
  }

  const char* LLVMState::symbol_cstr(const Symbol* sym) {
    return symbols_.lookup_cstring(sym);
  }

  void LLVMState::compile_soon(STATE, VMMethod* vmm, BlockEnvironment* block) {
    Object* placement;
    bool is_block = false;

    if(block) {
      is_block = true;
      placement = block;
    } else {
      placement = state->new_struct<MachineMethod>(G(machine_method));
    }

    BackgroundCompileRequest* req =
      new BackgroundCompileRequest(state, vmm, placement, is_block);

    queued_methods_++;

    background_thread_->add(req);
  }

  void LLVMState::remove(llvm::Function* func) {
    // Deallocate the JITed code
    engine_->freeMachineCodeForFunction(func);

    // Nuke the Function from the module
    func->replaceAllUsesWith(UndefValue::get(func->getType()));
    func->eraseFromParent();
  }

  static Value* get_field(BasicBlock* block, Value* val, int which) {
    Value* idx[] = {
      ConstantInt::get(Type::Int32Ty, 0),
      ConstantInt::get(Type::Int32Ty, which)
    };
    Value* gep = GetElementPtrInst::Create(val, idx, idx+2, "gep", block);
    return gep;
  }

  class BlockFinder : public VisitInstructions<BlockFinder> {
    BlockMap& map_;
    Function* function_;
    opcode current_ip_;
    int force_break_;
    bool creates_blocks_;
    int number_of_sends_;
    bool loops_;

  public:

    BlockFinder(BlockMap& map, Function* func)
      : map_(map)
      , function_(func)
      , current_ip_(0)
      , force_break_(false)
      , creates_blocks_(false)
      , number_of_sends_(0)
      , loops_(false)
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
      if(force_break_) {
        break_at(ip);
        force_break_ = false;
      }
    }

    void break_at(int ip) {
      BlockMap::iterator i = map_.find(ip);
      if(i == map_.end()) {
        std::ostringstream ss;
        ss << "ip" << ip;
        map_[ip] = BasicBlock::Create(ss.str().c_str(), function_);
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


  Value* constant(Object* obj, const Type* obj_type, BasicBlock* block) {
    return CastInst::Create(
        Instruction::IntToPtr,
        ConstantInt::get(Type::Int32Ty, (intptr_t)obj),
        obj_type, "cast_to_obj", block);
  }

  class LLVMWorkHorse {
  public:
    LLVMState* ls_;
    const Type* cf_type;
    const Type* vars_type;
    const Type* obj_type;
    const Type* obj_ary_type;
    Function* func;

    Value* vm;
    Value* prev;
    Value* msg;
    Value* args;
    Value* block_env;
    Value* block_inv;
    Value* top_scope;

    BasicBlock* block;

    Value* call_frame;
    Value* stk;
    Value* vars;

    Value* stack_top;
    Value* method_entry_;
    Value* method;

  public:
    LLVMWorkHorse(LLVMState* ls)
      : ls_(ls)
    {
      llvm::Module* mod = ls->module();
      cf_type = mod->getTypeByName("struct.rubinius::CallFrame");
      vars_type = mod->getTypeByName("struct.rubinius::VariableScope");
      obj_type = ls->ptr_type("Object");
      obj_ary_type = PointerType::getUnqual(obj_type);
    }

    void return_value(Value* ret, BasicBlock* cont = 0) {
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

    void initialize_call_frame(int stack_size) {
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

    void initialize_block_frame(int stack_size) {
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

    void nil_stack(int size, Value* nil) {
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

    void nil_locals(VMMethod* vmm) {
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

          Value* gep = GetElementPtrInst::Create(vars, idx, idx+3, "stack_pos", block);
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

      Value* gep = GetElementPtrInst::Create(vars, idx, idx+3, "stack_pos", top);
      new StoreInst(nil, gep, top);

      Value* added = BinaryOperator::CreateAdd(cur, one, "added", top);
      new StoreInst(added, counter, top);

      Value* cmp = new ICmpInst(ICmpInst::ICMP_EQ, added, max, "loop_check", top);
      BranchInst::Create(cont, top, cmp, top);

      block = cont;
    }

    void setup_scope(VMMethod* vmm) {
      Value* flag_idx[] = {
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, 0)
      };

      Value* flag_pos = GetElementPtrInst::Create(vars, flag_idx, flag_idx+5,
          "flag_pos", block);

      new StoreInst(ConstantInt::get(Type::Int32Ty, 0), flag_pos, false, block);

      Value* self = new LoadInst(get_field(block, args, offset::args_recv),
                                 "args.recv", block);
      new StoreInst(self, get_field(block, vars, offset::vars_self), false, block);
      new StoreInst(method, get_field(block, vars, offset::vars_method),
                    false, block);
      Value* mod = new LoadInst(get_field(block, msg, offset::msg_module),
                                "msg.module", block);
      new StoreInst(mod, get_field(block, vars, offset::vars_module), false, block);

      Value* blk = new LoadInst(get_field(block, args, offset::args_block),
                                "args.block", block);
      new StoreInst(blk, get_field(block, vars, offset::vars_block), false, block);

      Value* locals = ConstantInt::get(Type::Int32Ty, vmm->number_of_locals);
      new StoreInst(locals, get_field(block, vars, offset::vars_num_locals),
                    false, block);

      new StoreInst(Constant::getNullValue(vars->getType()),
                    get_field(block, vars, offset::vars_parent), false, block);

      nil_locals(vmm);
    }

    void setup_block_scope(VMMethod* vmm) {
      Value* flag_idx[] = {
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, 0)
      };

      Value* flag_pos = GetElementPtrInst::Create(vars, flag_idx, flag_idx+5,
          "flag_pos", block);

      new StoreInst(ConstantInt::get(Type::Int32Ty, 0), flag_pos, false, block);

      Value* self = new LoadInst(
          get_field(block, block_inv, offset::blockinv_self),
          "invocation.self", block);

      new StoreInst(self, get_field(block, vars, offset::vars_self), false, block);
      new StoreInst(method, get_field(block, vars, offset::vars_method),
                    false, block);

      Value* mod = new LoadInst(get_field(block, top_scope, offset::vars_module),
                                "top_scope.module", block);
      new StoreInst(mod, get_field(block, vars, offset::vars_module), false, block);

      Value* blk = new LoadInst(get_field(block, top_scope, offset::vars_block),
                                "args.block", block);
      new StoreInst(blk, get_field(block, vars, offset::vars_block), false, block);

      Value* locals = ConstantInt::get(Type::Int32Ty, vmm->number_of_locals);
      new StoreInst(locals, get_field(block, vars, offset::vars_num_locals),
                    false, block);

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

    void import_args(VMMethod* vmm) {
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

    void setup_block(VMMethod* vmm) {
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
            (sizeof(VariableScope) / sizeof(Object*)) + vmm->number_of_locals),
          "var_mem", block);

      vars = CastInst::Create(
          Instruction::BitCast,
          var_mem,
          PointerType::getUnqual(vars_type), "vars", block);

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

    void setup(VMMethod* vmm) {
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
            (sizeof(VariableScope) / sizeof(Object*)) + vmm->number_of_locals),
          "var_mem", block);

      vars = CastInst::Create(
          Instruction::BitCast,
          var_mem,
          PointerType::getUnqual(vars_type), "vars", block);

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
  };

  void LLVMCompiler::compile(LLVMState* ls, VMMethod* vmm, bool is_block) {
    LLVMWorkHorse work(ls);

    if(is_block) {
      work.setup_block(vmm);
    } else {
      work.setup(vmm);
    }

    llvm::Function* func = work.func;

    JITVisit visitor(ls, vmm, ls->module(), func,
                     work.block, work.stk, work.call_frame, work.stack_top,
                     work.method_entry_, work.args,
                     work.vars, is_block);

    // Pass 1, detect BasicBlock boundaries
    BlockFinder finder(visitor.block_map(), func);
    finder.drive(vmm);

    // DISABLED: This still has problems.
    // visitor.set_creates_blocks(finder.creates_blocks());

    if(finder.number_of_sends() > 0 || finder.loops_p()) {
      // Check for interrupts at the top
      visitor.visit_check_interrupts();
    }

    // Pass 2, compile!
    try {
      visitor.drive(vmm->opcodes, vmm->total);
    } catch(JITVisit::Unsupported &e) {
      function_ = NULL;
      // This is too noisy to report
      // std::cout << "not supported yet.\n";
      return;
    }

    if(ls->jit_dump_code() & cSimple) {
      std::cout << "[[[ LLVM Simple IR ]]]\n";
      std::cout << *func << "\n";
    }

    if(llvm::verifyFunction(*func, PrintMessageAction)) {
      std::cout << "ERROR: complication error detected.\n";
      std::cout << "ERROR: Please report the above message and the\n";
      std::cout << "       code below to http://github.com/evanphx/rubinius/issues\n";
      std::cout << *func << "\n";
      function_ = NULL;
      return;
    }

    ls->passes()->run(*func);

    if(ls->jit_dump_code() & cOptimized) {
      std::cout << "[[[ LLVM Optimized IR ]]]\n";
      std::cout << *func << "\n";
    }

    function_ = func;
  }

  void* LLVMCompiler::function_pointer(STATE) {
    if(!mci_) {
      if(!function_) return NULL;
      mci_ = new llvm::MachineCodeInfo();
      LLVMState* ls = LLVMState::get(state);
      ls->engine()->runJITOnFunction(function_, mci_);

      if(state->shared.config.jit_dump_code & cMachineCode) {
        std::cout << "[[[ JIT Machine Code ]]]\n";
        assembler_x86::AssemblerX86::show_buffer(mci_->address(), mci_->size(), false, NULL);
      }

      ls->add_code_bytes(mci_->size());
    }

    return mci_->address();
  }

  void* LLVMCompiler::function_pointer() {
    if(!mci_) return NULL;
    return mci_->address();
  }

  void LLVMCompiler::show_machine_code() {
    std::cout << "[[[ JIT Machine Code ]]]\n";
    assembler_x86::AssemblerX86::show_buffer(mci_->address(), mci_->size(), false, NULL);
  }

  void* LLVMCompiler::generate_function(LLVMState* ls) {
    if(!mci_) {
      if(!function_) return NULL;
      mci_ = new llvm::MachineCodeInfo();
      ls->engine()->runJITOnFunction(function_, mci_);
      ls->add_code_bytes(mci_->size());
    }

    return mci_->address();
  }

  llvm::Function* LLVMCompiler::llvm_function(STATE) {
    return function_;
  }

  void LLVMCompiler::show_assembly(STATE) {
    if(function_) {
      std::cout << *function_ << "\n";
      std::cout << "\n== x86 assembly ==\n";

      // Force it to be compiled
      function_pointer(state);
      assembler_x86::AssemblerX86::show_buffer(mci_->address(), mci_->size(), false, NULL);
    } else {
      std::cout << "NULL function!\n";
    }
  }

}

#endif
