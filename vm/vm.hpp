#ifndef RBX_VM_H
#define RBX_VM_H

#include "missing/time.h"

#include "vm_jit.hpp"
#include "globals.hpp"
#include "gc/object_mark.hpp"
#include "gc/managed.hpp"
#include "vm_thread_state.hpp"

#include "util/refcount.hpp"
#include "util/thread.hpp"

#include "gc/variable_buffer.hpp"
#include "gc/root_buffer.hpp"
#include "gc/slab.hpp"

#include "shared_state.hpp"

#include "unwind_info.hpp"
#include "fiber_stack.hpp"

#include <vector>
#include <setjmp.h>

namespace llvm {
  class Module;
}

namespace rbxti {
  class Env;
}

namespace rubinius {

  class Exception;
  class LLVMState;

  namespace event {
    class Loop;
  }

  namespace gc {
    class WriteBarrier;
  }

  class Channel;
  class GlobalCache;
  class Primitives;
  class ObjectMemory;
  class TypeInfo;
  class String;
  class Symbol;
  class ConfigParser;
  class TypeError;
  class Assertion;
  struct CallFrame;
  class CallSiteInformation;
  class Object;
  class Configuration;
  class VMManager;
  class LookupTable;
  class SymbolTable;
  class SharedState;
  class Fiber;
  class GarbageCollector;
  class Park;

  enum MethodMissingReason {
    eNone, ePrivate, eProtected, eSuper, eVCall, eNormal
  };

  enum ConstantMissingReason {
    vFound, vPrivate, vNonExistent
  };

  /**
   * Represents an execution context for running Ruby code.
   *
   * Each Ruby thread is backed by an instance of this class, as well as an
   * instance of the Thread class. Thread manages the (Ruby visible) thread-
   * related state, while this class manages the execution machinery for
   * running Ruby code.
   */

  class VM : public ManagedThread {
    friend class State;
    friend class VMJIT;

  private:
    UnwindInfoSet unwinds_;

    CallFrame* saved_call_frame_;
    CallSiteInformation* saved_call_site_information_;
    FiberStacks fiber_stacks_;
    Park* park_;
    rbxti::Env* tooling_env_;

    uintptr_t root_stack_start_;
    uintptr_t root_stack_size_;

    VMJIT vm_jit_;

    MethodMissingReason method_missing_reason_;
    ConstantMissingReason constant_missing_reason_;

    bool run_signals_;
    bool tooling_;
    bool allocation_tracking_;

  public:
    /* Data members */
    SharedState& shared;
    TypedRoot<Channel*> waiting_channel_;
    TypedRoot<Exception*> interrupted_exception_;
    /// The Thread object for this VM state
    TypedRoot<Thread*> thread;

    /// The current fiber running on this thread
    TypedRoot<Fiber*> current_fiber;

    /// Root fiber, if any (lazily initialized)
    TypedRoot<Fiber*> root_fiber;

    InflatedHeader* waiting_header_;

    void (*custom_wakeup_)(void*);
    void* custom_wakeup_data_;

    ObjectMemory* om;

    VMThreadState thread_state_;

    static unsigned long cStackDepthMax;

  public: /* Inline methods */

    UnwindInfoSet& unwinds() {
      return unwinds_;
    }

    uint32_t thread_id() const {
      return id_;
    }

    bool run_signals_p() const {
      return run_signals_;
    }

    void set_run_signals(bool val) {
      run_signals_ = val;
    }

    VMThreadState* thread_state() {
      return &thread_state_;
    }

    CallFrame** call_frame_location() {
      return &saved_call_frame_;
    }

    void set_call_frame(CallFrame* frame) {
      saved_call_frame_ = frame;
    }

    CallFrame* saved_call_frame() const {
      return saved_call_frame_;
    }

    void set_call_site_information(CallSiteInformation* info) {
      saved_call_site_information_ = info;
    }

    CallSiteInformation* saved_call_site_information() {
      return saved_call_site_information_;
    }

    GlobalCache* global_cache() const {
      return shared.global_cache;
    }

    Globals& globals() {
      return shared.globals;
    }

    void* stack_start() const {
      return reinterpret_cast<void*>(vm_jit_.stack_start_);
    }

    int stack_size() const {
      return vm_jit_.stack_size_;
    }

