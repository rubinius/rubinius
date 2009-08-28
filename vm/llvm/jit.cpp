#ifdef ENABLE_LLVM

#include "vmmethod.hpp"
#include "llvm/jit.hpp"

#include "builtin/fixnum.hpp"
#include "builtin/staticscope.hpp"
#include "builtin/module.hpp"
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
#include <llvm/Support/CFG.h>
#include <llvm/Analysis/Passes.h>

#include <sstream>

using namespace llvm;

#include "llvm/jit_workhorse.hpp"
#include "llvm/passes.hpp"
#include "instructions_util.hpp"

template <typename T>
static Value* constant(T obj, const Type* obj_type, BasicBlock* block) {
  return CastInst::Create(
      Instruction::IntToPtr,
      ConstantInt::get(Type::Int32Ty, (intptr_t)obj),
      obj_type, "cast_to_obj", block);
}


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

        req->vmmethod()->set_jitted(jit->llvm_function(),
                                    jit->code_bytes(),
                                    jit->function_pointer());

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
      passes->add(create_overflow_folding_pass());
      passes->add(create_guard_eliminator_pass());
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

      passes_->add(create_rubinius_alias_analysis());
      passes_->add(createGVNPass());
      // passes_->add(createCFGSimplificationPass());
      passes_->add(createDeadStoreEliminationPass());
      // passes_->add(createVerifierPass());
      passes_->add(createScalarReplAggregatesPass());

      passes_->add(create_overflow_folding_pass());
      passes_->add(create_guard_eliminator_pass());

      passes_->add(createCFGSimplificationPass());
      passes_->add(createInstructionCombiningPass());
      passes_->add(createScalarReplAggregatesPass());
      passes_->add(createDeadStoreEliminationPass());
      passes_->add(createCFGSimplificationPass());
      passes_->add(createInstructionCombiningPass());
    }

    passes_->doInitialization();

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

    // Ignore it!
    if(vmm->call_count < 0) return;
    vmm->call_count = -1;

    if(block) {
      is_block = true;
      placement = block;
    } else {
      placement = state->new_struct<MachineMethod>(G(machine_method));
    }

    state->stats.jitted_methods++;

    BackgroundCompileRequest* req =
      new BackgroundCompileRequest(state, vmm, placement, is_block);

    queued_methods_++;

    background_thread_->add(req);

    if(state->shared.config.jit_show_compiling) {
      std::cout << "[[[ JIT Queued"
                << (block ? " block " : " method ")
                << queued_methods() << "/"
                << jitted_methods() << " ]]]\n";
    }
  }

  void LLVMState::remove(llvm::Function* func) {
    // Deallocate the JITed code
    engine_->freeMachineCodeForFunction(func);

    // Nuke the Function from the module
    func->replaceAllUsesWith(UndefValue::get(func->getType()));
    func->eraseFromParent();
  }

  const static int cInlineMaxDepth = 8;

  VMMethod* LLVMState::find_candidate(VMMethod* start, CallFrame* call_frame) {
    VMMethod* last = start;
    int depth = 0;

    // No upper call_frames or generic inlining is off, use the start.
    // With generic inlining off, there is no way to inline back to start,
    // so we don't both trying.
    if(!call_frame || !config_.jit_inline_generic) return last;

    VMMethod* cur = call_frame->cm->backend_method();
    while(depth < cInlineMaxDepth) {
      if(cur->required_args != cur->total_args ||
          cur->call_count < 200 ||
          cur->jitted() ||
          cur->total < 10) break;

      call_frame = call_frame->previous;
      if(!call_frame) break;

      cur = call_frame->cm->backend_method();

      // Jump to defining methods of blocks!
      if(VMMethod* parent = cur->parent()) {
        cur = parent;
      } else {
        last = cur;
      }

      depth++;
    }

    return last;
  }

  void LLVMCompiler::compile(LLVMState* ls, VMMethod* vmm, bool is_block) {
    if(ls->config().jit_inline_debug) {
      if(is_block) {
        VMMethod* parent = vmm->parent();
        assert(parent);

        std::cerr << "JIT: compiling block in "
                  << ls->symbol_cstr(parent->original->scope()->module()->name())
                  << "#"
                  << ls->symbol_cstr(vmm->original->name())
                  << " near "
                  << ls->symbol_cstr(vmm->original->file()) << ":"
                  << vmm->original->start_line() << "\n";
      } else {
        std::cerr << "JIT: compiling "
                  << ls->symbol_cstr(vmm->original->scope()->module()->name())
                  << "#"
                  << ls->symbol_cstr(vmm->original->name()) << "\n";
      }
    }

    JITMethodInfo info(vmm);
    info.is_block = is_block;

    LLVMWorkHorse work(ls, info);

    if(is_block) {
      work.setup_block();
    } else {
      work.setup();
    }

    llvm::Function* func = info.function();

    if(!work.generate_body()) {
      function_ = NULL;
      // This is too noisy to report
      // std::cout << "not supported yet.\n";
      return;
    }

    if(ls->jit_dump_code() & cSimple) {
      std::cout << "[[[ LLVM Simple IR ]]]\n";
      std::cout << *func << "\n";
    }

    std::vector<BasicBlock*> to_remove;
    bool Broken = false;
    for(Function::iterator I = func->begin(), E = func->end(); I != E; ++I) {
      if(I->empty()) {
        BasicBlock& bb = *I;
        // No one jumps to it....
        if(llvm::pred_begin(&bb) == llvm::pred_end(&bb)) {
          to_remove.push_back(&bb);
        } else {
          std::cout << "Basic Block is empty and used!\n";
        }
      } else if(!I->back().isTerminator()) {
        std::cerr << "Basic Block does not have terminator!\n";
        std::cerr << *I << "\n";
        cerr << "\n";
        Broken = true;
      }
    }

    for(std::vector<BasicBlock*>::iterator i = to_remove.begin();
        i != to_remove.end();
        i++) {
      (*i)->eraseFromParent();
    }

    if(Broken or llvm::verifyFunction(*func, PrintMessageAction)) {
      std::cout << "ERROR: complication error detected.\n";
      std::cout << "ERROR: Please report the above message and the\n";
      std::cout << "       code below to http://github.com/evanphx/rubinius/issues\n";
      std::cout << *func << "\n";
      function_ = NULL;
      return;
    }

    ls->passes()->run(*func);

    if(ls->jit_dump_code() & cOptimized) {
      std::cout << "[[[ LLVM Optimized IR: "
        << ls->symbol_cstr(vmm->original->name()) << " ]]]\n";
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

  void LLVMState::show_machine_code(void* impl, size_t bytes) {
    assembler_x86::AssemblerX86::show_buffer(impl, bytes, false, NULL);
  }

}

extern "C" {
  void llvm_dump(Value* val) {
    std::cout << *val;
  }
}

#endif
