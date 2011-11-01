#ifndef RBX_STATE_HPP
#define RBX_STATE_HPP

namespace rubinius {
  class VM;
  class ManagedThread;
  class ThreadState;

  class State {
    VM* vm_;

  public:
    State(VM* vm)
      : vm_(vm)
    {}

    VM* vm() {
      return vm_;
    }

    ManagedThread* thread() {
      return (ManagedThread*)vm_;
    }

    void set_call_frame(CallFrame* cf) {
      vm_->set_call_frame(cf);
    }

    Globals& globals() {
      return vm_->shared.globals;
    }

    Symbol* symbol(const char* str) {
      return vm_->symbol(str);
    }

    Symbol* symbol(std::string str) {
      return vm_->symbol(str);
    }

    Symbol* symbol(String* str) {
      return vm_->symbol(str);
    }

    template <class T>
      T* new_object(Class *cls) {
        return reinterpret_cast<T*>(vm_->new_object_typed(cls, sizeof(T), T::type));
      }
    
    ThreadState* thread_state() {
      return vm_->thread_state();
    }

    bool detect_stack_condition(void* end) {
      return vm_->detect_stack_condition(end);
    }

    bool check_async(CallFrame* call_frame) {
      if(vm_->check_local_interrupts) {
        return process_async(call_frame);
      }
      return true;
    }

    void raise_stack_error(CallFrame* call_frame);

    bool check_stack(CallFrame* call_frame, void* end) {
      // @TODO assumes stack growth direction
      if(unlikely(reinterpret_cast<uintptr_t>(end) < vm_->stack_limit_)) {
        raise_stack_error(call_frame);
        return false;
      }

      return true;
    }

    bool process_async(CallFrame* call_frame);
    void check_exception(CallFrame* call_frame);
    bool check_interrupts(GCToken gct, CallFrame* call_frame, void* end);

    gc::Slab& local_slab() {
      return vm_->local_slab();
    }

    void stop_the_world() {
      vm_->shared.stop_the_world(vm_);
    }

    void restart_world() {
      vm_->shared.restart_world(vm_);
    }

    void gc_independent() {
      vm_->shared.gc_independent(vm_);
    }

    void gc_dependent() {
      vm_->shared.gc_dependent(vm_);
    }

    void checkpoint() {
      vm_->shared.checkpoint(vm_);
    }

  };
}

#endif
