#ifndef RBX_SHARED_STATE_H
#define RBX_SHARED_STATE_H

#include "config.h"

#include "memory/variable_buffer.hpp"
#include "memory/root_buffer.hpp"

#include "globals.hpp"
#include "machine_threads.hpp"
#include "primitives.hpp"
#include "spinlock.hpp"
#include "symbol_table.hpp"
#include "thread_nexus.hpp"

#include "util/thread.hpp"

#include <unistd.h>
#include <atomic>
#include <functional>
#include <mutex>
#include <string>
#include <unordered_set>
#include <vector>

#ifdef RBX_WINDOWS
#include <winsock2.h>
#endif

namespace rubinius {
  namespace memory {
    class Collector;
    class ManagedThread;
  }

  class Machine;
  class ConfigParser;
  class Configuration;
  class Environment;
  class Fiber;
  class Fixnum;
  class Memory;
  class QueryAgent;
  class State;
  class VM;

  struct CallFrame;

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

    uint64_t start_time_;
    uint64_t  class_count_;
    int global_serial_;

    bool initialized_;
    bool check_global_interrupts_;
    bool check_gc_;

    VM* root_vm_;
    Environment* env_;

    std::recursive_mutex codedb_lock_;

    utilities::thread::SpinLock wait_lock_;
    utilities::thread::SpinLock type_info_lock_;
    utilities::thread::SpinLock code_resource_lock_;

    int primitive_hits_[Primitives::cTotalPrimitives];

    std::atomic<Phase> phase_;

    Machine* machine_;

  public:
    ConfigParser& user_variables;
    uint32_t hash_seed;

  public:
    SharedState(Environment* env, Machine* m, ConfigParser& cp);
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
      initialized_ = true;
    }

    double run_time();

    Machine* const machine() {
      return machine_;
    }

    ThreadNexus* const thread_nexus();
    MachineThreads* const machine_threads();

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

    Environment* const env() {
      return env_;
    }

    void set_root_vm(VM* vm) {
      root_vm_ = vm;
    }

    VM* root_vm() const {
      return root_vm_;
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
      thread_nexus()->set_stop();
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

    std::recursive_mutex& codedb_lock() {
      return codedb_lock_;
    }

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

  };
}

#endif