    void reset_stack_limit() {
      // @TODO assumes stack growth direction
      vm_jit_.stack_limit_ = (vm_jit_.stack_start_ - vm_jit_.stack_size_) + (4096 * 10);
    }

    void set_stack_bounds(uintptr_t start, int length) {
      if(start == 0) {
        start  = root_stack_start_;
        length = root_stack_size_;
      }

      vm_jit_.stack_start_ = start;
      vm_jit_.stack_size_ = length;
      reset_stack_limit();
    }

    void set_root_stack(uintptr_t start, int length) {
      root_stack_start_ = start;
      root_stack_size_ = length;

      set_stack_bounds(root_stack_start_, root_stack_size_);
    }

    bool detect_stack_condition(void* end) const {
      // @TODO assumes stack growth direction
      return reinterpret_cast<uintptr_t>(end) < vm_jit_.stack_limit_;
    }

    MethodMissingReason method_missing_reason() const {
      return method_missing_reason_;
    }

    void set_method_missing_reason(MethodMissingReason reason) {
      method_missing_reason_ = reason;
    }

    ConstantMissingReason constant_missing_reason() const {
      return constant_missing_reason_;
    }

    void set_constant_missing_reason(ConstantMissingReason reason) {
      constant_missing_reason_ = reason;
    }

    bool thread_step() const {
      return vm_jit_.thread_step_;
    }

    void clear_thread_step() {
      clear_check_local_interrupts();
      vm_jit_.thread_step_ = false;
    }

    void set_thread_step() {
      set_check_local_interrupts();
      vm_jit_.thread_step_ = true;
    }

    bool check_local_interrupts() const {
      return vm_jit_.check_local_interrupts_;
    }

    void clear_check_local_interrupts() {
      vm_jit_.check_local_interrupts_ = false;
    }

    void set_check_local_interrupts() {
      vm_jit_.check_local_interrupts_ = true;
    }

    bool interrupt_by_kill() const {
      return vm_jit_.interrupt_by_kill_;
    }

    void clear_interrupt_by_kill() {
      vm_jit_.interrupt_by_kill_ = false;
    }

    void set_interrupt_by_kill() {
      vm_jit_.interrupt_by_kill_ = true;
    }

    Exception* interrupted_exception() const {
      return interrupted_exception_.get();
    }

    void clear_interrupted_exception() {
      interrupted_exception_.set(cNil);
    }

    rbxti::Env* tooling_env() const {
      return tooling_env_;
    }

    bool tooling() const {
      return tooling_;
    }

    void enable_tooling() {
      tooling_ = true;
    }

    void disable_tooling() {
      tooling_ = false;
    }

    bool allocation_tracking() const {
      return allocation_tracking_;
    }

    void enable_allocation_tracking() {
      allocation_tracking_ = true;
    }

    void disable_allocation_tracking() {
      allocation_tracking_ = false;
    }

    FiberStack* allocate_fiber_stack() {
      return fiber_stacks_.allocate();
    }

    void* fiber_trampoline() {
      return fiber_stacks_.trampoline();
    }

    FiberData* new_fiber_data(bool root=false) {
      return fiber_stacks_.new_data(root);
    }

    void remove_fiber_data(FiberData* data) {
      fiber_stacks_.remove_data(data);
    }

    VariableRootBuffers& current_root_buffers();

  public:
    static void init_stack_size();

    static VM* current();
    static void set_current(VM* vm, std::string name);

    static void discard(STATE, VM*);

  public:

    /* Prototypes */
    VM(uint32_t id, SharedState& shared);
    ~VM();

    void initialize_as_root();

    void bootstrap_class(STATE);
    void bootstrap_ontology(STATE);
    void bootstrap_symbol(STATE);
    void initialize_config();

    void setup_errno(STATE, int num, const char* name, Class* sce, Module* ern);
    void bootstrap_exceptions(STATE);
    void initialize_fundamental_constants(STATE);
    void initialize_builtin_classes(STATE);
    void initialize_platform_data(STATE);

    void set_current_fiber(Fiber* fib);

    Object* new_object_typed_dirty(Class* cls, size_t bytes, object_type type);
    Object* new_object_typed(Class* cls, size_t bytes, object_type type);
    Object* new_object_typed_mature(Class* cls, size_t bytes, object_type type);

    template <class T>
      T* new_object(Class *cls) {
        return static_cast<T*>(new_object_typed(cls, sizeof(T), T::type));
      }

