#ifndef RBX_STATE_HPP
#define RBX_STATE_HPP

#include "type_info.hpp"

#include <string.h>
#include <string>

namespace rubinius {
  struct CallFrame;

  class Class;
  class Exception;
  class Object;
  class Memory;
  class SharedState;
  class String;
  class Symbol;
  class VM;
  class VMThreadState;

  namespace memory {
    class Collector;
    class ManagedThread;
  }

  class State {
    VM* vm_;
    SharedState& shared_;

  public:
    State(VM* vm)
      : vm_(vm)
      , shared_(vm->shared)
    {}

    Configuration* const configuration();
    ThreadNexus* const thread_nexus();
    MachineThreads* const machine_threads();
    memory::Collector* const collector();
    Memory* const memory();

    VM* vm() {
      return vm_;
    }

    memory::ManagedThread* thread() {
      return static_cast<memory::ManagedThread*>(vm_);
    }

    Object* raise_exception(Exception* exc) {
      vm_->thread_state()->raise_exception(exc);
      return 0;
    }

    void set_vm(VM* vm) {
      vm_ = vm;
    }

    Globals& globals();

    Symbol* const symbol(const char* str);
    Symbol* const symbol(const char* str, size_t len);
    Symbol* const symbol(std::string str);
    Symbol* const symbol(String* str);

    uint32_t hash_seed() const {
      return shared_.hash_seed;
    }

    VMThreadState* thread_state() {
      return vm_->thread_state();
    }

    SharedState& shared() {
      return shared_;
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
