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

    Globals& globals() {
      return shared_.globals;
    }

    Symbol* symbol(const char* str) {
      return shared_.symbols.lookup(this, str, strlen(str));
    }

    Symbol* symbol(const char* str, size_t len) {
      return shared_.symbols.lookup(this, str, len);
    }

    Symbol* symbol(std::string str) {
      return shared_.symbols.lookup(this, str);
    }

    Symbol* symbol(String* str) {
      return shared_.symbols.lookup(this, str);
    }

    uint32_t hash_seed() const {
      return shared_.hash_seed;
    }

    VMThreadState* thread_state() {
      return vm_->thread_state();
    }

    Memory* memory() {
      return shared_.memory();
    }

    SharedState& shared() {
      return shared_;
    }

    bool check_local_interrupts() const {
      return vm_->check_local_interrupts();
    }

    void raise_stack_error(STATE);

    Object* park(STATE);
    Object* park_timed(STATE, struct timespec* ts);
  };
}

#endif