    template <class T>
      T* new_object_mature(Class *cls) {
        return static_cast<T*>(new_object_typed_mature(cls, sizeof(T), T::type));
      }

    template <class T>
      T* new_object_bytes_dirty(Class* cls, size_t& bytes) {
        bytes = ObjectHeader::align(sizeof(T) + bytes);
        return static_cast<T*>(new_object_typed_dirty(cls, bytes, T::type));
      }

    template <class T>
      T* new_object_bytes(Class* cls, size_t& bytes) {
        bytes = ObjectHeader::align(sizeof(T) + bytes);
        return static_cast<T*>(new_object_typed(cls, bytes, T::type));
      }

    template <class T>
      T* new_object_variable(Class* cls, size_t fields, size_t& bytes) {
        bytes = T::fields_offset + (fields * sizeof(Object*));
        return static_cast<T*>(new_object_typed(cls, bytes, T::type));
      }

    /// Create a String in the young GC space, return NULL if not possible.
    String* new_young_string_dirty();

    /// Create a Tuple in the young GC space, return NULL if not possible.
    Tuple* new_young_tuple_dirty(size_t fields);

    TypeInfo* find_type(int type);

    void init_ffi(STATE);
    void init_native_libraries();

    /// Check the GC flags in ObjectMemory and collect if we need to.
    void collect_maybe(GCToken gct, CallFrame* call_frame);

    void raise_from_errno(const char* reason);
    void raise_exception(Exception* exc);
    Exception* new_exception(Class* cls, const char* msg);
    Object* current_block();

    void set_const(const char* name, Object* val);
    void set_const(Module* mod, const char* name, Object* val);

    Object* path2class(const char* name);

#ifdef ENABLE_LLVM
    llvm::Module* llvm_module();
    void llvm_cleanup();
#endif

    void print_backtrace();


    /// Run the garbage collectors as soon as you can
    void run_gc_soon();

    void wait_on_channel(Channel* channel);
    void wait_on_inflated_lock(InflatedHeader* ih);
    void wait_on_custom_function(void (*func)(void*), void* data);
    void clear_waiter();
    bool wakeup(STATE, GCToken gct, CallFrame* call_frame);

    void set_parked();
    void set_unparked();

    void set_sleeping();
    void clear_sleeping();

    void interrupt_with_signal();
    bool should_interrupt_with_signal() const {
      return vm_jit_.interrupt_with_signal_;
    }

    void register_raise(STATE, Exception* exc);
    void register_kill(STATE);

    void gc_scan(GarbageCollector* gc);
    void gc_fiber_clear_mark();
    void gc_fiber_scan(GarbageCollector* gc, bool only_marked = true);
    void gc_verify(GarbageCollector* gc);
  };

}

#include "state.hpp"

namespace rubinius {


  /**
   * Instantiation of an instance of this class causes Ruby execution on all
   * threads to be suspended. Upon destruction of the instance, Ruby execution
   * is resumed.
   */

  class StopTheWorld {
    State* state_;

  public:
    StopTheWorld(STATE, GCToken gct, CallFrame* cf) :
      state_(state)
    {
      while(!state->stop_the_world()) {
        state->checkpoint(gct, cf);
      }
    }

    ~StopTheWorld() {
      state_->restart_world();
    }
  };

  class NativeMethodEnvironment;

  class GCIndependent {
    State* state_;

  public:
    GCIndependent(STATE, CallFrame* call_frame)
      : state_(state)
    {
      GCTokenImpl gct;
      state_->gc_independent(gct, call_frame);
    }

    GCIndependent(NativeMethodEnvironment* env);

    ~GCIndependent() {
      GCTokenImpl gct;
      state_->gc_dependent(gct, state_->vm()->saved_call_frame());
    }
  };

  template <class T>
  class GCIndependentLockGuard : public utilities::thread::LockGuardTemplate<T> {
    State* state_;
  public:
    GCIndependentLockGuard(STATE, GCToken gct, CallFrame* call_frame, T& in_lock)
      : utilities::thread::LockGuardTemplate<T>(in_lock, false)
      , state_(state)
    {
      state_->shared().gc_independent(state_, call_frame);
      this->lock();
      state->shared().gc_dependent(state_, call_frame);
    }

    ~GCIndependentLockGuard() {
      this->unlock();
    }
  };

   typedef GCIndependentLockGuard<utilities::thread::Mutex> GCLockGuard;
};

#endif
