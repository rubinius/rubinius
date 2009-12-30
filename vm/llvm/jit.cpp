#ifdef ENABLE_LLVM

#include "vmmethod.hpp"
#include "llvm/jit.hpp"

#include "builtin/fixnum.hpp"
#include "builtin/staticscope.hpp"
#include "builtin/module.hpp"
#include "field_offset.hpp"

#include "call_frame.hpp"
#include "configuration.hpp"

#include "instruments/profiler.hpp"

#include <llvm/Target/TargetData.h>
// #include <llvm/LinkAllPasses.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/CallingConv.h>
#include <llvm/Support/CFG.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/Target/TargetSelect.h>

#include <llvm/Target/TargetOptions.h>

namespace autogen_types {
  void makeLLVMModuleContents(llvm::Module* module);
}

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <dlfcn.h>

#include "llvm/jit_compiler.hpp"
#include "llvm/jit_method.hpp"
#include "llvm/jit_block.hpp"
#include "llvm/passes.hpp"
#include "instructions_util.hpp"

#include <udis86.h>
// for abi::__cxa_demangle
#include <cxxabi.h>

using namespace llvm;

namespace rubinius {

  AllocaInst* JITMethodInfo::create_alloca(const Type* type, Value* size,
                                           const Twine& name)
  {
    return new AllocaInst(type, size, name,
        function_->getEntryBlock().getTerminator());
  }

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

