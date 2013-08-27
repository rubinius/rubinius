#ifndef RBX_STATE_HPP
#define RBX_STATE_HPP

#include <string.h>

namespace rubinius {
  class VM;
  class VMJIT;
  class ManagedThread;
  class VMThreadState;
  class SharedState;

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

    void set_call_frame(CallFrame* cf) {
      vm_->set_call_frame(cf);
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
      T* new_object(Class *cls) {
        return static_cast<T*>(vm_->new_object_typed(cls, sizeof(T), T::type));
      }

    template <class T>
      T* new_object_dirty(Class *cls) {
        return static_cast<T*>(vm_->new_object_typed_dirty(cls, sizeof(T), T::type));
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
      set_call_frame(call_frame);
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

    bool stop_the_world() WARN_UNUSED {
      return shared_.stop_the_world(vm_);
    };

    void restart_world() {
      shared_.restart_world(vm_);
    }

    void gc_independent(GCToken gct, CallFrame* call_frame) {
      shared_.gc_independent(this, call_frame);
    }

    void gc_dependent(GCToken gct, CallFrame* call_frame) {
      shared_.gc_dependent(this, call_frame);
    }

    void checkpoint(GCToken gct, CallFrame* call_frame) {
      set_call_frame(call_frame);
      gc_checkpoint(gct, call_frame);
      shared_.checkpoint(vm_);
    }

    void gc_checkpoint(GCToken gct, CallFrame* frame) {
      if(unlikely(shared_.check_gc_p())) {
        vm_->collect_maybe(gct, frame);
      }
    }

    void lock(GCToken gct, CallFrame* call_frame) {
      gc_independent(gct, call_frame);
      vm_->lock(vm_);
      gc_dependent(gct, call_frame);
    }

    void unlock() {
      vm_->unlock(vm_);
    }

    Object* park(GCToken gct, CallFrame* call_frame);

    Object* park_timed(GCToken gct, CallFrame* call_frame, struct timespec* ts);
  };
}

#endif
