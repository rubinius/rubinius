#ifdef ENABLE_LLVM

#include "llvm/state.hpp"

#include "llvm/jit_compiler.hpp"
#include "llvm/jit_method.hpp"
#include "llvm/jit_block.hpp"
#include "llvm/method_info.hpp"
#include "llvm/background_compile_request.hpp"
#include "llvm/disassembler.hpp"
#include "llvm/jit_context.hpp"

#include "vm/config.h"

#include "builtin/fixnum.hpp"
#include "builtin/staticscope.hpp"
#include "builtin/module.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/class.hpp"
#include "builtin/block_environment.hpp"

#include "vmmethod.hpp"
#include "field_offset.hpp"
#include "objectmemory.hpp"

#include "call_frame.hpp"
#include "configuration.hpp"
#include "instruments/timing.hpp"

#include <llvm/Target/TargetData.h>
// #include <llvm/LinkAllPasses.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/CallingConv.h>
#include <llvm/Support/CFG.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/Target/TargetSelect.h>

#include <llvm/Target/TargetOptions.h>

#include "windows_compat.h"

namespace autogen_types {
  void makeLLVMModuleContents(llvm::Module* module);
}

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <stdlib.h>
#ifndef RBX_WINDOWS
#include <dlfcn.h>
#endif

#include "llvm/passes.hpp"
#include "instructions_util.hpp"

#include <udis86.h>
// for abi::__cxa_demangle
#include <cxxabi.h>