  const llvm::Type* LLVMState::type(std::string name) {
    std::string full_name = std::string("struct.rubinius::") + name;
    return module_->getTypeByName(full_name.c_str());
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

        jit::Compiler jit;

        void* func = 0;
        {
          timer::Running timer(ls_->time_spent);

          if(req->is_block()) {
            jit.compile_block(ls_, req->vmmethod());
          } else {
            jit.compile_method(ls_, req->vmmethod());
          }

          func = jit.generate_function(ls_);
        }

        // We were unable to compile this function, likely
        // because it's got something we don't support.
        if(!func) {
          if(ls_->config().jit_show_compiling) {
            llvm::outs() << "[[[ JIT error in background compiling ]]]\n";
          }

          delete req;
          continue; // for(;;)
        }

        if(show_machine_code_) {
          jit.show_machine_code();
        }

        // Ok, now we are manipulating managed memory, so make
        // sure the GC doesn't run.
        ls_->shared().gc_dependent();

        req->vmmethod()->set_jitted(jit.llvm_function(),
                                    jit.code_bytes(),
                                    func);

        if(req->is_block()) {
          BlockEnvironment* be = req->block_env();
          if(!be) {
            llvm::outs() << "Fatal error in JIT. Expected a BlockEnvironment.\n";
          } else {
            be->set_native_function(func);
          }
        } else {
          req->vmmethod()->original.get()->execute =
                           reinterpret_cast<executor>(func);
        }

        int which = ls_->add_jitted_method();
        if(ls_->config().jit_show_compiling) {
          llvm::outs() << "[[[ JIT finished background compiling "
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
    : ctx_(llvm::getGlobalContext())
    , config_(state->shared.config)
    , global_lock_(state->global_lock())
    , symbols_(state->symbols)
    , jitted_methods_(0)
    , queued_methods_(0)
    , accessors_inlined_(0)
    , uncommons_taken_(0)
    , shared_(state->shared)
    , include_profiling_(state->shared.config.jit_profile)
    , code_bytes_(0)
    , log_(0)
    , time_spent(0)
  {

    if(state->shared.config.jit_log.value.size() == 0) {
      log_ = &std::cerr;
    } else {
      std::ofstream* s = new std::ofstream(
          state->shared.config.jit_log.value.c_str(), std::ios::out);

      if(s->fail()) {
        delete s;
        log_ = &std::cerr;
      } else {
        log_ = s;
      }
    }

    llvm::NoFramePointerElim = true;
    llvm::InitializeNativeTarget();

    VoidTy = Type::getVoidTy(ctx_);

    Int1Ty = Type::getInt1Ty(ctx_);
    Int8Ty = Type::getInt8Ty(ctx_);
    Int16Ty = Type::getInt16Ty(ctx_);
    Int32Ty = Type::getInt32Ty(ctx_);
    Int64Ty = Type::getInt64Ty(ctx_);

#ifdef IS_X8664
    IntPtrTy = Int64Ty;
#else
    IntPtrTy = Int32Ty;
#endif

    FloatTy = Type::getFloatTy(ctx_);
    DoubleTy = Type::getDoubleTy(ctx_);

    bool fast_code_passes = false;

    module_ = new llvm::Module("rubinius", ctx_);

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
        *module_, Int1Ty, false,
        GlobalVariable::ExternalLinkage,
        0, "profiling_flag");

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
    if(vmm->call_count < 0) {
      if(config().jit_inline_debug) {
        log() << "JIT: ignoring candidate! "
          << symbol_cstr(vmm->original->scope()->module()->name())
          << "#"
          << symbol_cstr(vmm->original->name()) << "\n";
      }
      return;
    }

    if(config().jit_inline_debug) {
      log() << "JIT: queueing method: "
        << symbol_cstr(vmm->original->scope()->module()->name())
        << "#"
        << symbol_cstr(vmm->original->name()) << "\n";
    }
    vmm->call_count = -1;

    if(block) {
      is_block = true;
      placement = block;
    } else {
      placement = Qnil;
    }

    state->stats.jitted_methods++;

    BackgroundCompileRequest* req =
      new BackgroundCompileRequest(state, vmm, placement, is_block);

    queued_methods_++;

    background_thread_->add(req);

    if(state->shared.config.jit_show_compiling) {
      llvm::outs() << "[[[ JIT Queued"
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

  const static int cInlineMaxDepth = 4;

  /*
  static CallFrame* find_call_frame(CallFrame* frame, VMMethod* meth, int* dist) {
    *dist = 0;
    while(frame) {
      if(frame->cm->backend_method() == meth) return frame;
      frame = frame->previous;
      dist++;
    }

    return 0;
  }
  */

  /*
  static void show_method(LLVMState* ls, VMMethod* vmm, const char* extra = "") {
    CompiledMethod* cm = vmm->original.get();

    std::cerr << "  "
              << ls->symbol_cstr(cm->scope()->module()->name())
              << "#"
              << ls->symbol_cstr(cm->name())
              << " (" << vmm->call_count << ") "
              << extra
              << "\n";
  }
  */

  static VMMethod* find_first_non_block(CallFrame* cf) {
    VMMethod* vmm = cf->cm->backend_method();
    while(vmm->parent()) {
      cf = cf->previous;
      if(!cf) return 0;
      vmm = cf->cm->backend_method();
    }

    return vmm;
  }

  static CallFrame* validate_block_parent(CallFrame* cf, VMMethod* parent) {
    if(cf->previous && cf->previous->previous) {
      cf = cf->previous->previous;
      if(cf->cm->backend_method() == parent) return cf;
    }

    return 0;
  }

  void LLVMState::compile_callframe(STATE, VMMethod* start, CallFrame* call_frame,
                                    int primitive) {
    if(config().jit_inline_debug) {
      if(start) {
        log() << "JIT: target search from "
          << symbol_cstr(start->original->scope()->module()->name())
          << "#"
          << symbol_cstr(start->original->name()) << "\n";
      } else {
        log() << "JIT: target search from primitive\n";
      }
    }

    VMMethod* candidate = find_candidate(start, call_frame);
    if(!candidate) {
      if(config().jit_inline_debug) {
        log() << "JIT: unable to find candidate\n";
      }
      return;
    }

    assert(!candidate->parent());

    if(candidate->call_count < 0) {
      if(!start) return;
      // Ignore it. compile this one.
      candidate = start;
    }

    compile_soon(state, candidate);
  }

  VMMethod* LLVMState::find_candidate(VMMethod* start, CallFrame* call_frame) {
    VMMethod* found = start;
    int depth = 0;
    bool consider_block_parents = config_.jit_inline_blocks;

    // No upper call_frames or generic inlining is off, use the start.
    // With generic inlining off, there is no way to inline back to start,
    // so we don't both trying.
    if(!config_.jit_inline_generic) {
      if(!start) start = call_frame->cm->backend_method();
      return find_first_non_block(call_frame);
    }

    /*
    std::cerr << "JIT target search:\n";

    if(start) {
      show_method(this, start);
    } else {
      std::cerr << "  <primitive>\n";
    }
    */

    VMMethod* next = call_frame->cm->backend_method();
    VMMethod* parent = 0;

    while(depth < cInlineMaxDepth) {
      // show_method(this, next);

      // Basic requirements
      if(next->required_args != next->total_args ||
          next->call_count < 200 ||
          next->jitted()) break;

      // Jump to defining methods of blocks?
      parent = next->parent();
      if(parent) {
        if(consider_block_parents) {
          // See if parent is in this call_frame chain properly..
          if(CallFrame* pf = validate_block_parent(call_frame, parent)) {
            depth++;

            // Method parents are valuable, so always use them if we find them.
            if(!parent->parent()) {
              found = parent;
            }
            // show_method(this, parent, " parent!");

            call_frame = pf;
          }
        } else {
          // We hit a block, just bail.
          break;
        }
      } else {
        found = next;
      }

      call_frame = call_frame->previous;
      if(!call_frame) break;

      next = call_frame->cm->backend_method();

      depth++;
    }

    if(!found && !next->parent()) return next;

    return found;
  }

  void LLVMState::show_machine_code(void* buffer, size_t size) {
    ud_t ud;

    ud_init(&ud);
#ifdef IS_X8664
    ud_set_mode(&ud, 64);
#else
    ud_set_mode(&ud, 32);
#endif
    ud_set_syntax(&ud, UD_SYN_ATT);
    ud_set_input_buffer(&ud, reinterpret_cast<uint8_t*>(buffer), size);

    while(ud_disassemble(&ud)) {
      void* address = reinterpret_cast<void*>(
          reinterpret_cast<uintptr_t>(buffer) + ud_insn_off(&ud));

      std::cout << std::setw(10) << std::right
                << address
                << "  ";

      std::cout << std::setw(24) << std::left << ud_insn_asm(&ud);

      if(ud.operand[0].type == UD_OP_JIMM) {
        const void* addr = (const void*)((uintptr_t)buffer + ud.pc + (int)ud.operand[0].lval.udword);
        std::cout << " ; " << addr;
        if(ud.mnemonic == UD_Icall) {
          Dl_info info;
          if(dladdr(addr, &info)) {
            int status = 0;
            char* cpp_name = abi::__cxa_demangle(info.dli_sname, 0, 0, &status);
            if(status >= 0) {
              // Chop off the arg info from the signature output
              char *paren = strstr(cpp_name, "(");
              *paren = 0;
              std::cout << " " << cpp_name;
              free(cpp_name);
            } else {
              std::cout << " " << info.dli_sname;
            }
          }
        }
      }

      std::cout << "\n";
    }
  }

}

extern "C" {
  void llvm_dump(Value* val) {
    llvm::outs() << *val << "\n";
  }
}

#endif
