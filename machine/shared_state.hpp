#ifndef RBX_SHARED_STATE_H
#define RBX_SHARED_STATE_H

#include "config.h"

#include "memory/variable_buffer.hpp"
#include "memory/root_buffer.hpp"

#include "globals.hpp"
#include "jit.hpp"
#include "machine_threads.hpp"
#include "primitives.hpp"
#include "spinlock.hpp"
#include "symbol_table.hpp"
#include "thread_nexus.hpp"

#include "diagnostics.hpp"

#include "util/thread.hpp"

#include "capi/capi_constants.h"

#include <unistd.h>
#include <atomic>
#include <functional>
#include <string>
#include <unordered_set>
#include <vector>

#ifdef RBX_WINDOWS
#include <winsock2.h>
#endif

namespace rubinius {
  namespace console {
    class Console;
  }

  namespace memory {
    class FinalizerThread;
    class ManagedThread;
  }

  class ConfigParser;
  class Configuration;
  class Environment;
  class Fiber;
  class Fixnum;
  class Memory;
  class QueryAgent;
  class SignalThread;
  class State;
  class VM;

  struct CallFrame;

  typedef std::unordered_set<std::string> CApiBlackList;
  typedef std::vector<utilities::thread::Mutex*> CApiLocks;
  typedef std::unordered_map<std::string, int> CApiLockMap;

  typedef std::vector<std::string> CApiConstantNameMap;
  typedef std::unordered_map<int, MemoryHandle*> CApiConstantHandleMap;

  /**
   * SharedState represents the global shared state that needs to be shared
   * across all VM instances.
   *
   * Rubinius makes no use of global variables; instead, all shared state is
   * stored in a reference counted instance of this class. This makes it
   * possible in theory to have multiple independent Rubinius runtimes in a
   * single process.
   */

  class SharedState {
  public:
    enum Phase {
      eBooting,
      eRunning,
      eHalting,
    };

  private:
    ThreadNexus* thread_nexus_;
    MachineThreads* machine_threads_;
    SignalThread* signals_;
    memory::FinalizerThread* finalizer_;
    console::Console* console_;
    jit::JIT* jit_;

    diagnostics::Diagnostics* diagnostics_;
    diagnostics::BootMetrics* boot_metrics_;
    diagnostics::CodeDBMetrics* codedb_metrics_;
    diagnostics::GCMetrics* gc_metrics_;
    diagnostics::MemoryMetrics* memory_metrics_;
    diagnostics::Profiler* profiler_;

    CApiConstantNameMap capi_constant_name_map_;
    CApiConstantHandleMap capi_constant_map_;

    uint64_t start_time_;
    uint64_t  class_count_;
    int global_serial_;

    bool initialized_;
    bool check_global_interrupts_;
    bool check_gc_;

    VM* root_vm_;
    Environment* env_;

    utilities::thread::Mutex codedb_lock_;

    utilities::thread::SpinLock capi_ds_lock_;
    utilities::thread::SpinLock capi_locks_lock_;
    utilities::thread::SpinLock capi_constant_lock_;
    utilities::thread::SpinLock wait_lock_;
    utilities::thread::SpinLock type_info_lock_;
    utilities::thread::SpinLock code_resource_lock_;

    CApiBlackList capi_black_list_;
    CApiLocks capi_locks_;
    CApiLockMap capi_lock_map_;

    bool use_capi_lock_;
    int primitive_hits_[Primitives::cTotalPrimitives];

    std::atomic<Phase> phase_;

  public:
    Globals globals;
    Memory* om;
    Configuration& config;
    ConfigParser& user_variables;
    SymbolTable symbols;
    std::string nodename;
    std::string username;
    std::string pid;
    uint32_t hash_seed;

  public:
    SharedState(Environment* env, Configuration& config, ConfigParser& cp);
    ~SharedState();

    bool booting_p() {
      return phase_ == eBooting;
    }

    void set_booting() {
      phase_ = eBooting;
    }

    bool running_p() {
      return phase_ == eRunning;
    }

    void set_running() {
      phase_ = eRunning;
    }

    bool halting_p() {
      return phase_ == eHalting;
    }

    void set_halting() {
      phase_ = eHalting;
    }

    int size();

    void set_initialized() {
      setup_capi_constant_names();
      initialized_ = true;
    }

    double run_time();