using namespace llvm;

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

  void LLVMState::start(STATE) {
    if(!state->shared.llvm_state) return;
    state->shared.llvm_state->start_i();
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
      cIdle,
      cStopped
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
      : Thread(0, false)
      , ls_(ls)
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
        if(state == cStopped) return;

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

      {
        thread::Mutex::LockGuard guard(mutex_);
        state = cStopped;
      }
    }

    void start() {
      thread::Mutex::LockGuard guard(mutex_);
      if(state != cStopped) return;
      state = cUnknown;
      run();
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
      set_name("rbx.jit");

      ManagedThread::set_current(ls_);

#ifndef RBX_WINDOWS
      sigset_t set;
      sigfillset(&set);
      pthread_sigmask(SIG_SETMASK, &set, NULL);
#endif

      for(;;) { // forever

        BackgroundCompileRequest* req = 0;

        // Lock, wait, get a request, unlock
        {
          thread::Mutex::LockGuard guard(mutex_);

          if(pause_) {
            state = cPaused;

            paused_ = true;
            pause_condition_.broadcast();

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
        ls_->shared().gc_dependent(ls_);

        // mutex now unlock, allowing others to push more requests
        //

        jit::Compiler jit(ls_);

        int spec_id = 0;
        if(Class* cls = req->receiver_class()) {
          spec_id = cls->class_id();
        }

        void* func = 0;
        {
          timer::Running<1000000> timer(ls_->shared().stats.jit_time_spent);

          jit.compile(ls_, req);

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
          ls_->shared().gc_independent(ls_);

          continue;
        }

        if(show_machine_code_) {
          jit.show_machine_code();
        }

        // If the method has had jit'ing request disabled since we started
        // JIT'ing it, discard our work.
        if(!req->vmmethod()->jit_disabled()) {

          jit::RuntimeDataHolder* rd = jit.context().runtime_data_holder();

          ls_->start_method_update();

          if(!req->is_block()) {
            if(spec_id) {
              req->method()->add_specialized(spec_id, reinterpret_cast<executor>(func), rd);
            } else {
              req->method()->set_unspecialized(reinterpret_cast<executor>(func), rd);
            }
          } else {
            req->method()->set_unspecialized(reinterpret_cast<executor>(func), rd);
          }

          assert(req->method()->jit_data());

          ls_->end_method_update();

          rd->run_write_barrier(ls_->write_barrier(), req->method());

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
        ls_->shared().gc_independent(ls_);
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
      // gcc doesn't know cpuid would clobber ebx/rbx. Preserve it manually.
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

  static const bool debug_search = false;

  LLVMState::LLVMState(STATE)
    : ManagedThread(state->shared.new_thread_id(state),
                    state->shared, ManagedThread::eSystem)
    , ctx_(llvm::getGlobalContext())
    , config_(state->shared.config)
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
    state->shared.om->add_aux_barrier(state, &write_barrier_);

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

    VoidPtrTy = llvm::PointerType::getUnqual(Int8Ty);

    FloatTy = Type::getFloatTy(ctx_);
    DoubleTy = Type::getDoubleTy(ctx_);

    Int8PtrTy = llvm::PointerType::getUnqual(Int8Ty);

    Zero = llvm::ConstantInt::get(Int32Ty, 0);
    One = llvm::ConstantInt::get(Int32Ty, 1);

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

    metadata_id_ = ctx_.getMDKindID("rbx-classid");

    fixnum_class_id_ = G(fixnum_class)->class_id();
    symbol_class_id_ = G(symbol)->class_id();
    string_class_id_ = G(string)->class_id();

    type_optz_ = state->shared.config.jit_type_optz;

    background_thread_ = new BackgroundCompilerThread(this);
    background_thread_->run();
  }

  LLVMState::~LLVMState() {
    shared_.remove_managed_thread(this);
    shared_.om->del_aux_barrier(0, &write_barrier_);
    delete passes_;
    delete module_;
    delete background_thread_;
  }

  bool LLVMState::debug_p() {
    return config_.jit_debug;
  }

  void LLVMState::shutdown_i() {
    background_thread_->stop();
  }

  void LLVMState::start_i() {
    background_thread_->start();
  }

  void LLVMState::on_fork_i() {
    shared_.add_managed_thread(this);
    background_thread_->restart();
  }

  void LLVMState::pause_i() {
    background_thread_->pause();
  }

  void LLVMState::unpause_i() {
    background_thread_->unpause();
  }

  Symbol* LLVMState::symbol(const std::string sym) {
    return symbols_.lookup(sym);
  }

  std::string LLVMState::symbol_debug_str(const Symbol* sym) {
    if(sym == reinterpret_cast<const Symbol*>(Qnil)) return "<nil>";
    return symbols_.lookup_debug_string(sym);
  }

  std::string LLVMState::enclosure_name(CompiledMethod* cm) {
    StaticScope* ss = cm->scope();
    if(!kind_of<StaticScope>(ss) || !kind_of<Module>(ss->module())) {
      return "ANONYMOUS";
    }

    return symbol_debug_str(ss->module()->name());
  }

  void LLVMState::compile_soon(STATE, CompiledMethod* cm, Object* placement,
                               bool is_block)
  {
    bool wait = config().jit_sync;

    // Ignore it!
    if(cm->backend_method()->call_count <= 1) {
      // if(config().jit_inline_debug) {
        // log() << "JIT: ignoring candidate! "
          // << symbol_debug_str(cm->name()) << "\n";
      // }
      return;
    }

    if(debug_search) {
      if(is_block) {
        std::cout << "JIT: queueing block inside: "
          << enclosure_name(cm) << "#" << symbol_debug_str(cm->name()) << "\n";
      } else {
        std::cout << "JIT: queueing method: "
          << enclosure_name(cm) << "#" << symbol_debug_str(cm->name()) << "\n";
      }
    }

    // Don't do this because it prevents other class from heating
    // it up too!
    cm->backend_method()->call_count = -1;

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

      // if(config().jit_inline_debug) {
        // if(block) {
          // log() << "JIT: compiled block inside: "
                // << symbol_debug_str(cm->name()) << "\n";
        // } else {
          // log() << "JIT: compiled method: "
                // << symbol_debug_str(cm->name()) << "\n";
        // }
      // }
    } else {
      background_thread_->add(req);

      if(state->shared.config.jit_show_compiling) {
        llvm::outs() << "[[[ JIT Queued"
          << (is_block ? " block " : " method ")
          << queued_methods() << "/"
          << jitted_methods() << " ]]]\n";
      }
    }
  }

  void LLVMState::remove(llvm::Function* func) {
    shared_.stats.jitted_methods.dec();

    // Deallocate the JITed code
    engine_->freeMachineCodeForFunction(func);

    // Nuke the Function from the module
    func->replaceAllUsesWith(UndefValue::get(func->getType()));
    func->eraseFromParent();
  }

  const static int cInlineMaxDepth = 2;
  const static size_t eMaxInlineSendCount = 10;

  void LLVMState::compile_callframe(STATE, CompiledMethod* start, CallFrame* call_frame,
                                    int primitive) {

    if(debug_search) {
      std::cout << "\nJIT:       triggered: "
            << enclosure_name(start) << "#"
            << symbol_debug_str(start->name()) << "\n";
    }

    // if(config().jit_inline_debug) {
      // if(start) {
        // std::cout << "JIT: target search from "
          // << symbol_debug_str(start->name()) << "\n";
      // } else {
        // std::cout << "JIT: target search from primitive\n";
      // }
    // }

    CallFrame* candidate = find_candidate(state, start, call_frame);
    if(!candidate || candidate->jitted_p() || candidate->inline_method_p()) {
      if(debug_search) {
        std::cout << "JIT: invalid candidate returned\n";
      }

      return;
    }

    if(debug_search) {
      std::cout << "! ";
      candidate->print_backtrace(state, 1);
    }

    if(start && candidate->cm != start) {
      start->backend_method()->call_count = 0;
    }

    if(candidate->cm->backend_method()->call_count <= 1) {
      if(!start || start->backend_method()->jitted()) return;
      // Ignore it. compile this one.
      candidate = call_frame;
    }

    if(candidate->block_p()) {
      compile_soon(state, candidate->cm, candidate->block_env(), true);
    } else {
      if(candidate->cm->can_specialize_p()) {
        compile_soon(state, candidate->cm, candidate->self()->class_object(state));
      } else {
        compile_soon(state, candidate->cm, Qnil);
      }
    }
  }

#define SMALL_METHOD_SIZE 50

  CallFrame* LLVMState::find_candidate(STATE, CompiledMethod* start, CallFrame* call_frame) {
    if(!config_.jit_inline_generic) {
      return call_frame;
    }

    int depth = cInlineMaxDepth;

    if(!start) rubinius::bug("null start");
    if(!call_frame) rubinius::bug("null call_frame");

    // if(!start) {
      // start = call_frame->cm;
      // call_frame = call_frame->previous;
      // depth--;
    // }

    if(debug_search) {
      std::cout << "> call_count: " << call_frame->cm->backend_method()->call_count
            << " size: " << call_frame->cm->backend_method()->total
            << " sends: " << call_frame->cm->backend_method()->inline_cache_count()
            << "\n";

      call_frame->print_backtrace(state, 1);
    }

    if(start->backend_method()->total > SMALL_METHOD_SIZE) {
      if(debug_search) {
        std::cout << "JIT: STOP. reason: trigger method isn't small: "
              << start->backend_method()->total << " > "
              << SMALL_METHOD_SIZE
              << "\n";
      }

      return call_frame;
    }

    VMMethod* vmm = start->backend_method();

    if(vmm->required_args != vmm->total_args) {
      if(debug_search) {
        std::cout << "JIT: STOP. reason: trigger method req_args != total_args\n";
      }

      return call_frame;
    }

    if(vmm->no_inline_p()) {
      if(debug_search) {
        std::cout << "JIT: STOP. reason: trigger method no_inline_p() = true\n";
      }

      return call_frame;
    }

    CallFrame* callee = call_frame;
    call_frame = call_frame->previous;

    if(!call_frame) return callee;

    // Now start looking at callers.

    while(depth-- > 0) {
      CompiledMethod* cur = call_frame->cm;

      if(!cur) {
        if(debug_search) {
          std::cout << "JIT: STOP. reason: synthetic CallFrame hit\n";
        }
        return callee;
      }

      VMMethod* vmm = cur->backend_method();

      if(debug_search) {
        std::cout << "> call_count: " << vmm->call_count
              << " size: " << vmm->total
              << " sends: " << vmm->inline_cache_count()
              << "\n";

        call_frame->print_backtrace(state, 1);
      }


      /*
      if(call_frame->block_p()
          || vmm->required_args != vmm->total_args // has a splat
          || vmm->call_count < 200 // not called much
          || vmm->jitted() // already jitted
          || vmm->parent() // is a block
        ) return callee;
      */

      if(vmm->required_args != vmm->total_args) {
        if(debug_search) {
          std::cout << "JIT: STOP. reason: req_args != total_args\n";
        }
        return callee;
      }

      if(vmm->call_count < 200) {
        if(debug_search) {
          std::cout << "JIT: STOP. reason: call_count too small: "
                << vmm->call_count << " < 200\n";
        }

        return callee;
      }

      if(vmm->jitted()) {
        if(debug_search) {
          std::cout << "JIT: STOP. reason: already jitted\n";
        }

        return callee;
      }

      if(vmm->no_inline_p()) {
        if(debug_search) {
          std::cout << "JIT: STOP. reason: no_inline_p() = true\n";
        }

        return callee;
      }

      if(vmm->inline_cache_count() > eMaxInlineSendCount) {
        if(debug_search) {
          std::cout << "JIT: STOP. reason: high send count\n";
        }

        return call_frame;
      }

      // if(vmm->required_args != vmm->total_args // has a splat
          // || vmm->call_count < 200 // not called much
          // || vmm->jitted() // already jitted
          // || !vmm->no_inline_p() // method marked as not inlineable
        // ) return callee;

      CallFrame* prev = call_frame->previous;

      if(!prev) {
        if(debug_search) {
          std::cout << "JIT: STOP. reason: toplevel method\n";
        }
        return call_frame;
      }

      // if(cur->backend_method()->total > SMALL_METHOD_SIZE) {
        // if(debug_search) {
          // std::cout << "JIT: STOP. reason: big method: "
                // << cur->backend_method()->total << " > "
                // << SMALL_METHOD_SIZE
                // << "\n";
        // }

        // return call_frame;
      // }

      // if(!next || cur->backend_method()->total > SMALL_METHOD_SIZE) return call_frame;

      callee->cm->backend_method()->call_count = 0;

      callee = call_frame;
      call_frame = prev;
    }

    return callee;
  }

  void LLVMState::show_machine_code(void* buffer, size_t size) {

#if defined(IS_X86) || defined(IS_X8664)
#ifndef RBX_WINDOWS
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
#endif  // !RBX_WINDOWS

#else
    JITDisassembler disassembler(buffer, size);
    std::string output = disassembler.print_machine_code();
    std::cout << output;
#endif // !IS_X86

  }

}

extern "C" {
  void llvm_dump(Value* val) {
    llvm::outs() << *val << "\n";
  }
}

#endif
