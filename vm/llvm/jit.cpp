#ifdef ENABLE_LLVM

#include "vmmethod.hpp"
#include "llvm/jit.hpp"
#include "llvm/jit_context.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/staticscope.hpp"
#include "builtin/module.hpp"
#include "field_offset.hpp"
#include "builtin/compiledmethod.hpp"
#include "objectmemory.hpp"

#include "call_frame.hpp"
#include "configuration.hpp"

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
#include <stdlib.h>
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

  JITMethodInfo::JITMethodInfo(jit::Context& ctx, CompiledMethod* cm, VMMethod* v,
                  JITMethodInfo* parent)
    : context_(ctx)
    , function_(0)
    , entry_(0)
    , call_frame_(0)
    , stack_(0)
    , vm_(0)
    , args_(0)
    , previous_(0)
    , profiling_entry_(0)
    , out_args_(0)
    , counter_(0)
    , parent_info_(parent)
    , creator_info_(0)
    , use_full_scope_(false)
    , inline_block_(0)
    , block_info_(0)
    , method_(&ctx.state()->roots())
    , vmm(v)
    , is_block(false)
    , inline_return(0)
    , return_value(0)
    , inline_policy(0)
    , fin_block(0)
    , called_args(-1)
    , stack_args(0)
    , root(0)
  {
    method_.set(cm);
  }

  void JITMethodInfo::set_function(llvm::Function* func) {
    function_ = func;

    return_pad_ = llvm::BasicBlock::Create(context_.state()->ctx(), "return_pad", func);
    return_phi_ = llvm::PHINode::Create(
       context_.state()->ptr_type("Object"), "return_phi", return_pad_);
  }

  JITInlineBlock::JITInlineBlock(LLVMState* ls, llvm::PHINode* phi, llvm::BasicBlock* brk,
                   CompiledMethod* cm, VMMethod* code,
                   JITMethodInfo* scope, int which)
      : block_break_result_(phi)
      , block_break_loc_(brk)
      , code_(code)
      , method_(&ls->roots())
      , scope_(scope)
      , which_(which)
      , created_object_(false)
    {
      method_.set(cm);
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
    return llvm::PointerType::getUnqual(
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
      sigset_t set;
      sigfillset(&set);

      pthread_sigmask(SIG_SETMASK, &set, NULL);

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

          while(pending_requests_.empty()) {
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

        // This isn't ideal, but it's the safest. Keep the GC from
        // running while we're building the IR.
        ls_->shared().gc_dependent();

        // mutex now unlock, allowing others to push more requests
        //

        jit::Compiler jit;

        void* func = 0;
        {
          timer::Running<size_t, 1000000> timer(ls_->shared().stats.jit_time_spent);

          if(req->is_block()) {
            jit.compile_block(ls_, req->method(), req->vmmethod());
          } else {
            jit.compile_method(ls_, req->method(), req->vmmethod());
          }

          func = jit.generate_function(ls_);
        }

        // We were unable to compile this function, likely
        // because it's got something we don't support.
        if(!func) {
          if(ls_->config().jit_show_compiling) {
            llvm::outs() << "[[[ JIT error in background compiling ]]]\n";
          }
          // If someone was waiting on this, wake them up.
          if(thread::Condition* cond = req->waiter()) {
            cond->signal();
          }

          delete req;

          // We don't depend on the GC here, so let it run independent
          // of us.
          ls_->shared().gc_independent();

          continue;
        }

        if(show_machine_code_) {
          jit.show_machine_code();
        }

        // If the method has had jit'ing request disabled since we started
        // JIT'ing it, discard our work.
        if(!req->vmmethod()->jit_disabled()) {
          req->vmmethod()->set_jitted(jit.llvm_function(),
                                      jit.code_bytes(),
                                      func);

          if(!req->is_block()) {
            req->method()->execute = reinterpret_cast<executor>(func);
          }
          assert(req->method()->jit_data());

          req->method()->jit_data()->run_write_barrier(ls_->write_barrier(), req->method());

          ls_->shared().stats.jitted_methods++;

          if(ls_->config().jit_show_compiling) {
            llvm::outs() << "[[[ JIT finished background compiling "
                      << (req->is_block() ? " (block)" : " (method)")
                      << " ]]]\n";
          }
        }

        // If someone was waiting on this, wake them up.
        if(thread::Condition* cond = req->waiter()) {
          cond->signal();
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

  namespace {
    /// GetX86CpuIDAndInfo - Execute the specified cpuid and return the 4 values in the
    /// specified arguments.  If we can't run cpuid on the host, return false.
    static bool GetX86CpuIDAndInfo(unsigned value, unsigned *rEAX,
        unsigned *rEBX, unsigned *rECX, unsigned *rEDX) {
#if defined(__x86_64__) || defined(_M_AMD64) || defined (_M_X64)
#if defined(__GNUC__)
      // gcc doesn't know cpuid would clobber ebx/rbx. Preseve it manually.
      asm ("movq\t%%rbx, %%rsi\n\t"
          "cpuid\n\t"
          "xchgq\t%%rbx, %%rsi\n\t"
          : "=a" (*rEAX),
          "=S" (*rEBX),
          "=c" (*rECX),
          "=d" (*rEDX)
          :  "a" (value));
      return true;
#elif defined(_MSC_VER)
      int registers[4];
      __cpuid(registers, value);
      *rEAX = registers[0];
      *rEBX = registers[1];
      *rECX = registers[2];
      *rEDX = registers[3];
      return true;
#endif
#elif defined(i386) || defined(__i386__) || defined(__x86__) || defined(_M_IX86)
#if defined(__GNUC__)
      asm ("movl\t%%ebx, %%esi\n\t"
          "cpuid\n\t"
          "xchgl\t%%ebx, %%esi\n\t"
          : "=a" (*rEAX),
          "=S" (*rEBX),
          "=c" (*rECX),
          "=d" (*rEDX)
          :  "a" (value));
      return true;
#elif defined(_MSC_VER)
      __asm {
        mov   eax,value
          cpuid
          mov   esi,rEAX
          mov   dword ptr [esi],eax
          mov   esi,rEBX
          mov   dword ptr [esi],ebx
          mov   esi,rECX
          mov   dword ptr [esi],ecx
          mov   esi,rEDX
          mov   dword ptr [esi],edx
      }
      return true;
#endif
#endif
      return false;
    }

    static void DetectX86FamilyModel(unsigned EAX, unsigned *o_Family, unsigned *o_Model) {
      unsigned Family = (EAX >> 8) & 0xf; // Bits 8 - 11
      unsigned Model  = (EAX >> 4) & 0xf; // Bits 4 - 7
      if (Family == 6 || Family == 0xf) {
        if (Family == 0xf)
          // Examine extended family ID if family ID is F.
          Family += (EAX >> 20) & 0xff;    // Bits 20 - 27
        // Examine extended model ID if family ID is 6 or F.
        Model += ((EAX >> 16) & 0xf) << 4; // Bits 16 - 19
      }

      *o_Family = Family;
      *o_Model = Model;
    }

    bool is_sandy_bridge() {
      unsigned EAX = 0, EBX = 0, ECX = 0, EDX = 0;
      if(!GetX86CpuIDAndInfo(0x1, &EAX, &EBX, &ECX, &EDX)) return false;

      unsigned Family = 0;
      unsigned Model  = 0;
      DetectX86FamilyModel(EAX, &Family, &Model);

      return Family == 6 && Model == 42;
    }
  }

  LLVMState::LLVMState(STATE)
    : ManagedThread(state->shared, ManagedThread::eSystem)
    , ctx_(llvm::getGlobalContext())
    , config_(state->shared.config)
    , global_lock_(state->global_lock())
    , symbols_(state->shared.symbols)
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

    set_name("Background Compiler");
    state->shared.add_managed_thread(this);
    state->shared.om->add_aux_barrier(&write_barrier_);

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

    Int8PtrTy = llvm::PointerType::getUnqual(Int8Ty);

    bool fast_code_passes = false;

    module_ = new llvm::Module("rubinius", ctx_);

    autogen_types::makeLLVMModuleContents(module_);

    // If this is a sandy bridge machine, force LLVM to treat it like a core2
    // machine to work around a LLVM bug in 2.8.
    if(is_sandy_bridge()) {
      engine_ = EngineBuilder(module_)
                  .setEngineKind(EngineKind::JIT)
                  .setErrorStr(0)
                  .setOptLevel(CodeGenOpt::Default)
                  .setMCPU("core2")
                  .create();
    } else {
      engine_ = ExecutionEngine::create(module_, false, 0, CodeGenOpt::Default, false);
    }

    passes_ = new llvm::FunctionPassManager(module_);

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

    add_internal_functions();

    background_thread_ = new BackgroundCompilerThread(this);
    background_thread_->run();
  }

  LLVMState::~LLVMState() {
    shared_.remove_managed_thread(this);
    shared_.om->del_aux_barrier(&write_barrier_);
  }

  bool LLVMState::debug_p() {
    return config_.jit_debug;
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
    if(sym == reinterpret_cast<const Symbol*>(Qnil)) return "<nil>";
    return symbols_.lookup_cstring(sym);
  }

  const char* LLVMState::enclosure_name(CompiledMethod* cm) {
    StaticScope* ss = cm->scope();
    if(!kind_of<StaticScope>(ss) || !kind_of<Module>(ss->module())) {
      return "ANONYMOUS";
    }

    return symbol_cstr(ss->module()->name());
  }

  void LLVMState::compile_soon(STATE, CompiledMethod* cm, BlockEnvironment* block) {
    Object* placement;
    bool is_block = false;
    bool wait = config().jit_sync;

    // Ignore it!
    if(cm->backend_method()->call_count <= 1) {
      if(config().jit_inline_debug) {
        log() << "JIT: ignoring candidate! "
          << symbol_cstr(cm->name()) << "\n";
      }
      return;
    }

    if(config().jit_inline_debug) {
      if(block) {
        log() << "JIT: queueing block inside: "
          << symbol_cstr(cm->name()) << "\n";
      } else {
        log() << "JIT: queueing method: "
          << symbol_cstr(cm->name()) << "\n";
      }
    }

    cm->backend_method()->call_count = -1;

    if(block) {
      is_block = true;
      placement = block;
    } else {
      placement = Qnil;
    }

    BackgroundCompileRequest* req =
      new BackgroundCompileRequest(state, cm, placement, is_block);

    queued_methods_++;

    if(wait) {
      thread::Condition cond;
      req->set_waiter(&cond);

      thread::Mutex mux;
      mux.lock();

      background_thread_->add(req);
      cond.wait(mux);

      mux.unlock();

      if(config().jit_inline_debug) {
        if(block) {
          log() << "JIT: compiled block inside: "
                << symbol_cstr(cm->name()) << "\n";
        } else {
          log() << "JIT: compiled method: "
                << symbol_cstr(cm->name()) << "\n";
        }
      }
    } else {
      background_thread_->add(req);

      if(state->shared.config.jit_show_compiling) {
        llvm::outs() << "[[[ JIT Queued"
          << (block ? " block " : " method ")
          << queued_methods() << "/"
          << jitted_methods() << " ]]]\n";
      }
    }
  }

  void LLVMState::remove(llvm::Function* func) {
    shared_.stats.jitted_methods--;

    // Deallocate the JITed code
    engine_->freeMachineCodeForFunction(func);

    // Nuke the Function from the module
    func->replaceAllUsesWith(UndefValue::get(func->getType()));
    func->eraseFromParent();
  }

  const static int cInlineMaxDepth = 4;

  void LLVMState::compile_callframe(STATE, CompiledMethod* start, CallFrame* call_frame,
                                    int primitive) {
    if(config().jit_inline_debug) {
      if(start) {
        log() << "JIT: target search from "
          << symbol_cstr(start->name()) << "\n";
      } else {
        log() << "JIT: target search from primitive\n";
      }
    }

    CallFrame* candidate = find_candidate(start, call_frame);
    if(!candidate || candidate->jitted_p() || candidate->inline_method_p()) {
      if(config().jit_inline_debug) {
        log() << "JIT: unable to find candidate\n";
      }
      return;
    }

    if(candidate->cm->backend_method()->call_count <= 1) {
      if(!start || start->backend_method()->jitted()) return;
      // Ignore it. compile this one.
      candidate = call_frame;
    }

    if(candidate->block_p()) {
      compile_soon(state, candidate->cm, candidate->block_env());
    } else {
      compile_soon(state, candidate->cm);
    }
  }

#define SMALL_METHOD_SIZE 50

  CallFrame* LLVMState::find_candidate(CompiledMethod* start, CallFrame* call_frame) {
    if(!config_.jit_inline_generic) {
      return call_frame;
    }

    int depth = cInlineMaxDepth;

    if(!start) {
      start = call_frame->cm;
      call_frame = call_frame->previous;
      depth--;
    }

    if(!call_frame || start->backend_method()->total > SMALL_METHOD_SIZE) {
      return call_frame;
    }

    CallFrame* caller = call_frame;

    while(depth-- > 0) {
      CompiledMethod* cur = call_frame->cm;
      VMMethod* vmm = cur->backend_method();

      /*
      if(call_frame->block_p()
          || vmm->required_args != vmm->total_args // has a splat
          || vmm->call_count < 200 // not called much
          || vmm->jitted() // already jitted
          || vmm->parent() // is a block
        ) return caller;
      */

      if(vmm->required_args != vmm->total_args // has a splat
          || vmm->call_count < 200 // not called much
          || vmm->jitted() // already jitted
          || !vmm->no_inline_p() // method marked as not inlinable
        ) return caller;

      CallFrame* next = call_frame->previous;

      if(!next|| cur->backend_method()->total > SMALL_METHOD_SIZE) return call_frame;

      caller = call_frame;
      call_frame = next;
    }

    return caller;
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

      for(uint8_t i = 0; i < 2; i++) {
        if(ud.operand[i].type == UD_OP_IMM) {
          Dl_info info;
          if(dladdr((void*)ud.operand[i].lval.uqword, &info)) {
            std::cout << " ; " << info.dli_sname;
            break; // only do one
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
