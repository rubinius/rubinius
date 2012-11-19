#ifndef RBX_STATE_HPP
#define RBX_STATE_HPP

namespace rubinius {
  class VM;
  class ManagedThread;
  class ThreadState;
  class SharedState;

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

    ManagedThread* thread() {
      return reinterpret_cast<ManagedThread*>(vm_);
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

    Globals& globals() {
      return shared_.globals;
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

    uint32_t hash_seed() {
      return shared_.hash_seed;
    }

    template <class T>
      T* new_object(Class *cls) {
        return reinterpret_cast<T*>(vm_->new_object_typed(cls, sizeof(T), T::type));
      }

    ThreadState* thread_state() {
      return vm_->thread_state();
    }

    ObjectMemory* memory() {
      return shared_.memory();
    }

    SharedState& shared() {
      return shared_;
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
      if(vm_->stack_limit_ == vm_->stack_start_) {
        vm_->reset_stack_limit();
      } else {
        if(unlikely(reinterpret_cast<uintptr_t>(end) < vm_->stack_limit_)) {
          raise_stack_error(call_frame);
          return false;
        }
      }
      return true;
    }

    bool process_async(CallFrame* call_frame);
    void check_exception(CallFrame* call_frame);
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

    void gc_independent(GCToken gct) {
      shared_.gc_independent(vm_);
    }

    void gc_dependent() {
      shared_.gc_dependent(vm_);
    }

    void checkpoint(GCToken gct, CallFrame* call_frame) {
      vm_->set_call_frame(call_frame);
      gc_checkpoint(gct, call_frame);
      shared_.checkpoint(vm_);
    }

    void gc_checkpoint(GCToken gct, CallFrame* frame) {
      if(unlikely(shared_.check_gc_p())) {
        vm_->collect_maybe(gct, frame);
      }
    }

    void lock(GCToken gct) {
      gc_independent(gct);
      vm_->lock(vm_);
      gc_dependent();
    }

    void unlock() {
      vm_->unlock(vm_);
    }

    Object* park(GCToken gct, CallFrame* call_frame);

    Object* park_timed(GCToken gct, CallFrame* call_frame, struct timespec* ts);
  };
}

#endif
