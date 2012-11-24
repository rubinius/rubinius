#ifdef ENABLE_LLVM

#include "llvm/state.hpp"

#include "llvm/jit_compiler.hpp"
#include "llvm/jit_method.hpp"
#include "llvm/jit_block.hpp"
#include "llvm/method_info.hpp"
#include "llvm/background_compile_request.hpp"
#include "llvm/disassembler.hpp"
#include "llvm/jit_context.hpp"

#include "builtin/fixnum.hpp"
#include "builtin/constantscope.hpp"
#include "builtin/module.hpp"
#include "builtin/compiledcode.hpp"
#include "builtin/class.hpp"
#include "builtin/block_environment.hpp"

#include "auxiliary_threads.hpp"
#include "machine_code.hpp"
#include "field_offset.hpp"
#include "objectmemory.hpp"

#include "call_frame.hpp"
#include "configuration.hpp"
#include "instruments/timing.hpp"

#if RBX_LLVM_API_VER >= 302
#include <llvm/DataLayout.h>
#else
#include <llvm/Target/TargetData.h>
#endif
// #include <llvm/LinkAllPasses.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/CallingConv.h>
#include <llvm/Support/CFG.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/Support/TargetSelect.h>

#include <llvm/Target/TargetOptions.h>

#include "windows_compat.h"

