#ifndef VM_LLVM_STATE_HPP
#define VM_LLVM_STATE_HPP

#include <stdint.h>
#include <unistd.h>

#include "config.h"

#if RBX_LLVM_API_VER >= 303
#include <llvm/IR/Module.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#else
#include <llvm/Module.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Function.h>
#include <llvm/Instructions.h>
#endif
#if RBX_LLVM_API_VER >= 303
#include <llvm/IR/IRBuilder.h>
#elif RBX_LLVM_API_VER >= 302
#include <llvm/IRBuilder.h>
#else
#include <llvm/Support/IRBuilder.h>
#endif
#ifdef RBX_LLVM_MCJIT_ENABLED
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/ExecutionEngine/ObjectImage.h"
#include "llvm/ADT/DenseMapInfo.h"
#include "llvm/Object/ObjectFile.h"
#include "llvm/ExecutionEngine/Interpreter.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#else
#include <llvm/ExecutionEngine/JIT.h>
#endif
#include <llvm/CodeGen/MachineCodeInfo.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>
#include <llvm/Pass.h>
#include <llvm/PassManager.h>
#include <llvm/Support/raw_ostream.h>
#if RBX_LLVM_API_VER >= 303
#include <llvm/IR/LLVMContext.h>
#else
#include <llvm/LLVMContext.h>
#endif
#include <llvm/ExecutionEngine/JITEventListener.h>

#include "llvm/local_info.hpp"

#include "gc/managed.hpp"
#include "auxiliary_threads.hpp"
#include "configuration.hpp"
#include "lock.hpp"
#include "metrics.hpp"
#include "util/thread.hpp"

namespace rubinius {
  typedef std::map<int, LocalInfo> LocalMap;
  class SymbolTable;
  class CompiledCode;
  class GarbageCollector;

  namespace jit {
    class Builder;
    class Compiler;
    class RubiniusJITMemoryManager;
  }

  class JITCompileRequest;
  class BlockEnvironment;
  class Context;
  class List;
  class Symbol;
  class Thread;

  enum JitDebug {
    cSimple = 1,
    cOptimized = 2,
    cMachineCode = 4
  };

  class LLVMState : public AuxiliaryThread, public Lockable {
    jit::RubiniusJITMemoryManager* memory_;
    llvm::JITEventListener* jit_event_listener_;

    Configuration& config_;

    VM* vm_;
    TypedRoot<Thread*> thread_;
    TypedRoot<List*> compile_list_;
    SymbolTable& symbols_;

    int jitted_methods_;
    int accessors_inlined_;
    int uncommons_taken_;

    SharedState& shared_;
    bool include_profiling_;

    int code_bytes_;

    std::ostream* log_;

    uint32_t fixnum_class_id_;
    uint32_t integer_class_id_;
    uint32_t numeric_class_id_;
    uint32_t bignum_class_id_;
    uint32_t float_class_id_;
    uint32_t symbol_class_id_;
    uint32_t string_class_id_;
    uint32_t regexp_class_id_;
    uint32_t encoding_class_id_;
    uint32_t module_class_id_;
    uint32_t class_class_id_;
    uint32_t nil_class_id_;
    uint32_t true_class_id_;
    uint32_t false_class_id_;
    uint32_t array_class_id_;
    uint32_t tuple_class_id_;

    bool type_optz_;

    bool enabled_;
    bool thread_exit_;

    jit::Compiler* current_compiler_;

    utilities::thread::SpinLock method_update_lock_;
    utilities::thread::Mutex wait_mutex;
    utilities::thread::Condition wait_cond;
    utilities::thread::Mutex request_lock_;
    utilities::thread::Mutex compile_lock_;
    utilities::thread::Condition compile_cond_;

    std::string cpu_;

  public:

    LLVMState(STATE);
    virtual ~LLVMState();

    void add_internal_functions();
    void enable(STATE);

    bool enabled() {
      return enabled_;
    }

    int jit_dump_code() {
      return config_.jit_dump_code;
    }

    bool debug_p();

    Configuration& config() {
      return config_;
    }

    bool include_profiling() {
      return include_profiling_;
    }

    jit::RubiniusJITMemoryManager* memory() { return memory_; }
    llvm::JITEventListener* jit_event_listener() { return jit_event_listener_; }

    int code_bytes() {
      return code_bytes_;
    }

    void add_code_bytes(int bytes) {
      code_bytes_ += bytes;
    }

    void add_accessor_inlined() {
      accessors_inlined_++;
    }

    int accessors_inlined() {
      return accessors_inlined_;
    }

    void add_uncommons_taken() {
      uncommons_taken_++;
    }

    int uncommons_taken() {
      return uncommons_taken_;
    }

    SharedState& shared() { return shared_; }

    std::ostream& log() {
      return *log_;
    }

    VM* vm() {
      return vm_;
    }

    uint32_t fixnum_class_id() {
      return fixnum_class_id_;
    }

    uint32_t integer_class_id() {
      return integer_class_id_;
    }

    uint32_t numeric_class_id() {
      return numeric_class_id_;
    }

    uint32_t bignum_class_id() {
      return bignum_class_id_;
    }

    uint32_t float_class_id() {
      return float_class_id_;
    }

    uint32_t symbol_class_id() {
      return symbol_class_id_;
    }

    uint32_t string_class_id() {
      return string_class_id_;
    }

    uint32_t regexp_class_id() {
      return regexp_class_id_;
    }

    uint32_t encoding_class_id() {
      return encoding_class_id_;
    }

    uint32_t module_class_id() {
      return module_class_id_;
    }

    uint32_t class_class_id() {
      return class_class_id_;
    }

    uint32_t nil_class_id() {
      return nil_class_id_;
    }

    uint32_t true_class_id() {
      return true_class_id_;
    }

    uint32_t false_class_id() {
      return false_class_id_;
    }

    uint32_t array_class_id() {
      return array_class_id_;
    }

    uint32_t tuple_class_id() {
      return tuple_class_id_;
    }

    bool type_optz() {
      return type_optz_;
    }

    std::string cpu() {
      return cpu_;
    }

    void start_method_update() {
      method_update_lock_.lock();
    }

    void end_method_update() {
      method_update_lock_.unlock();
    }

    void compile_soon(STATE, GCToken gct, CompiledCode* code, CallFrame* call_frame,
                      Class* receiver_class, BlockEnvironment* block_env = NULL, bool is_block=false);

    void add(STATE, JITCompileRequest* req);
    void remove(void* func);

    CallFrame* find_candidate(STATE, CompiledCode* start, CallFrame* call_frame);
    void compile_callframe(STATE, GCToken gct, CompiledCode* start, CallFrame* call_frame,
                           int primitive = -1);

    Symbol* symbol(const std::string& sym);
    std::string symbol_debug_str(const Symbol* sym);

    std::string enclosure_name(CompiledCode* code);

    void perform(STATE);
    void stop(STATE);

    void start_thread(STATE);
    void stop_thread(STATE);

    void after_fork_child(STATE);
    void after_fork_exec_child(STATE);

    void gc_scan(GarbageCollector* gc);

    static void show_machine_code(void* impl, size_t bytes);
  };

}

#endif
