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
  class VMJIT;
  class VMThreadState;

  namespace memory {
    class ManagedThread;
  }

  class State {
    VM* vm_;
    VMJIT* vm_jit_;
    SharedState& shared_;

  public:
    State(VM* vm)
      : vm_(vm)
      , vm_jit_(&vm->vm_jit_)
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

    void set_call_site_information(CallSiteInformation* info) {
      vm_->set_call_site_information(info);
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

    bool detect_stack_condition(void* end) const {
      return vm_->detect_stack_condition(end);
    }

    bool check_local_interrupts() const {
      return vm_jit_->check_local_interrupts_;
    }

    bool check_async(STATE) {
      if(vm_->check_local_interrupts()) {
        return process_async(state);
      }
      return true;
    }

    void raise_stack_error(STATE);

    bool check_stack(STATE, void* end) {
      // @TODO assumes stack growth direction
      if(unlikely(vm_->detect_stack_condition(end))) {
        raise_stack_error(state);
        return false;
      }
      return true;
    }

    bool process_async(STATE);
    bool check_interrupts(STATE);

    Object* park(STATE);
    Object* park_timed(STATE, struct timespec* ts);
  };
}

#endif