#include "gc/gc.hpp"

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

  static utilities::thread::Mutex lock_;

  LLVMState* LLVMState::get(STATE) {
    utilities::thread::Mutex::LockGuard lg(lock_);

    if(!state->shared().llvm_state) {
      state->shared().llvm_state = new LLVMState(state);
    }

    return state->shared().llvm_state;
  }

  LLVMState* LLVMState::get_if_set(STATE) {
    utilities::thread::Mutex::LockGuard lg(lock_);

    return state->shared().llvm_state;
  }

  LLVMState* LLVMState::get_if_set(VM* vm) {
    utilities::thread::Mutex::LockGuard lg(lock_);

    return vm->shared.llvm_state;
  }

  llvm::Type* LLVMState::ptr_type(std::string name) {
    std::string full_name = std::string("struct.rubinius::") + name;
    return llvm::PointerType::getUnqual(
        module_->getTypeByName(full_name));
  }

  llvm::Type* LLVMState::type(std::string name) {
    std::string full_name = std::string("struct.rubinius::") + name;
    return module_->getTypeByName(full_name);
  }

  class BackgroundCompilerThread : public utilities::thread::Thread {
    enum State {
      cUnknown,
      cRunning,
      cPaused,
      cIdle,
      cStopped
    };

    utilities::thread::Mutex mutex_;
    std::list<BackgroundCompileRequest*> pending_requests_;
    utilities::thread::Condition condition_;
    utilities::thread::Condition pause_condition_;

    LLVMState* ls_;
    bool show_machine_code_;

    jit::Compiler* current_compiler_;
    BackgroundCompileRequest* current_req_;

    State state;
    bool stop_;
    bool pause_;
    bool paused_;

  public:
    BackgroundCompilerThread(LLVMState* ls)
      : Thread(0, false)
      , ls_(ls)
      , current_compiler_(0)
      , current_req_(0)
      , state(cUnknown)
      , stop_(false)
      , pause_(false)
      , paused_(false)
    {
      show_machine_code_ = ls->jit_dump_code() & cMachineCode;
      condition_.init();
      pause_condition_.init();
    }

    void add(BackgroundCompileRequest* req) {
      utilities::thread::Mutex::LockGuard guard(mutex_);
      pending_requests_.push_back(req);
      condition_.signal();
    }

    void stop() {
      {
        utilities::thread::Mutex::LockGuard guard(mutex_);
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
        utilities::thread::Mutex::LockGuard guard(mutex_);
        state = cStopped;
      }
    }

    void start() {
      utilities::thread::Mutex::LockGuard guard(mutex_);
      if(state != cStopped) return;
      state = cUnknown;
      stop_ = false;
      pause_ = false;
      paused_ = false;
      run();
    }

    void pause() {
      utilities::thread::Mutex::LockGuard guard(mutex_);

      // it's idle, ie paused.
      if(state == cIdle || state == cPaused) return;

      pause_ = true;

      while(!paused_) {
        pause_condition_.wait(mutex_);
      }
    }

    void unpause() {
      utilities::thread::Mutex::LockGuard guard(mutex_);

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
      ManagedThread::set_current(ls_, "rbx.jit");

#ifndef RBX_WINDOWS
      sigset_t set;
      sigfillset(&set);
      pthread_sigmask(SIG_SETMASK, &set, NULL);
#endif

      for(;;) { // forever

        BackgroundCompileRequest* req = 0;

        // Lock, wait, get a request, unlock
        {
          utilities::thread::Mutex::LockGuard guard(mutex_);

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

          state = cRunning;
        }

        // This isn't ideal, but it's the safest. Keep the GC from
        // running while we're building the IR.
        ls_->shared().gc_dependent(ls_);

        // mutex now unlock, allowing others to push more requests
        //

        current_req_ = req;

        jit::Compiler jit(ls_);
        current_compiler_ = &jit;

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
          if(utilities::thread::Condition* cond = req->waiter()) {
            cond->signal();
          }

          current_req_ = 0;
          current_compiler_ = 0;
          pending_requests_.pop_front();
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
        if(!req->machine_code()->jit_disabled()) {

          jit::RuntimeDataHolder* rd = jit.context().runtime_data_holder();

          atomic::memory_barrier();
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

          // assert(req->method()->jit_data());

          ls_->end_method_update();

          rd->run_write_barrier(ls_->write_barrier(), req->method());

          ls_->shared().stats.jitted_methods++;

          if(ls_->config().jit_show_compiling) {
            CompiledCode* code = req->method();
            llvm::outs() << "[[[ JIT finished background compiling "
                      << ls_->enclosure_name(code) << "#" << ls_->symbol_debug_str(code->name())
                      << (req->is_block() ? " (block)" : " (method)")
                      << " ]]]\n";
          }
        }

        // If someone was waiting on this, wake them up.
        if(utilities::thread::Condition* cond = req->waiter()) {
          cond->signal();
        }

        current_req_ = 0;
        current_compiler_ = 0;
        pending_requests_.pop_front();
        delete req;

        // We don't depend on the GC here, so let it run independent
        // of us.
        ls_->shared().gc_independent(ls_);
      }
    }

    void gc_scan(GarbageCollector* gc) {
      utilities::thread::Mutex::LockGuard guard(mutex_);

      for(std::list<BackgroundCompileRequest*>::iterator i = pending_requests_.begin();
          i != pending_requests_.end();
          ++i)
      {
        BackgroundCompileRequest* req = *i;
        if(Object* obj = gc->saw_object(req->method())) {
          req->set_method(force_as<CompiledCode>(obj));
        }

        if(Object* obj = gc->saw_object(req->extra())) {
          req->set_extra(obj);
        }
      }

      if(current_compiler_) {
        jit::RuntimeDataHolder* rd = current_compiler_->context().runtime_data_holder();
        rd->set_mark();

        ObjectMark mark(gc);
        rd->mark_all(current_req_->method(), mark);
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

  void LLVMState::add_internal_functions() { }

  static const bool debug_search = false;

  LLVMState::LLVMState(STATE)
    : AuxiliaryThread()
    , ManagedThread(state->shared().new_thread_id(),
                    state->shared(), ManagedThread::eSystem)
    , ctx_(llvm::getGlobalContext())
    , config_(state->shared().config)
    , symbols_(state->shared().symbols)
    , jitted_methods_(0)
    , queued_methods_(0)
    , accessors_inlined_(0)
    , uncommons_taken_(0)
    , shared_(state->shared())
    , include_profiling_(state->shared().config.jit_profile)
    , code_bytes_(0)
    , log_(0)
    , time_spent(0)
  {

    state->shared().auxiliary_threads()->register_thread(this);
    state->shared().add_managed_thread(this);
    state->shared().om->add_aux_barrier(state, &write_barrier_);

    if(state->shared().config.jit_log.value.size() == 0) {
      log_ = &std::cerr;
    } else {
      std::ofstream* s = new std::ofstream(
          state->shared().config.jit_log.value.c_str(), std::ios::out);

      if(s->fail()) {
        delete s;
        log_ = &std::cerr;
      } else {
        log_ = s;
      }
    }

#if RBX_LLVM_API_VER <= 300
    llvm::NoFramePointerElim = true;
#endif
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

    llvm::EngineBuilder factory(module_);
    factory.setAllocateGVsWithCode(false);

#if RBX_LLVM_API_VER > 300
    llvm::TargetOptions opts;
    opts.NoFramePointerElim = true;
    opts.NoFramePointerElimNonLeaf = true;

    factory.setTargetOptions(opts);
#endif

    engine_ = factory.create();

    passes_ = new llvm::FunctionPassManager(module_);

#if RBX_LLVM_API_VER >= 302
    passes_->add(new llvm::DataLayout(*engine_->getDataLayout()));
#else
    passes_->add(new llvm::TargetData(*engine_->getTargetData()));
#endif

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

    type_optz_ = state->shared().config.jit_type_optz;

    background_thread_ = new BackgroundCompilerThread(this);
    background_thread_->run();
  }

  LLVMState::~LLVMState() {
    shared_.auxiliary_threads()->unregister_thread(this);

    shared_.remove_managed_thread(this);
    shared_.om->del_aux_barrier(&write_barrier_);
    delete passes_;
    delete engine_;
    delete background_thread_;
  }

  bool LLVMState::debug_p() {
    return config_.jit_debug;
  }

  void LLVMState::shutdown(STATE) {
    background_thread_->stop();
  }

  void LLVMState::before_exec(STATE) {
    background_thread_->stop();
  }

  void LLVMState::after_exec(STATE) {
    background_thread_->start();
  }

  void LLVMState::before_fork(STATE) {
    background_thread_->pause();
  }

  void LLVMState::after_fork_parent(STATE) {
    background_thread_->unpause();
  }

  void LLVMState::after_fork_child(STATE) {
    shared_.add_managed_thread(this);
    background_thread_->restart();
  }

  void LLVMState::gc_scan(GarbageCollector* gc) {
    background_thread_->gc_scan(gc);
  }

  Symbol* LLVMState::symbol(const std::string& sym) {
    return symbols_.lookup(&shared_, sym);
  }

  std::string LLVMState::symbol_debug_str(const Symbol* sym) {
    if(sym == reinterpret_cast<const Symbol*>(cNil)) return "<nil>";
    return symbols_.lookup_debug_string(sym);
  }

  std::string LLVMState::enclosure_name(CompiledCode* code) {
    ConstantScope* cs = code->scope();
    if(!kind_of<ConstantScope>(cs) || !kind_of<Module>(cs->module())) {
      return "ANONYMOUS";
    }

    return symbol_debug_str(cs->module()->module_name());
  }

  void LLVMState::compile_soon(STATE, GCToken gct, CompiledCode* code, CallFrame* call_frame,
                               Object* placement, bool is_block)
  {
    bool wait = config().jit_sync;

    // Ignore it!
    if(code->machine_code()->call_count <= 1) {
      // if(config().jit_inline_debug) {
        // log() << "JIT: ignoring candidate! "
          // << symbol_debug_str(code->name()) << "\n";
      // }
      return;
    }

    if(debug_search) {
      if(is_block) {
        std::cout << "JIT: queueing block inside: "
          << enclosure_name(code) << "#" << symbol_debug_str(code->name()) << std::endl;
      } else {
        std::cout << "JIT: queueing method: "
          << enclosure_name(code) << "#" << symbol_debug_str(code->name()) << std::endl;
      }
    }

    // Don't do this because it prevents other class from heating
    // it up too!
    code->machine_code()->call_count = -1;

    BackgroundCompileRequest* req =
      new BackgroundCompileRequest(state, code, placement, is_block);

    queued_methods_++;

    if(wait) {
      wait_mutex.lock();

      req->set_waiter(&wait_cond);

      background_thread_->add(req);

      state->set_call_frame(call_frame);
      state->gc_independent(gct);

      wait_cond.wait(wait_mutex);

      wait_mutex.unlock();
      state->gc_dependent();
      state->set_call_frame(0);
      // if(config().jit_inline_debug) {
        // if(block) {
          // log() << "JIT: compiled block inside: "
                // << symbol_debug_str(code->name()) << "\n";
        // } else {
          // log() << "JIT: compiled method: "
                // << symbol_debug_str(code->name()) << "\n";
        // }
      // }
    } else {
      background_thread_->add(req);

      if(state->shared().config.jit_show_compiling) {
        llvm::outs() << "[[[ JIT queued "
          << enclosure_name(code) << "#" << symbol_debug_str(code->name())
          << (req->is_block() ? " (block)" : " (method)")
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

  void LLVMState::compile_callframe(STATE, GCToken gct, CompiledCode* start, CallFrame* call_frame,
                                    int primitive) {

    if(debug_search) {
      std::cout << std::endl << "JIT:       triggered: "
            << enclosure_name(start) << "#"
            << symbol_debug_str(start->name()) << std::endl;
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
        std::cout << "JIT: invalid candidate returned" << std::endl;
      }

      return;
    }

    if(debug_search) {
      std::cout << "! ";
      candidate->print_backtrace(state, 1);
    }

    if(start && candidate->compiled_code != start) {
      start->machine_code()->call_count = 0;
    }

    if(candidate->compiled_code->machine_code()->call_count <= 1) {
      if(!start || start->machine_code()->jitted()) return;
      // Ignore it. compile this one.
      candidate = call_frame;
    }

    if(candidate->block_p()) {
      compile_soon(state, gct, candidate->compiled_code, call_frame, candidate->block_env(), true);
    } else {
      if(candidate->compiled_code->can_specialize_p()) {
        compile_soon(state, gct, candidate->compiled_code, call_frame, candidate->self()->class_object(state));
      } else {
        compile_soon(state, gct, candidate->compiled_code, call_frame, cNil);
      }
    }
  }

  CallFrame* LLVMState::find_candidate(STATE, CompiledCode* start, CallFrame* call_frame) {
    if(!config_.jit_inline_generic) {
      return call_frame;
    }

    int depth = cInlineMaxDepth;

    if(!start) rubinius::bug("null start");
    if(!call_frame) rubinius::bug("null call_frame");

    // if(!start) {
      // start = call_frame->compiled_code;
      // call_frame = call_frame->previous;
      // depth--;
    // }

    if(debug_search) {
      std::cout << "> call_count: " << call_frame->compiled_code->machine_code()->call_count
            << " size: " << call_frame->compiled_code->machine_code()->total
            << " sends: " << call_frame->compiled_code->machine_code()->inline_cache_count()
            << std::endl;

      call_frame->print_backtrace(state, 1);
    }

    if(start->machine_code()->total > (size_t)config_.jit_max_method_inline_size) {
      if(debug_search) {
        std::cout << "JIT: STOP. reason: trigger method isn't small: "
              << start->machine_code()->total << " > "
              << config_.jit_max_method_inline_size
              << std::endl;
      }

      return call_frame;
    }

    MachineCode* mcode = start->machine_code();

    if(mcode->required_args != mcode->total_args) {
      if(debug_search) {
        std::cout << "JIT: STOP. reason: trigger method req_args != total_args" << std::endl;
      }

      return call_frame;
    }

    if(mcode->no_inline_p()) {
      if(debug_search) {
        std::cout << "JIT: STOP. reason: trigger method no_inline_p() = true" << std::endl;
      }

      return call_frame;
    }

    CallFrame* callee = call_frame;
    call_frame = call_frame->previous;

    if(!call_frame) return callee;

    // Now start looking at callers.

    while(depth-- > 0) {
      CompiledCode* cur = call_frame->compiled_code;

      if(!cur) {
        if(debug_search) {
          std::cout << "JIT: STOP. reason: synthetic CallFrame hit" << std::endl;
        }
        return callee;
      }

      MachineCode* mcode = cur->machine_code();

      if(debug_search) {
        std::cout << "> call_count: " << mcode->call_count
              << " size: " << mcode->total
              << " sends: " << mcode->inline_cache_count()
              << std::endl;

        call_frame->print_backtrace(state, 1);
      }


      /*
      if(call_frame->block_p()
          || mcode->required_args != mcode->total_args // has a splat
          || mcode->call_count < 200 // not called much
          || mcode->jitted() // already jitted
          || mcode->parent() // is a block
        ) return callee;
      */

      if(mcode->required_args != mcode->total_args) {
        if(debug_search) {
          std::cout << "JIT: STOP. reason: req_args != total_args" << std::endl;
        }
        return callee;
      }

      if(mcode->call_count < config_.jit_call_inline_threshold) {
        if(debug_search) {
          std::cout << "JIT: STOP. reason: call_count too small: "
                << mcode->call_count << " < "
                << config_.jit_call_inline_threshold << std::endl;
        }

        return callee;
      }

      if(mcode->jitted()) {
        if(debug_search) {
          std::cout << "JIT: STOP. reason: already jitted" << std::endl;
        }

        return callee;
      }

      if(mcode->no_inline_p()) {
        if(debug_search) {
          std::cout << "JIT: STOP. reason: no_inline_p() = true" << std::endl;
        }

        return callee;
      }

      if(mcode->inline_cache_count() > eMaxInlineSendCount) {
        if(debug_search) {
          std::cout << "JIT: STOP. reason: high send count" << std::endl;
        }

        return call_frame;
      }

      // if(mcode->required_args != mcode->total_args // has a splat
          // || mcode->call_count < 200 // not called much
          // || mcode->jitted() // already jitted
          // || !mcode->no_inline_p() // method marked as not inlineable
        // ) return callee;

      CallFrame* prev = call_frame->previous;

      if(!prev) {
        if(debug_search) {
          std::cout << "JIT: STOP. reason: toplevel method" << std::endl;
        }
        return call_frame;
      }

      // if(cur->machine_code()->total > SMALL_METHOD_SIZE) {
        // if(debug_search) {
          // std::cout << "JIT: STOP. reason: big method: "
                // << cur->machine_code()->total << " > "
                // << SMALL_METHOD_SIZE
                // << "\n";
        // }

        // return call_frame;
      // }

      // if(!next || cur->machine_code()->total > SMALL_METHOD_SIZE) return call_frame;

      callee->compiled_code->machine_code()->call_count = 0;

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
          if(dladdr((void*)addr, &info)) {
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
