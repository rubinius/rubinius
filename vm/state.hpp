#ifndef RBX_STATE_HPP
#define RBX_STATE_HPP

#include "type_info.hpp"

#include <string.h>
#include <string>

namespace rubinius {
  struct CallFrame;

  class Class;
  class Exception;
  class ManagedThread;
  class Object;
  class ObjectMemory;
  class SharedState;
  class String;
  class Symbol;
  class VM;
  class VMJIT;
  class VMThreadState;

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

    ManagedThread* thread() {
      return static_cast<ManagedThread*>(vm_);
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

    template <class T>
      T* new_object(Class* cls) {
        return static_cast<T*>(vm_->new_object_typed(cls, sizeof(T), T::type));
      }

    template <class T>
      T* new_object_dirty(Class* cls) {
        return static_cast<T*>(vm_->new_object_typed_dirty(cls, sizeof(T), T::type));
      }

    template <class T>
      T* new_object_pinned(Class* cls) {
        T* obj = static_cast<T*>(vm_->new_object_typed_mature(cls, sizeof(T), T::type));
        obj->pin();

        return obj;
      }

    VMThreadState* thread_state() {
      return vm_->thread_state();
    }

    ObjectMemory* memory() {
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

    bool check_async(CallFrame* call_frame) {
      vm_->set_call_frame(call_frame);
      if(vm_->check_local_interrupts()) {
        return process_async(call_frame);
      }
      return true;
    }

    void raise_stack_error(CallFrame* call_frame);

    bool check_stack(CallFrame* call_frame, void* end) {
      // @TODO assumes stack growth direction
      if(unlikely(vm_->detect_stack_condition(end))) {
        raise_stack_error(call_frame);
        return false;
      }
      return true;
    }

    bool process_async(CallFrame* call_frame);
    bool check_interrupts(GCToken gct, CallFrame* call_frame, void* end);

    gc::Slab& local_slab() {
      return vm_->local_slab();
    }

    Object* park(GCToken gct, CallFrame* call_frame);

    Object* park_timed(GCToken gct, CallFrame* call_frame, struct timespec* ts);
  };
}

#endif
