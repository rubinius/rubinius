#ifdef ENABLE_LLVM

#include "llvm/state.hpp"

#include "llvm/jit_compiler.hpp"
#include "llvm/jit_method.hpp"
#include "llvm/jit_block.hpp"
#include "llvm/method_info.hpp"
#include "llvm/disassembler.hpp"
#include "llvm/jit_context.hpp"
#include "llvm/jit_memory_manager.hpp"
#include "llvm/detection.hpp"

#include "builtin/fixnum.hpp"
#include "builtin/constant_scope.hpp"
#include "builtin/module.hpp"
#include "builtin/compiled_code.hpp"
#include "builtin/class.hpp"
#include "builtin/block_environment.hpp"
#include "builtin/jit.hpp"
#include "builtin/list.hpp"
#include "builtin/thread.hpp"

#include "internal_threads.hpp"
#include "machine_code.hpp"
#include "field_offset.hpp"
#include "object_memory.hpp"
#include "on_stack.hpp"

#include "call_frame.hpp"
#include "configuration.hpp"
#include "instruments/timing.hpp"
#include "dtrace/dtrace.h"

#if RBX_LLVM_API_VER >= 303
#include <llvm/IR/DataLayout.h>
#elif RBX_LLVM_API_VER >= 302
#include <llvm/DataLayout.h>
#else
#include <llvm/Target/TargetData.h>
#endif
#if RBX_LLVM_API_VER >= 305
#include <llvm/IR/Verifier.h>
#include <llvm/IR/CFG.h>
#else
#include <llvm/Analysis/Verifier.h>
#include <llvm/Support/CFG.h>
#endif
#if RBX_LLVM_API_VER >= 303
#include <llvm/IR/CallingConv.h>
#else
#include <llvm/CallingConv.h>
#endif
#include <llvm/Analysis/Passes.h>
// #include <llvm/LinkAllPasses.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/Format.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Transforms/Scalar.h>

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

  static const bool debug_search = false;

  LLVMState::LLVMState(STATE)
    : InternalThread(state, "rbx.jit", InternalThread::eXLarge)
    , config_(state->shared().config)
    , compile_list_(state)
    , symbols_(state->shared().symbols)
    , accessors_inlined_(0)
    , uncommons_taken_(0)
    , shared_(state->shared())
    , include_profiling_(state->shared().config.jit_profile)
    , code_bytes_(0)
    , log_(NULL)
    , enabled_(false)
    , current_compiler_(0)
  {
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
    llvm::InitializeNativeTargetAsmPrinter();

    memory_ = new jit::RubiniusJITMemoryManager();
#if RBX_LLVM_API_VER > 300
    jit_event_listener_ = llvm::JITEventListener::createOProfileJITEventListener();
#else
    jit_event_listener_ = llvm::createOProfileJITEventListener();
#endif

    fixnum_class_id_   = G(fixnum_class)->class_id();
    integer_class_id_  = G(integer)->class_id();
    numeric_class_id_  = G(numeric)->class_id();
    bignum_class_id_   = G(bignum)->class_id();
    float_class_id_    = G(floatpoint)->class_id();
    symbol_class_id_   = G(symbol)->class_id();
    string_class_id_   = G(string)->class_id();
    regexp_class_id_   = G(regexp)->class_id();
    module_class_id_   = G(module)->class_id();
    class_class_id_    = G(klass)->class_id();
    nil_class_id_      = G(nil_class)->class_id();
    true_class_id_     = G(true_class)->class_id();
    false_class_id_    = G(false_class)->class_id();
    array_class_id_    = G(array)->class_id();
    tuple_class_id_    = G(tuple)->class_id();

    type_optz_ = state->shared().config.jit_type_optz;

    cpu_ = rubinius::getHostCPUName();

    compile_list_.set(G(jit)->compile_list());
  }

  LLVMState::~LLVMState() {
    delete memory_;
    memory_ = NULL;

    delete jit_event_listener_;
    jit_event_listener_ = NULL;

    if(log_ != &std::cerr) {
      delete log_;
      log_ = NULL;
    }
  }

  void LLVMState::enable(STATE) {
    utilities::thread::SpinLock::LockGuard lg(shared_.llvm_state_lock());

    if(enabled_) return;

    start(state);
  }

  bool LLVMState::debug_p() {
    return config_.jit_debug;
  }

  void LLVMState::initialize(STATE) {
    InternalThread::initialize(state);

    method_update_lock_.init();
    wait_mutex.init();
    wait_cond.init();
    request_lock_.init();
    compile_lock_.init();
    compile_cond_.init();

    enabled_ = true;
  }

  void LLVMState::wakeup(STATE) {
    InternalThread::wakeup(state);

    compile_cond_.signal();
    wait_cond.signal();
  }

  void LLVMState::stop(STATE) {
    enabled_ = false;
    InternalThread::stop(state);
  }

  void LLVMState::after_fork_child(STATE) {
    compile_list_.get()->clear(state);
    current_compiler_ = 0;

    InternalThread::after_fork_child(state);
  }

  void LLVMState::run(STATE) {
    GCTokenImpl gct;
    JITCompileRequest* compile_request = nil<JITCompileRequest>();
    OnStack<1> os(state, compile_request);

    metrics().init(metrics::eJITMetrics);

    state->gc_dependent(gct, 0);

    bool show_machine_code_ = jit_dump_code() & cMachineCode;

    while(!thread_exit_) {

      current_compiler_ = 0;

      {
        GCIndependent guard(state, 0);

        {
          utilities::thread::Mutex::LockGuard lg(compile_lock_);

          while(compile_list_.get()->empty_p()) {
            compile_cond_.wait(compile_lock_);

            if(thread_exit_) break;
          }
        }
      }

      if(thread_exit_) break;

      {
        utilities::thread::Mutex::LockGuard guard(request_lock_);

        compile_request = try_as<JITCompileRequest>(compile_list_.get()->shift(state));
        if(!compile_request || compile_request->nil_p()) continue;
      }

      utilities::thread::Condition* cond = compile_request->waiter();

      // Don't proceed until requester has reached the wait_cond
      if(cond) wait_mutex.lock();

      Context ctx(this);
      jit::Compiler jit(&ctx);

      current_compiler_ = &jit;

      uint32_t class_id = 0;
      uint32_t serial_id = 0;
      void* func = 0;

      try {
        if(compile_request->receiver_class() &&
            !compile_request->receiver_class()->nil_p()) {
          // Apparently already compiled, probably some race
          if(compile_request->method()->find_specialized(
                compile_request->receiver_class())) {
            if(config().jit_show_compiling) {
              CompiledCode* code = compile_request->method();
              llvm::outs() << "[[[ JIT already compiled "
                        << enclosure_name(code) << "#" << symbol_debug_str(code->name())
                        << (compile_request->is_block() ? " (block)" : " (method)")
                        << " ]]]\n";
            }

            // If someone was waiting on this, wake them up.
            if(cond) {
              wait_mutex.unlock();
              cond->signal();
            }

            current_compiler_ = 0;

            continue;
          }

          class_id = compile_request->receiver_class()->class_id();
          serial_id = compile_request->receiver_class()->serial_id();
        }

        {
          timer::StopWatch<timer::microseconds> timer(
              metrics().m.jit_metrics.time_us);

          jit.compile(compile_request);

          bool indy = !config().jit_sync;
          func = jit.generate_function(indy);
        }

        // We were unable to compile this function, likely
        // because it's got something we don't support.
        if(!func) {
          if(config().jit_show_compiling) {
            CompiledCode* code = compile_request->method();
            llvm::outs() << "[[[ JIT error background compiling "
                      << enclosure_name(code) << "#" << symbol_debug_str(code->name())
                      << (compile_request->is_block() ? " (block)" : " (method)")
                      << " ]]]\n";
          }
          // If someone was waiting on this, wake them up.
          if(cond) {
            wait_mutex.unlock();
            cond->signal();
          }

          current_compiler_ = 0;

          continue;
        }
      } catch(LLVMState::CompileError& e) {
        utilities::logger::warn("JIT: compile error: %s", e.error());

        metrics().m.jit_metrics.methods_failed++;

        // If someone was waiting on this, wake them up.
        if(cond) {
          wait_mutex.unlock();
          cond->signal();
        }
        current_compiler_ = 0;

        continue;
      }

      if(show_machine_code_) {
        jit.show_machine_code();
      }

      // If the method has had jit'ing request disabled since we started
      // JIT'ing it, discard our work.
      if(!compile_request->machine_code()->jit_disabled()) {

        jit::RuntimeDataHolder* rd = ctx.runtime_data_holder();

        atomic::memory_barrier();
        start_method_update();

        if(!compile_request->is_block()) {
          if(class_id) {
            compile_request->method()->add_specialized(state,
                class_id, serial_id, reinterpret_cast<executor>(func), rd);
          } else {
            compile_request->method()->set_unspecialized(reinterpret_cast<executor>(func), rd);
          }
        } else {
          compile_request->method()->set_unspecialized(reinterpret_cast<executor>(func), rd);
        }

        compile_request->machine_code()->clear_compiling();

        end_method_update();

        rd->run_write_barrier(shared().om, compile_request->method());

        if(config().jit_show_compiling) {
          CompiledCode* code = compile_request->method();
          llvm::outs() << "[[[ JIT finished background compiling "
                    << enclosure_name(code) << "#" << symbol_debug_str(code->name())
                    << (compile_request->is_block() ? " (block)" : " (method)")
                    << " ]]]\n";
        }
      }

      // If someone was waiting on this, wake them up.
      if(cond) {
        wait_mutex.unlock();
        cond->signal();
      }

      current_compiler_ = 0;
      metrics().m.jit_metrics.methods_compiled++;
    }
  }

  void LLVMState::gc_scan(GarbageCollector* gc) {
    if(current_compiler_) {
      jit::RuntimeDataHolder* rd = current_compiler_->context()->runtime_data_holder();
      rd->set_mark();

      ObjectMark mark(gc);
      rd->mark_all(0, mark);
    }
  }

  Symbol* LLVMState::symbol(const std::string& sym) {
    return symbols_.lookup(&shared_, sym);
  }

  std::string LLVMState::symbol_debug_str(const Symbol* sym) {
    if(sym == nil<Symbol>()) return "<nil>";
    return symbols_.lookup_debug_string(sym);
  }

  std::string LLVMState::enclosure_name(CompiledCode* code) {
    ConstantScope* cs = code->scope();
    if(!kind_of<ConstantScope>(cs) || !kind_of<Module>(cs->module())) {
      return "ANONYMOUS";
    }

    return symbol_debug_str(cs->module()->module_name());
  }

  void LLVMState::add(STATE, JITCompileRequest* req) {
    utilities::thread::Mutex::LockGuard guard(request_lock_);

    if(!enabled_) return;

    G(jit)->compile_list()->append(state, req);
    metrics().m.jit_metrics.methods_queued++;

    compile_cond_.signal();
  }

  void LLVMState::compile(STATE, GCToken gct, CompiledCode* code, CallFrame* call_frame,
      Class* receiver_class, BlockEnvironment* block_env, bool is_block)
  {
    if(!enabled_) return;

    // In case the method hasn't been internalized yet
    if(!code->machine_code()) {
      code->internalize(state, gct, call_frame);
    }

    JITCompileRequest* req = JITCompileRequest::create(state, code, receiver_class,
        0, block_env, is_block);

    wait_mutex.lock();
    req->set_waiter(&wait_cond);

    add(state, req);

    state->set_call_frame(call_frame);

    {
      GCIndependent guard(state, call_frame);

      wait_cond.wait(wait_mutex);
    }

    wait_mutex.unlock();
    state->set_call_frame(0);

    if(state->shared().config.jit_show_compiling) {
      llvm::outs() << "[[[ JIT compiled "
        << enclosure_name(code) << "#" << symbol_debug_str(code->name())
        << (req->is_block() ? " (block) " : " (method) ") << " ]]]\n";
    }
  }

  void LLVMState::compile_soon(STATE, GCToken gct, CompiledCode* code, CallFrame* call_frame,
                               Class* receiver_class, BlockEnvironment* block_env, bool is_block)
  {
    bool wait = config().jit_sync;

    if(!enabled_) return;

    if(code->machine_code()->call_count <= 1) {
      return;
    }

    if(code->machine_code()->compiling_p()) {
      return;
    }

    int hits = code->machine_code()->method_call_count();
    code->machine_code()->set_compiling();

    JITCompileRequest* req = JITCompileRequest::create(state, code, receiver_class,
        hits, block_env, is_block);

    if(wait) {
      wait_mutex.lock();

      req->set_waiter(&wait_cond);

      add(state, req);
      bool req_block = req->is_block();

      state->set_call_frame(call_frame);

      {
        GCIndependent guard(state, call_frame);

        wait_cond.wait(wait_mutex);
      }

      wait_mutex.unlock();
      state->set_call_frame(0);

      if(state->shared().config.jit_show_compiling) {
        llvm::outs() << "[[[ JIT compiled "
          << enclosure_name(code) << "#" << symbol_debug_str(code->name())
          << (req_block ? " (block) " : " (method) ")
          << " (" << hits << ") " << " ]]]\n";
      }
    } else {
      add(state, req);

      if(state->shared().config.jit_show_compiling) {
        llvm::outs() << "[[[ JIT queued "
          << enclosure_name(code) << "#" << symbol_debug_str(code->name())
          << (req->is_block() ? " (block) " : " (method) ")
          << " (" << hits << ") " << " ]]]\n";
      }
    }
  }

  void LLVMState::remove(void* func) {
    metrics().m.jit_metrics.methods_compiled--;
    if(memory_) memory_->deallocateFunctionBody(func);
  }

  const static size_t eMaxInlineSendCount = 10;

  void LLVMState::compile_callframe(STATE, GCToken gct, CompiledCode* start,
      CallFrame* call_frame, int primitive)
  {
    if(debug_search) {
      std::cout << std::endl << "JIT:       triggered: "
            << enclosure_name(start) << "#"
            << symbol_debug_str(start->name()) << std::endl;
    }

   CallFrame* candidate = find_candidate(state, start, call_frame);
    if(!candidate || candidate->jitted_p() || candidate->inline_method_p()) {
      if(config().jit_show_compiling) {
        llvm::outs() << "[[[ JIT error finding call frame "
                      << enclosure_name(start) << "#" << symbol_debug_str(start->name());
        if(!candidate) {
          llvm::outs() << " no candidate";
        } else {
          if(candidate->jitted_p()) {
           llvm::outs() << " candidate is jitted";
          }
          if(candidate->inline_method_p()) {
            llvm::outs() << " candidate is inlined";
          }
        }

        llvm::outs() << " ]]]\n";
      }
      return;
    }

    if(debug_search) {
      std::cout << "! ";
      candidate->print_backtrace(state, 1);
    }

    if(candidate->compiled_code->machine_code()->call_count <= 1) {
      if(!start || start->machine_code()->jitted_p()) return;
      // Ignore it. compile this one.
      candidate = call_frame;
    }

    if(candidate->block_p()) {
      compile_soon(state, gct, candidate->compiled_code, call_frame,
                   candidate->self()->direct_class(state), candidate->block_env(), true);
    } else {
      if(candidate->compiled_code->can_specialize_p()) {
        compile_soon(state, gct, candidate->compiled_code, call_frame,
                     candidate->self()->direct_class(state));
      } else {
        compile_soon(state, gct, candidate->compiled_code, call_frame, NULL);
      }
    }
  }

  CallFrame* LLVMState::find_candidate(STATE, CompiledCode* start, CallFrame* call_frame) {
    if(!config_.jit_inline_generic) {
      return call_frame;
    }

    int depth = config().jit_limit_search;

    if(!start) {
      throw CompileError("find_candidate: null start");
    }

    if(!call_frame) {
      throw CompileError("find_candidate: null call frame");
    }

    // if(!start) {
      // start = call_frame->compiled_code;
      // call_frame = call_frame->previous;
      // depth--;
    // }

    if(debug_search) {
      std::cout << "> call_count: " << call_frame->compiled_code->machine_code()->call_count
            << " size: " << call_frame->compiled_code->machine_code()->total
            << " sends: " << call_frame->compiled_code->machine_code()->call_site_count()
            << std::endl;

      call_frame->print_backtrace(state, 1);
    }

    if(start->machine_code()->total > (size_t)config_.jit_limit_inline_method) {
      if(debug_search) {
        std::cout << "JIT: STOP. reason: trigger method isn't small: "
              << start->machine_code()->total << " > "
              << config_.jit_limit_inline_method
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
              << " sends: " << mcode->call_site_count()
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

      if(mcode->call_count < config_.jit_threshold_inline) {
        if(debug_search) {
          std::cout << "JIT: STOP. reason: call_count too small: "
                << mcode->call_count << " < "
                << config_.jit_threshold_inline << std::endl;
        }

        return callee;
      }

      if(mcode->jitted_p()) {
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

      if(call_frame->jitted_p() || call_frame->inline_method_p()) {
        return callee;
      }

      if(mcode->call_site_count() > eMaxInlineSendCount) {
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
#ifdef IS_64BIT_ARCH
    ud_set_mode(&ud, 64);
#else
    ud_set_mode(&ud, 32);
#endif
    ud_set_syntax(&ud, UD_SYN_ATT);
    ud_set_input_buffer(&ud, reinterpret_cast<uint8_t*>(buffer), size);

    while(ud_disassemble(&ud)) {
      void* address = reinterpret_cast<void*>(
          reinterpret_cast<uintptr_t>(buffer) + ud_insn_off(&ud));

      llvm::outs() << format("%10p", address) << "  ";
      llvm::outs() << format("%-24s", ud_insn_asm(&ud));
      if(ud.operand[0].type == UD_OP_JIMM) {
        const void* addr = (const void*)((uintptr_t)buffer + ud.pc + (int)ud.operand[0].lval.udword);
        llvm::outs() << " ; " << addr;
        if(ud.mnemonic == UD_Icall) {
          Dl_info info;
          if(dladdr((void*)addr, &info)) {
            int status = 0;
            char* cpp_name = abi::__cxa_demangle(info.dli_sname, 0, 0, &status);
            if(status >= 0) {
              // Chop off the arg info from the signature output
              char *paren = strstr(cpp_name, "(");
              *paren = 0;
              llvm::outs() << " " << cpp_name;
              free(cpp_name);
            } else {
              llvm::outs() << " " << info.dli_sname;
            }
          }
        }
      }

      for(uint8_t i = 0; i < 2; i++) {
        if(ud.operand[i].type == UD_OP_IMM) {
          Dl_info info;
          if(dladdr((void*)ud.operand[i].lval.uqword, &info)) {
            llvm::outs() << " ; " << info.dli_sname;
            break; // only do one
          }
        }
      }

      llvm::outs() << "\n";
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
