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

#include <sstream>

using namespace llvm;

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

          // If we've been asked to stop, do so now.
          if(stop_) return;

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
        jit->compile(ls_, req->vmmethod());
        jit->generate_function(ls_);

        if(show_machine_code_) {
          jit->show_machine_code();
        }

        // Ok, compiled, generated machine code, now update MachineMethod

        // Ok, now we are manipulating managed memory, so make
        // sure the GC doesn't run.
        ls_->shared().gc_dependent();

        req->vmmethod()->set_jitted();

        MachineMethod* mm = req->machine_method();
        mm->update(req->vmmethod(), jit);
        mm->activate();

        if(ls_->config().jit_show_compiling) {
          std::cout << "[[[ JIT finished background compiling "
                    << ls_->add_jitted_method()
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

  LLVMState::LLVMState(STATE)
    : config_(state->shared.config)
    , global_lock_(state->global_lock())
    , symbols_(state->symbols)
    , shared_(state->shared)
    , include_profiling_(state->shared.config.jit_profile)
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
    }

    object_ = ptr_type("Object");

    profiling_ = new GlobalVariable(
        Type::Int1Ty,
        false,
        GlobalVariable::ExternalLinkage,
        0, "profiling_flag", module_);

    engine_->addGlobalMapping(profiling_,
        reinterpret_cast<void*>(state->shared.profiling_address()));

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

  void LLVMState::compile_soon(STATE, VMMethod* vmm) {
    MachineMethod* mm = state->new_struct<MachineMethod>(G(machine_method));

    BackgroundCompileRequest* req = new BackgroundCompileRequest(state, vmm, mm);

    queued_methods_++;

    background_thread_->add(req);
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
    int current_ip_;
    int force_break_;
    bool creates_blocks_;

  public:

    BlockFinder(BlockMap& map, Function* func)
      : map_(map)
      , function_(func)
      , current_ip_(0)
      , force_break_(false)
      , creates_blocks_(false)
    {}

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
      break_at(which);
      next_ip_too();
    }

    void visit_goto_if_true(opcode which) {
      break_at(which);
    }

    void visit_goto_if_false(opcode which) {
      break_at(which);
    }

    void visit_goto_if_defined(opcode which) {
      break_at(which);
    }

    void visit_setup_unwind(opcode which, opcode type) {
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

    BasicBlock* block;

    Value* call_frame;
    Value* stk;
    Value* vars;

    Value* stack_top;
    Value* method_entry_;

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
      Value* cm_gep = get_field(block, call_frame, 3);
      Value* meth = CastInst::Create(
          Instruction::BitCast,
          exec,
          cast<PointerType>(cm_gep->getType())->getElementType(),
          "cm", block);

      // previous
      new StoreInst(prev, get_field(block, call_frame, 0), false, block);

      // msg
      new StoreInst(msg, get_field(block, call_frame, 2), false, block);

      // cm
      new StoreInst(meth, cm_gep, false, block);

      // flags
      new StoreInst(ConstantInt::get(Type::Int32Ty, 0),
          get_field(block, call_frame, 4), false, block);

      // args
      Value* total = new LoadInst(get_field(block, args, 2), "args.total", block);
      new StoreInst(total, get_field(block, call_frame, 5), false, block);

      // ip
      new StoreInst(ConstantInt::get(Type::Int32Ty, 0),
          get_field(block, call_frame, 6), false, block);

      // scope
      new StoreInst(vars, get_field(block, call_frame, 8), false, block);

      // stk
      new StoreInst(stk, get_field(block, call_frame, 9), false, block);

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
          meth
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

    void import_args(VMMethod* vmm) {
      Value* vm_obj = vm;
      Value* dis_obj = msg;
      Value* arg_obj = args;

      // Check the argument count
      Value* total_idx[] = {
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, 2),
      };

      Value* total_offset = GetElementPtrInst::Create(arg_obj, total_idx,
          total_idx + 2, "total_pos", block);
      Value* total = new LoadInst(total_offset, "arg.total", block);

      BasicBlock* arg_error = BasicBlock::Create("arg_error", func);
      BasicBlock* cont = BasicBlock::Create("method_body", func);

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
        call_frame,
        dis_obj,
        arg_obj,
        ConstantInt::get(Type::Int32Ty, vmm->required_args)
      };

      Value* val = sig.call("rbx_arg_error", call_args, 5, "ret", block);
      return_value(val);

      // Switch to using continuation
      block = cont;

      // Prepare the scope
      Signature sig2(ls_, "Object");

      sig2 << "VM";
      sig2 << "VariableScope";
      sig2 << "CallFrame";
      sig2 << "Dispatch";
      sig2 << "Arguments";

      Value* call_args2[] = {
        vm_obj,
        vars,
        call_frame,
        dis_obj,
        arg_obj
      };

      sig2.call("rbx_setup_scope", call_args2, 5, "", block);

      // Import the arguments
      Value* idx1[] = {
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, 3),
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
            ConstantInt::get(Type::Int32Ty, 8),
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
          ConstantInt::get(Type::Int32Ty, 8),
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
        sig << "CallFrame";
        sig << "Arguments";
        sig << Type::Int32Ty;

        Value* call_args[] = {
          vm_obj,
          call_frame,
          arg_obj,
          ConstantInt::get(Type::Int32Ty, vmm->total_args)
        };

        Value* splat_val = sig.call("rbx_construct_splat", call_args, 4, "splat_val", block);

        Value* idx3[] = {
          ConstantInt::get(Type::Int32Ty, 0),
          ConstantInt::get(Type::Int32Ty, 8),
          ConstantInt::get(Type::Int32Ty, vmm->splat_position)
        };

        Value* pos = GetElementPtrInst::Create(vars, idx3, idx3+3, "splat_pos", block);
        new StoreInst(splat_val, pos, false, block);
      }
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

      call_frame = new AllocaInst(cf_type, 0, "call_frame", block);
      stk = new AllocaInst(obj_type,
          ConstantInt::get(Type::Int32Ty, vmm->stack_size),
          "stack", block);

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
    }
  };

  void LLVMCompiler::compile(LLVMState* ls, VMMethod* vmm) {
    LLVMWorkHorse work(ls);

    work.setup(vmm);

    llvm::Function* func = work.func;

    JITVisit visitor(ls, vmm, ls->module(), func,
                     work.block, work.stk, work.call_frame, work.stack_top,
                     work.method_entry_);

    // Pass 1, detect BasicBlock boundaries
    BlockFinder finder(visitor.block_map(), func);
    finder.drive(vmm);

    // Check for interrupts at the top of every method...
    visitor.visit_check_interrupts();

    // Pass 2, compile!
    try {
      visitor.drive(vmm->opcodes, vmm->total);
    } catch(JITVisit::Unsupported &e) {
      function_ = NULL;
      std::cout << "not supported yet.\n";
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
      LLVMState::get(state)->engine()->runJITOnFunction(function_, mci_);

      if(state->shared.config.jit_dump_code & cMachineCode) {
        std::cout << "[[[ JIT Machine Code ]]]\n";
        assembler_x86::AssemblerX86::show_buffer(mci_->address(), mci_->size(), false, NULL);
      }
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
