#ifndef RBX_STATE_HPP
#define RBX_STATE_HPP

#include "type_info.hpp"

#include <string.h>
#include <string>

namespace rubinius {
  struct CallFrame;

  class C_API;
  class Class;
  class Console;
  class Environment;
  class Exception;
  class Object;
  class Machine;
  class MachineState;
  class Memory;
  class Profiler;
  class SignalThread;
  class String;
  class Symbol;
  class VM;
  class UnwindState;

  namespace memory {
    class Collector;
    class ManagedThread;
  }

  class State {
    VM* vm_;

  public:
    State(VM* vm)
      : vm_(vm)
    {}

    MachineState* const machine_state();
    Machine* const machine();
    Configuration* const configuration();
    Environment* const environment();
    ThreadNexus* const thread_nexus();
    Diagnostics* const diagnostics();
    MachineThreads* const machine_threads();
    memory::Collector* const collector();
    SignalThread* const signals();
    Memory* const memory();
    C_API* const c_api();
    Profiler* const profiler();
    Console* const console();

    VM* vm() {
      return vm_;
    }

    memory::ManagedThread* thread() {
      return static_cast<memory::ManagedThread*>(vm_);
    }

    Object* raise_exception(Exception* exc);

    void set_vm(VM* vm) {
      vm_ = vm;
    }

    Globals& globals();

    Symbol* const symbol(const char* str);
    Symbol* const symbol(const char* str, size_t len);
    Symbol* const symbol(std::string str);
    Symbol* const symbol(String* str);

    const uint32_t hash_seed();

    UnwindState* unwind_state() {
      return vm_->unwind_state(this);
    }

    bool check_local_interrupts() const {
      return vm_->check_local_interrupts();
    }

    void raise_stack_error();

    Object* park(STATE);
    Object* park_timed(STATE, struct timespec* ts);
  };
}

#endif