    ThreadNexus* thread_nexus() {
      return thread_nexus_;
    }

    MachineThreads* machine_threads() const {
      return machine_threads_;
    }

    memory::FinalizerThread* finalizer() const {
      return finalizer_;
    }

    void set_finalizer(memory::FinalizerThread* thr) {
      finalizer_ = thr;
    }

    Array* vm_threads(STATE);
    Fixnum* vm_threads_count(STATE);
    Array* vm_fibers(STATE);
    Fixnum* vm_fibers_count(STATE);
    Array* vm_thread_fibers(STATE, Thread* thread);
    void vm_thread_fibers(STATE, Thread* thread, std::function<void (STATE, Fiber*)> f);

    int global_serial() const {
      return global_serial_;
    }

    int inc_global_serial(STATE) {
      return atomic::fetch_and_add(&global_serial_, (int)1);
    }

    uint32_t new_thread_id();

    int* global_serial_address() {
      return &global_serial_;
    }

    uint64_t inc_class_count(STATE) {
      return atomic::fetch_and_add(&class_count_, (uint64_t)1);
    }

    int inc_primitive_hit(int primitive) {
      return ++primitive_hits_[primitive];
    }

    int& primitive_hits(int primitive) {
      return primitive_hits_[primitive];
    }

    SignalThread* signals() const {
      return signals_;
    }

    SignalThread* start_signals(STATE);

    console::Console* console() const {
      return console_;
    }

    console::Console* start_console(STATE);

    diagnostics::Diagnostics* diagnostics() const {
      return diagnostics_;
    }

    diagnostics::Diagnostics* start_diagnostics(STATE);

    void report_diagnostics(diagnostics::Diagnostic* diagnostic);

    diagnostics::BootMetrics* boot_metrics() {
      return boot_metrics_;
    }

    diagnostics::CodeDBMetrics* codedb_metrics() {
      return codedb_metrics_;
    }

    diagnostics::GCMetrics* gc_metrics() {
      return gc_metrics_;
    }

    diagnostics::MemoryMetrics* memory_metrics() {
      return memory_metrics_;
    }


    diagnostics::Profiler* profiler() {
      return profiler_;
    }

    jit::JIT* start_jit(STATE);

    jit::JIT* jit() const {
      return jit_;
    }

    Environment* env() const {
      return env_;
    }

    void set_root_vm(VM* vm) {
      root_vm_ = vm;
    }

    VM* root_vm() const {
      return root_vm_;
    }

    Memory* memory() const {
      return om;
    }

    bool check_gc_p() {
      bool c = check_gc_;
      if(unlikely(c)) {
        check_gc_ = false;
      }
      return c;
    }

    void gc_soon() {
      check_global_interrupts_ = true;
      check_gc_ = true;
      thread_nexus_->set_stop();
    }

    bool check_global_interrupts() const {
      return check_global_interrupts_;
    }

    void set_check_global_interrupts() {
      check_global_interrupts_ = true;
    }

    void clear_check_global_interrupts() {
      check_global_interrupts_ = false;
    }

    bool* check_global_interrupts_address() {
      return &check_global_interrupts_;
    }

    const unsigned int* object_memory_mark_address() const;

    utilities::thread::Mutex& codedb_lock() {
      return codedb_lock_;
    }

    void set_use_capi_lock(bool s) {
      use_capi_lock_ = s;
    }

    utilities::thread::SpinLock& capi_ds_lock() {
      return capi_ds_lock_;
    }

    utilities::thread::SpinLock& capi_constant_lock() {
      return capi_constant_lock_;
    }

    int capi_lock_index(std::string name);

    utilities::thread::SpinLock& wait_lock() {
      return wait_lock_;
    }

    utilities::thread::SpinLock& type_info_lock() {
      return type_info_lock_;
    }

    utilities::thread::SpinLock& code_resource_lock() {
      return code_resource_lock_;
    }

    void scheduler_loop();

    void after_fork_child(STATE);

    void enter_capi(STATE, const char* file, int line);
    void leave_capi(STATE);

    void setup_capi_constant_names();
    CApiConstantNameMap& capi_constant_name_map() {
      return capi_constant_name_map_;
    }

    CApiConstantHandleMap& capi_constant_map() {
      return capi_constant_map_;
    }

    void initialize_capi_black_list();
  };
}

#endif
